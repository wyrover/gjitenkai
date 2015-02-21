#include "worddic.h"

void worddic_init (worddic *worddic)
{
  GError *err = NULL;
  worddic->definitions = gtk_builder_new ();
  gtk_builder_add_from_file (worddic->definitions,
                             UI_DEFINITIONS_FILE_WORDDIC, &err);
  if (err != NULL) {
    g_printerr
      ("Error while loading worddic definitions file: %s\n",
       err->message);
    g_error_free (err);
    gtk_main_quit ();
  }
  gtk_builder_connect_signals (worddic->definitions, worddic);

  //init the configuration handler
  conf_init_handler();

  //load configuration 
  worddic->conf = conf_load();

  //by default search everything
  worddic->match_criteria_jp = ANY_MATCH;
  worddic->match_criteria_lat = ANY_MATCH;

  //highlight style of the result text buffer
  GtkTextBuffer*textbuffer_search_results = (GtkTextBuffer*)
    gtk_builder_get_object(worddic->definitions, 
                           "textbuffer_search_results");
  
  GtkTextTag *highlight = gtk_text_buffer_create_tag (textbuffer_search_results,
                                                      "results_highlight",
                                                      "foreground-rgba",
                                                      worddic->conf->results_highlight_color,
                                                      NULL);
  worddic->conf->highlight = highlight;

  //init the verb de-inflection mechanism
  Verbinit();

  //Init the preference window's widgets
  init_prefs_window(worddic);

  // enable clickable kanji
  //g_signal_connect(G_OBJECT(wordDic->text_results_view), "button-release-event", G_CALLBACK(kanji_clicked), worddic);
  //g_signal_connect(G_OBJECT(wordDic->text_results_view), "motion-notify-event", G_CALLBACK(result_view_motion), NULL);
  
}

