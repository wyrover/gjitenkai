#include <glib.h>
#include <glib/gprintf.h>

#include "worddic.h"
#include "worddic_dicfile.h"
#include "preferences.h"
#include "../common/dicfile.h"

G_MODULE_EXPORT gboolean on_search_results_button_release_event(GtkWidget *text_view,
                                                                GdkEventButton *event,
                                                                worddic *worddic) {
  return FALSE;
}

/*
 * Update the cursor image if the pointer is above a kanji. 
 */
G_MODULE_EXPORT gboolean on_search_results_motion_notify_event(GtkWidget *text_view,
                                                               GdkEventMotion *event){
  gint x, y;
  GtkTextIter mouse_iter;
  gunichar kanji;
  gint trailing;
  
  gtk_text_view_window_to_buffer_coords(GTK_TEXT_VIEW(text_view), 
                                        GTK_TEXT_WINDOW_WIDGET,
                                        event->x, event->y, &x, &y);
  
  gtk_text_view_get_iter_at_position(GTK_TEXT_VIEW(text_view),
                                     &mouse_iter, &trailing,
                                     x , y);
  
  kanji = gtk_text_iter_get_char(&mouse_iter);

  // Change the cursor if necessary
  if ((isKanjiChar(kanji) == TRUE)) {
    gdk_window_set_cursor(gtk_text_view_get_window(GTK_TEXT_VIEW(text_view),
                                                   GTK_TEXT_WINDOW_TEXT),
                          cursor_selection);
  }
  else{
    gdk_window_set_cursor(gtk_text_view_get_window(GTK_TEXT_VIEW(text_view),
                                                   GTK_TEXT_WINDOW_TEXT),
                          cursor_default);
  }
  
  return FALSE;
}

/**
   search entry activate signal callback:
   Search in the dictionaries the entered text in the search entry
   and put the results in the search result textview buffer
*/
G_MODULE_EXPORT void on_search_activate(GtkEntry *entry, worddic *worddic){

  //wait if a dictionary is being loaded in a thread
  if(worddic->thread_load_dic){
    g_thread_join(worddic->thread_load_dic);
  }
  
  gint match_criteria_jp  = worddic->match_criteria_jp;
  gint match_criteria_lat  = worddic->match_criteria_lat;

  //clear the last search results
  worddic->results = g_list_first(worddic->results);
  g_list_free_full(worddic->results, (GDestroyNotify)dicresult_free);
  worddic->results = NULL;
  
  //get the expression to search from the search entry
  const gchar *search_entry_text = gtk_entry_get_text(entry);
  if(!strcmp(search_entry_text, ""))return;
  
  //detect is the search is in japanese
  gboolean is_jp = detect_japanese(search_entry_text);
  
  //get the modified string with anchors from the GString
  //convert fullwidth regex punctuation to halfwidth regex puncutation
  gchar *entry_text = regex_full_to_half(search_entry_text);

  //get the search result text entry to display matches
  GtkTextBuffer *textbuffer_search_results = 
    (GtkTextBuffer*)gtk_builder_get_object(worddic->definitions, 
                                           "textbuffer_search_results");
  
  gboolean deinflection   = worddic->conf->verb_deinflection;

  //search in the dictionaries
  GSList *dicfile_node;
  WorddicDicfile *dicfile;
  dicfile_node = worddic->conf->dicfile_list;    //matched dictionary entries
  GList *results=NULL;

  //clear the display result buffer
  gtk_text_buffer_set_text(textbuffer_search_results, "", 0);

  //in each dictionaries
  gint i=0;
  while (dicfile_node != NULL) {
    dicfile = dicfile_node->data; 

    //do not search in this dictionary if it's not active
    if(!dicfile->is_active){
      dicfile_node = g_slist_next(dicfile_node);
      i++;
      continue;
    }
    

    //if this dictionary was not loaded, parse it now
    if(!dicfile->is_loaded){
      worddic_dicfile_open(dicfile);
      worddic_dicfile_parse_all(dicfile);
      worddic_dicfile_close(dicfile);

      dicfile->is_loaded = TRUE;

      GtkListStore *model = (GtkListStore*)gtk_builder_get_object(worddic->definitions, 
                                                                  "liststore_dic");
      GtkTreeIter  iter;
      GtkTreePath *path = gtk_tree_path_new_from_indices (i, -1);
      
      //set the model
      gtk_tree_model_get_iter (GTK_TREE_MODEL(model), &iter, path);
      gtk_list_store_set (GTK_LIST_STORE (model), &iter, COL_LOADED, TRUE, -1);

      gtk_tree_path_free (path);
    }
    
    if(is_jp){
      //search for deinflections
      if(deinflection){
        results = g_list_concat(results,
                                search_inflections(dicfile, entry_text));
      }

      //search hiragana on katakana
      if (worddic->conf->search_hira_on_kata &&
          hasKatakanaString(entry_text)) {
        gchar *hiragana = kata_to_hira(entry_text);
        results = g_list_concat(results, dicfile_search(dicfile,
                                                        hiragana,
                                                        "from katakana",
                                                        GIALL,
                                                        match_criteria_jp,
                                                        match_criteria_lat,
                                                        1)
                                );
        g_free(hiragana);  //free memory
      }
    
      //search katakana on hiragana
      if (worddic->conf->search_kata_on_hira &&
          hasHiraganaString(entry_text)) { 
        gchar *katakana = hira_to_kata(entry_text);
        results = g_list_concat(results, dicfile_search(dicfile,
                                                        katakana,
                                                        "from hiragana",
                                                        GIALL,
                                                        match_criteria_jp,
                                                        match_criteria_lat,
                                                        1)
                                );
        g_free(katakana); //free memory
      }
    }

    //standard search
    results = g_list_concat(results, dicfile_search(dicfile,
                                                    entry_text,
                                                    NULL,
                                                    GIALL,
                                                    match_criteria_jp,
                                                    match_criteria_lat,
                                                    is_jp)
                            );
    
    //get the next node in the dic list
    dicfile_node = g_slist_next(dicfile_node);
    i++;
  }

  worddic->results = results;

  //print the first page
  print_entries(textbuffer_search_results, worddic);

    
  //free memory
  g_free(entry_text);
}

//////////////////////
//Menuitems callbacks

//Edit
///Preferences
G_MODULE_EXPORT void on_menuitem_prefs_activate(GtkMenuItem *menuitem, worddic *worddic){
  GtkDialog *prefs = GTK_DIALOG(gtk_builder_get_object(worddic->definitions, 
                                                       "prefs"));
  //set size and display the preference window
  gtk_window_set_default_size(GTK_WINDOW(prefs), 320, 220);
  gtk_widget_show_all ((GtkWidget*)prefs);
}

//Search
///Japanese
G_MODULE_EXPORT void on_menuitem_search_japanese_exact_activate (GtkMenuItem *menuitem, 
                                                                 worddic *worddic){
  worddic->match_criteria_jp = EXACT_MATCH;
}

G_MODULE_EXPORT void on_menuitem_search_japanese_start_activate (GtkMenuItem *menuitem, 
                                                                 worddic *worddic){
  worddic->match_criteria_jp = START_WITH_MATCH;
}

G_MODULE_EXPORT void on_menuitem_search_japanese_end_activate (GtkMenuItem *menuitem, 
                                                               worddic *worddic){
  worddic->match_criteria_jp = END_WITH_MATCH;
}

G_MODULE_EXPORT void on_menuitem_search_japanese_any_activate (GtkMenuItem *menuitem, 
                                                               worddic *worddic){
  worddic->match_criteria_jp = ANY_MATCH;
}

///Latin
G_MODULE_EXPORT void on_menuitem_search_latin_whole_expressions_activate (GtkMenuItem *menuitem, 
                                                                          worddic *worddic){
  worddic->match_criteria_lat = EXACT_MATCH;
}

G_MODULE_EXPORT void on_menuitem_search_latin_whole_words_activate (GtkMenuItem *menuitem, 
                                                                    worddic *worddic){
  worddic->match_criteria_lat = WORD_MATCH;
}

G_MODULE_EXPORT void on_menuitem_search_latin_any_matches_activate (GtkMenuItem *menuitem, 
                                                                    worddic *worddic){
  worddic->match_criteria_lat = ANY_MATCH;
}


//Help
///About
G_MODULE_EXPORT void on_menuitem_help_about_activate (GtkMenuItem *menuitem, 
                                                      worddic *worddic){
  GtkWindow *window_about = (GtkWindow*)gtk_builder_get_object(worddic->definitions,
                                                               "aboutdialog");
  gtk_dialog_run(GTK_DIALOG(window_about));
  gtk_widget_hide (GTK_WIDGET(window_about)); 
}


//if available for this version of GTK, display the next page of result when
//the scroll window is scrolled at the bottom
#if GTK_MAJOR_VERSION >= 3 && GTK_MINOR_VERSION >= 16
void G_MODULE_EXPORT on_worddic_search_results_edge_reached(GtkScrolledWindow* sw,
							    GtkPositionType pos,
							    worddic* worddic){

  //get the search result text entry to display matches
  GtkTextBuffer *textbuffer_search_results = 
    (GtkTextBuffer*)gtk_builder_get_object(worddic->definitions, 
                                           "textbuffer_search_results");
  if(pos == GTK_POS_BOTTOM){
    print_entries(textbuffer_search_results, worddic);
  }
}
#endif
