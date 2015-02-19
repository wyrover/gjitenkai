#include "worddic.h"
#include "../common/dicfile.h"

/**
   search entry activate signal callback:
   Search in the dictionaries the entered text in the search entry
   and put the results in the search result textview buffer
*/
void on_search_activate(GtkEntry *entry, worddic *worddic){

  //get the expression to search from the search entry
  const gchar *entry_text = gtk_entry_get_text(entry);

  //get the search result text entry to display matches
  GtkTextBuffer *textbuffer_search_results = 
    (GtkTextBuffer*)gtk_builder_get_object(worddic->definitions, 
                                           "textbuffer_search_results");

  //get the search options
  gint match_criteria_jp  = worddic->match_criteria_jp;
  gint match_criteria_lat = worddic->match_criteria_lat;
  gint match_type         = worddic->match_criteria_jp;
  gboolean deinflection   = worddic->conf->verb_deinflection;

  //search in the dictionaries
  GSList *dicfile_node;
  GjitenDicfile *dicfile;
  dicfile_node = worddic->conf->dicfile_list;
  dicfile = dicfile_node->data;
  GList *results=NULL;

  //in each dictionaries
  while (dicfile_node != NULL) {

    dicfile = dicfile_node->data; 
    
    //free previously used dic load in the current dic to mmaped memory
    dicfile_load(dicfile);
    
    ////Special searches
    //search for deinflections
    if(deinflection){
      results = g_list_concat(results,
			      search_verb_inflections(dicfile, entry_text));
    }

    //search katakana on hiragana
    if (worddic->conf->search_kata_on_hira) {
      if (isKatakanaString(entry_text) == TRUE) {
        gchar *hiragana = kata2hira(entry_text);
        results = g_list_concat(results, dicfile_search(dicfile, 
                                                        hiragana, 
                                                        match_criteria_jp, 
                                                        match_criteria_lat, 
                                                        match_type));
        g_free(hiragana);
      }
    }

    //search hiragana on katakana
    if (worddic->conf->search_hira_on_kata) {
      if (isHiraganaString(entry_text) == TRUE) {
        gchar *katakana = hira2kata(entry_text);
        results = g_list_concat(results, dicfile_search(dicfile, 
                                                        katakana,
                                                        match_criteria_jp, 
                                                        match_criteria_lat, 
                                                        match_type));
        g_free(katakana);
      }
    }

    if((match_criteria_lat == REGEX)||
       (match_criteria_jp == REGEX)){
      //regex search
      results = g_list_concat(results, dicfile_search_regex(dicfile, 
							    entry_text));
    }
    else{
      //standard search
      results = g_list_concat(results, dicfile_search(dicfile, 
						      entry_text, 
						      match_criteria_jp, 
						      match_criteria_lat, 
						      match_type));
    }

    //get the next node in the dic list
    dicfile_node = g_slist_next(dicfile_node);
  }

  //clear the display result buffer
  gtk_text_buffer_set_text(textbuffer_search_results, "", 0); 

  //insert all search results via the result buffer
  GList *l;
  for (l = results; l != NULL; l = l->next){
        gtk_text_buffer_insert_at_cursor(textbuffer_search_results, 
                                     l->data, strlen(l->data));
  }

  //highlight the searched expression in the result buffer
  gboolean has_iter;
  GtkTextIter iter, match_start, match_end;
  gtk_text_buffer_get_start_iter (textbuffer_search_results, &iter);
  
  do{
    //search where the result string is located in the result buffer
    has_iter = gtk_text_iter_forward_search (&iter,
                                             entry_text,
                                             GTK_TEXT_SEARCH_VISIBLE_ONLY,
                                             &match_start,
                                             &match_end,
                                             NULL);
    if(has_iter){
      //highlight at this location
      gtk_text_buffer_apply_tag (textbuffer_search_results,
                                 worddic->conf->highlight,
                                 &match_start, 
                                 &match_end);

      //next iteration starts at the end of this iteration
      iter = match_end;
    }

  }while(has_iter);
}

//////////////////////
//Menuitems callbacks

//Edit
///Preferences
void on_menuitem_prefs_activate(GtkMenuItem *menuitem, worddic *worddic){
  GtkDialog *prefs = (GtkWindow*)gtk_builder_get_object(worddic->definitions, 
                                                               "prefs");
  //set size and display the preference window
  gtk_window_set_default_size(GTK_WINDOW(prefs), 320, 220);
  gtk_widget_show_all ((GtkWidget*)prefs);
}

//Search
///Japanese
void on_menuitem_search_japanese_exact_activate (GtkMenuItem *menuitem, 
                                                 worddic *worddic){
  worddic->match_criteria_jp = EXACT_MATCH;
}

void on_menuitem_search_japanese_start_activate (GtkMenuItem *menuitem, 
                                                 worddic *worddic){
  worddic->match_criteria_jp = START_WITH_MATCH;
}

void on_menuitem_search_japanese_end_activate (GtkMenuItem *menuitem, 
                                               worddic *worddic){
  worddic->match_criteria_jp = END_WITH_MATCH;
}

void on_menuitem_search_japanese_any_activate (GtkMenuItem *menuitem, 
                                               worddic *worddic){
  worddic->match_criteria_jp = ANY_MATCH;
}

void on_menuitem_search_japanese_regex_activate (GtkMenuItem *menuitem, 
                                            worddic *worddic){
  worddic->match_criteria_jp = REGEX;
}


///Latin
void on_menuitem_search_latin_whole_expressions_activate (GtkMenuItem *menuitem, 
                                                         worddic *worddic){
  worddic->match_criteria_lat = EXACT_MATCH;
}

void on_menuitem_search_latin_whole_words_activate (GtkMenuItem *menuitem, 
                                                   worddic *worddic){
  worddic->match_criteria_lat = WORD_MATCH;
}

void on_menuitem_search_latin_any_matches_activate (GtkMenuItem *menuitem, 
                                            worddic *worddic){
  worddic->match_criteria_lat = ANY_MATCH;
}

void on_menuitem_search_latin_regex_activate (GtkMenuItem *menuitem, 
                                            worddic *worddic){
  worddic->match_criteria_lat = REGEX;
}


//Help
///About
void on_menuitem_help_about_activate (GtkMenuItem *menuitem, 
                                      worddic *worddic){
  GtkWindow *window_about = (GtkWindow*)gtk_builder_get_object(worddic->definitions,
                                                               "aboutdialog");
  gtk_dialog_run(window_about);
  gtk_widget_hide (window_about); 
}
