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
  worddic->settings = conf_init_handler(SETTINGS_WORDDIC);

  //load configuration 
  worddic->conf = worddic_conf_load(worddic);

  //by default, no mmaped dictionary
  worddic->conf->mmaped_dicfile = NULL;
  
  //by default search everything
  worddic->match_criteria_jp = ANY_MATCH;
  worddic->match_criteria_lat = ANY_MATCH;
  
  init_search_menu(worddic);

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

  //default font for the search results
  const gchar *font_name= worddic->conf->resultsfont;
  PangoFontDescription *font_desc = pango_font_description_from_string(font_name);

  //get the textview
  GtkTextView *textview_search_results = 
    (GtkTextView*)gtk_builder_get_object(worddic->definitions, "search_results");

  //apply the newly selected font to the results textview
  gtk_widget_override_font(GTK_WIDGET(textview_search_results), font_desc);
  
  //init the verb de-inflection mechanism
  Verbinit();

  //Init the preference window's widgets
  init_prefs_window(worddic);

  //init cursors
  cursor_selection = gdk_cursor_new(GDK_ARROW);
  cursor_default = gdk_cursor_new(GDK_XTERM);
  
}

void init_search_menu(worddic *worddic)
{
  
  //get the search options
  gint match_criteria_jp = worddic->match_criteria_jp;
  gint match_criteria_lat = worddic->match_criteria_lat;
 
  GtkRadioMenuItem* radio_jp;
  GtkRadioMenuItem* radio_lat;
 
  switch(match_criteria_lat){
  case EXACT_MATCH:
    radio_lat = gtk_builder_get_object(worddic->definitions, "menuitem_search_whole_expression");
    break;
  case WORD_MATCH:
    radio_lat = gtk_builder_get_object(worddic->definitions, "menuitem_search_latin_word");
    break;
  case ANY_MATCH:
    radio_lat = gtk_builder_get_object(worddic->definitions, "menuitem_search_latin_any");
    break;
    /*  case REGEX:
    radio_lat = gtk_builder_get_object(worddic->definitions, "menuitem_search_latin_regex");
    break;*/
  }

  switch(match_criteria_jp){
  case EXACT_MATCH:
    radio_jp = gtk_builder_get_object(worddic->definitions, "menuitem_search_japanese_exact");
    break;
  case START_WITH_MATCH:
    radio_jp = gtk_builder_get_object(worddic->definitions, "menuitem_search_japanese_start");
    break;
  case END_WITH_MATCH:
    radio_jp = gtk_builder_get_object(worddic->definitions, "menuitem_search_japanese_end");
    break;
  case ANY_MATCH:
    radio_jp = gtk_builder_get_object(worddic->definitions, "menuitem_search_japanese_any");
    break;
    /*case REGEX:
    radio_jp = gtk_builder_get_object(worddic->definitions, "menuitem_search_japanese_regex");
    break;*/
  }
 
  gtk_check_menu_item_set_active(radio_jp, TRUE);
  gtk_check_menu_item_set_active(radio_lat, TRUE); 
}

void print_entry(GtkTextBuffer *textbuffer_search_results,
                 GtkTextTag *highlight,
                 GList *entries_highlight,
                 GList *entries){

  GList *l = NULL;                   //browse results    
  for (l = entries; l != NULL; l = l->next){
    GjitenDicentry *entry = l->data;
        
    gtk_text_buffer_insert_at_cursor(textbuffer_search_results, 
                                     "•", strlen("•"));

    //print the japanese definition
    ////get the font from conf
    gtk_text_buffer_insert_at_cursor(textbuffer_search_results, 
                                     entry->jap_definition,
                                     strlen(entry->jap_definition));

    if(entry->jap_reading){
      gtk_text_buffer_insert_at_cursor(textbuffer_search_results, 
                                       " [", strlen(" ["));
      gtk_text_buffer_insert_at_cursor(textbuffer_search_results, 
                                       entry->jap_reading,
                                       strlen(entry->jap_reading));
      gtk_text_buffer_insert_at_cursor(textbuffer_search_results, 
                                       "] ", strlen("] "));
    }

    GList *d = NULL;
    for(d = entry->definitions;
        d != NULL;
        d = d->next){

      gchar* definition = (gchar*)d->data;
          
      gtk_text_buffer_insert_at_cursor(textbuffer_search_results, 
                                       definition,
                                       strlen(definition));
          
      gtk_text_buffer_insert_at_cursor(textbuffer_search_results, 
                                       "/",
                                       strlen("/"));
    }
        
    gtk_text_buffer_insert_at_cursor(textbuffer_search_results, 
                                     "\n",
                                     strlen("\n"));
    //highligh
    highlight_result(textbuffer_search_results,
                     highlight,
                     entries_highlight->data);
    
    entries_highlight = entries_highlight->next;
  }
}

void highlight_result(GtkTextBuffer *textbuffer_search_results,
		      GtkTextTag *highlight,
		      const gchar *text_to_highlight){
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
