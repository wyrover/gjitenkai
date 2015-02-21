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
}

void highlight_result(GtkTextBuffer *textbuffer_search_results,
		      GtkTextTag *highlight,
		      gchar *text_to_highlight){
  gboolean has_iter;
  GtkTextIter iter, match_start, match_end;
  gtk_text_buffer_get_start_iter (textbuffer_search_results, &iter);
  
  do{
    //search where the result string is located in the result buffer
    has_iter = gtk_text_iter_forward_search (&iter,
                                             text_to_highlight,
                                             GTK_TEXT_SEARCH_VISIBLE_ONLY,
                                             &match_start,
                                             &match_end,
                                             NULL);
    if(has_iter){
      //highlight at this location
      gtk_text_buffer_apply_tag (textbuffer_search_results,
                                 highlight,
                                 &match_start, 
                                 &match_end);

      //next iteration starts at the end of this iteration
      iter = match_end;
    }

  }while(has_iter);
}

