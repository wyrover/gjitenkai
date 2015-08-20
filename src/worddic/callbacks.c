#include <glib.h>
#include <glib/gprintf.h>

#include "worddic.h"
#include "worddic_dicfile.h"
#include "preferences.h"
#include "../common/dicfile.h"

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
G_MODULE_EXPORT void on_search_expression_activate(GtkEntry *entry, worddic *worddic){
  //get the expression to search from the search entry
  const gchar *search_entry_text = gtk_entry_get_text(entry);
  if(!strcmp(search_entry_text, ""))return;

  //search for the regex string of the text entry in the dictionaries
  worddic_search(search_entry_text, worddic);
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

//History
////Clear
G_MODULE_EXPORT void on_history_clear_activate (GtkMenuItem *menuitem, 
                                                worddic *worddic){
}

G_MODULE_EXPORT void on_checkbutton_dark_theme_toggled(GtkCheckButton* check_button,  
                                                       worddic *worddic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);

  GtkSettings *settings = gtk_settings_get_default ();
  
  g_object_set (G_OBJECT (settings),
                "gtk-application-prefer-dark-theme",
                toggled,
                NULL);
  
  worddic->conf->dark_theme = toggled;
  worddic_conf_save(worddic->settings, worddic->conf, WSE_DARK_THEME);
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

G_MODULE_EXPORT void on_button_welcome_clicked(GtkButton* button, worddic *p_worddic){
  GtkDialog *dialog = (GtkDialog*)gtk_builder_get_object(p_worddic->definitions, 
                                                         "dialog_welcome");
  gtk_widget_hide (GTK_WIDGET(dialog)); 
}
