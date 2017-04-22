#include <glib.h>
#include <glib/gprintf.h>
#include <libsoup/soup.h>

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

/**
   Callback function when the dictionary has been downloaded.
   write to local drive the content to a file.
*/
static void on_dictionary_download_finished_callback (SoupSession *session,
						      SoupMessage *msg,
						      worddic *p_worddic){
  if(SOUP_STATUS_IS_SUCCESSFUL (msg->status_code)){
    //file path where to save the dictionary
    const gchar *destination = g_strdup_printf("%s/%s", g_get_home_dir(), "edict2u.gz");

    //write to a file
    g_file_set_contents(destination,
			msg->response_body->data,
			msg->response_body->length,
			NULL);

    //create a new worddic dictionary
    WorddicDicfile *dicfile = NULL;
    dicfile = g_new0(WorddicDicfile, 1);
    dicfile->name = g_strdup("edict2u from Monash");
    dicfile->path = destination;
    dicfile->is_loaded = FALSE;
    dicfile->is_active = TRUE;
    p_worddic->conf->dicfile_list = g_slist_append(p_worddic->conf->dicfile_list, dicfile);

    //update the model
    GtkTreeIter iter;
    GtkListStore *store = (GtkListStore*)gtk_builder_get_object(p_worddic->definitions,
								"liststore_dic");
    //insert a new row in the model
    gtk_list_store_insert (store, &iter, -1);
    gtk_list_store_set (store, &iter,
			COL_NAME, dicfile->name,
			COL_PATH, dicfile->path,
			COL_ACTIVE, dicfile->is_active,
			COL_LOADED, dicfile->is_loaded,
			-1);

    worddic_conf_save(p_worddic->settings, p_worddic->conf, WSE_DICFILE);
  }

  //re enable the button in case the user wants to download dictionary again
  GtkButton *button = gtk_builder_get_object(p_worddic->definitions, "button_download_dic");
  gtk_widget_set_sensitive(button, TRUE);
  gtk_button_set_label(button, "Download");
}

static void got_chunk (SoupMessage *msg, SoupBuffer *chunk, worddic *p_worddic){
  gdouble resp_len = 5925214;
  g_printf("> %.2f / %.2f = %.2f\n",
	   (gdouble)msg->response_body->length,
	   (gdouble)resp_len,
	   (gdouble) msg->response_body->length / resp_len);

  GtkProgressBar *pbar = (GtkProgressBar*) gtk_builder_get_object(p_worddic->definitions,
								  "progressbar_download_dic");
  gtk_progress_bar_set_fraction(pbar, ((gdouble)msg->response_body->length / (gdouble)resp_len));
}

G_MODULE_EXPORT void on_button_download_clicked(GtkButton* button, worddic *p_worddic){
  //disable the button to prevent multiple click
  gtk_widget_set_sensitive(button, FALSE);
  gtk_button_set_label(button, "Downloading ...");

  //the remote dictionary location is hard coded. TODO put dictionary location in a file or
  //in a GSettings variable and create UI of a true download manager with several possible
  //locations
  const char *download_url = "http://ftp.monash.edu/pub/nihongo/edict2u.gz";
  SoupSession *session = soup_session_new();
  SoupMessage *msg = soup_message_new ("GET", download_url);
  soup_session_queue_message (session, msg, on_dictionary_download_finished_callback, p_worddic);

  g_object_connect (msg,
		    "signal::got-chunk", got_chunk, p_worddic,
		    NULL);
}

G_MODULE_EXPORT void on_button_welcome_clicked(GtkButton* button, worddic *p_worddic){
  GtkDialog *dialog = (GtkDialog*)gtk_builder_get_object(p_worddic->definitions,
                                                         "dialog_welcome");
  gtk_widget_hide (GTK_WIDGET(dialog));
}
