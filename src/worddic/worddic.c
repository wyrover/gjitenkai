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

  //set the loading dictionary thread to NULL
  worddic->thread_load_dic = NULL;
  
  //init the configuration handler
  worddic->settings = conf_init_handler(SETTINGS_WORDDIC);

  //load configuration 
  worddic->conf = worddic_conf_load(worddic);
  
  //by default search everything
  worddic->match_criteria_jp = ANY_MATCH;
  worddic->match_criteria_lat = ANY_MATCH;

  //set the number of entries to display par page result
  //TODO GSettings
  worddic->entries_per_page = 512;
  
  init_search_menu(worddic);

  //highlight style of the result text buffer
  GtkTextBuffer*textbuffer_search_results = (GtkTextBuffer*)
    gtk_builder_get_object(worddic->definitions, 
                           "textbuffer_search_results");
  
  GtkTextTag *highlight = gtk_text_buffer_create_tag (textbuffer_search_results,
                                                      "results_highlight",
                                                      "background-rgba",
                                                      worddic->conf->results_highlight_color,
                                                      NULL);
  worddic->conf->highlight = highlight;

  //japanese definition
  GtkTextTag *jap_def_tag = gtk_text_buffer_create_tag (textbuffer_search_results,
                                                        "japanese_definition",
                                                        "foreground-rgba",
                                                        worddic->conf->jap_def.color,
                                                        "font", 
                                                        worddic->conf->jap_def.font,
                                                        NULL);
  worddic->conf->jap_def.tag = jap_def_tag;
  
  //japanese reading  
  GtkTextTag *jap_reading_tag = gtk_text_buffer_create_tag (textbuffer_search_results,
                                                            "japanese_reading",
                                                            "foreground-rgba",
                                                            worddic->conf->jap_reading.color,
                                                            "font", 
                                                            worddic->conf->jap_reading.font,
                                                            NULL);
  worddic->conf->jap_reading.tag = jap_reading_tag;

  //gloss
  GtkTextTag *gloss_tag = gtk_text_buffer_create_tag (textbuffer_search_results,
                                                      "gloss",
                                                      "foreground-rgba",
                                                      worddic->conf->gloss.color,
                                                      "font", 
                                                      worddic->conf->gloss.font,
                                                      NULL);
  worddic->conf->gloss.tag = gloss_tag;

  //subgloss
  GtkTextTag *subgloss_tag = gtk_text_buffer_create_tag (textbuffer_search_results,
                                                      "subgloss",
                                                      "foreground-rgba",
                                                      worddic->conf->subgloss.color,
                                                      "font", 
                                                      worddic->conf->subgloss.font,
                                                      NULL);
  worddic->conf->subgloss.tag = subgloss_tag;

  //notes
  GtkTextTag *notes_tag = gtk_text_buffer_create_tag (textbuffer_search_results,
                                                      "notes",
                                                      "foreground-rgba",
                                                      worddic->conf->notes.color,
                                                      "font", 
                                                      worddic->conf->notes.font,
                                                      NULL);
  worddic->conf->notes.tag = notes_tag;
  
  
  //default font for the search results
  const gchar *font_name= worddic->conf->resultsfont;
  PangoFontDescription *font_desc = pango_font_description_from_string(font_name);

  //get the textview
  GtkTextView *textview_search_results = 
    (GtkTextView*)gtk_builder_get_object(worddic->definitions,
                                         "search_results");

  //apply the newly selected font to the results textview
  gtk_widget_override_font(GTK_WIDGET(textview_search_results), font_desc);
  
  //init the verb de-inflection mechanism
  init_inflection();

  //Init the preference window's widgets
  init_prefs_window(worddic);

  //init cursors
  cursor_selection = gdk_cursor_new(GDK_ARROW);
  cursor_default = gdk_cursor_new(GDK_XTERM);

  if(!worddic->conf->dicfile_list){
    GtkWidget *dialog;
    gchar *message = "<b>Welcome to the Gnome Japanese dictionary 'Gjiten Kai グジテン改'</b>\n It appears that you do not have any <i>EDICT</i> dictionary file setup. \n\n You can download one at the <a href=\"http://ftp.monash.edu.au/pub/nihongo/00INDEX.html#dic_fil\">Official EDICT website</a>\n\nThen add it in Gjiten Kai using the <b>Edit/Preferences Menu</b> then \n<b>Worddic - Dictionaries - Add</b>";
    
    dialog = gtk_message_dialog_new_with_markup(NULL, 0, GTK_MESSAGE_INFO,
                                                GTK_BUTTONS_OK, message);
    g_signal_connect_swapped(G_OBJECT(dialog), "response",
                             G_CALLBACK(gtk_widget_destroy),
                             G_OBJECT(dialog));
		
    gtk_widget_show_all(dialog);
  }

  //add a callback when scrolling to the edge of the result only if GTK >= 3.16
#if GTK_MAJOR_VERSION >= 3 && GTK_MINOR_VERSION >= 16
  GtkScrolledWindow *scrolledwindow_search_result = (GtkScrolledWindow*)
    gtk_builder_get_object(worddic->definitions, "scrolledwindow_search_result");
  g_signal_connect(scrolledwindow_search_result,
		   "edge-reached",
		   G_CALLBACK(on_worddic_search_results_edge_reached),
		   worddic);
#endif 
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
    radio_lat = (GtkRadioMenuItem*)gtk_builder_get_object(worddic->definitions,
                                                          "menuitem_search_whole_expression");
    break;
  case WORD_MATCH:
    radio_lat = (GtkRadioMenuItem*)gtk_builder_get_object(worddic->definitions,
                                                          "menuitem_search_latin_word");
    break;
  case ANY_MATCH:
    radio_lat = (GtkRadioMenuItem*)gtk_builder_get_object(worddic->definitions,
                                       "menuitem_search_latin_any");
    break;
  }

  switch(match_criteria_jp){
  case EXACT_MATCH:
    radio_jp = (GtkRadioMenuItem*)gtk_builder_get_object(worddic->definitions,
                                                         "menuitem_search_japanese_exact");
    break;
  case START_WITH_MATCH:
    radio_jp = (GtkRadioMenuItem*)gtk_builder_get_object(worddic->definitions,
                                                         "menuitem_search_japanese_start");
    break;
  case END_WITH_MATCH:
    radio_jp = (GtkRadioMenuItem*)gtk_builder_get_object(worddic->definitions,
                                                         "menuitem_search_japanese_end");
    break;
  case ANY_MATCH:
    radio_jp = (GtkRadioMenuItem*)gtk_builder_get_object(worddic->definitions,
                                                         "menuitem_search_japanese_any");
    break;
  }
 
  gtk_check_menu_item_set_active(radio_jp, TRUE);
  gtk_check_menu_item_set_active(radio_lat, TRUE); 
}

void print_unit(GtkTextBuffer *textbuffer,
                gchar *text,
                unit_style *style){
  GtkTextIter iter;
  gtk_text_buffer_insert_at_cursor(textbuffer, 
                                   style->start,
                                   strlen(style->start));

  gtk_text_buffer_get_end_iter (textbuffer, &iter);
  gtk_text_buffer_insert_with_tags(textbuffer,
                                   &iter,
                                   text,
                                   strlen(text),
                                   style->tag,
                                   NULL);

  gtk_text_buffer_insert_at_cursor(textbuffer, 
                                   style->end,
                                   strlen(style->end));
}

void print_entries(GtkTextBuffer *textbuffer, worddic *worddic){
  gint entry_number=0;
  
  while(worddic->results &&
	entry_number <= worddic->entries_per_page){
    
    dicresult *p_dicresult = worddic->results->data;

    GjitenDicentry *entry = p_dicresult->entry;
    gchar *match = p_dicresult->match;
    gchar *comment = p_dicresult->comment;

    //browse list
    GSList *unit = NULL;

    //text to print
    gchar* text = NULL;

    GtkTextIter iter_from;
    gtk_text_buffer_get_end_iter(textbuffer, &iter_from);
    
    //create a mark, indicating the start where the new entry is writed
    GtkTextMark *start_mark = 
      gtk_text_buffer_create_mark (textbuffer, NULL, &iter_from, TRUE);
 
    //Japanese definition
    for(unit = entry->jap_definition; unit != NULL; unit = unit->next){
      text = (gchar*)unit->data;
      print_unit(textbuffer, text, &worddic->conf->jap_def);
    }
    
    //reading
    if(entry->jap_reading){
      for(unit = entry->jap_reading;unit != NULL;unit = unit->next){
        text = (gchar*)unit->data;
        print_unit(textbuffer, text, &worddic->conf->jap_reading);
      }
    }
    
    //Entry General Informations
    for(unit = entry->general_informations;
        unit != NULL;
        unit = unit->next){
      text = (gchar*)unit->data;
      print_unit(textbuffer, text, &worddic->conf->notes);
    }

    //comment
    if(comment)print_unit(textbuffer, comment, &worddic->conf->notes);
    
    //gloss
    for(unit = entry->gloss;unit != NULL;unit = unit->next){

      gtk_text_buffer_insert_at_cursor(textbuffer, worddic->conf->gloss.start,
                                       strlen(worddic->conf->gloss.start));

      gloss *p_gloss = unit->data;
            
      ////General Informations
      GSList *GI = NULL;
      for(GI = p_gloss->general_informations;
          GI != NULL;
          GI = GI->next){
        text = (gchar*)GI->data;
        print_unit(textbuffer, text, &worddic->conf->notes);
      }
      
      GSList *sub_gloss = NULL;
      ////sub gloss
      for(sub_gloss = p_gloss->sub_gloss;
          sub_gloss != NULL;
          sub_gloss = sub_gloss->next){
        text = (gchar*)sub_gloss->data;
        print_unit(textbuffer, text, &worddic->conf->subgloss);
      }

      gtk_text_buffer_insert_at_cursor(textbuffer, worddic->conf->gloss.end,
                                       strlen(worddic->conf->gloss.end));
    }

    gtk_text_buffer_insert_at_cursor(textbuffer, "\n", strlen("\n"));

    //set the iter from where to search text to highlight from the start mark
    gtk_text_buffer_get_iter_at_mark(textbuffer, &iter_from, start_mark);
    
    //search and highlight the matched expression from the iter_from
    highlight_result(textbuffer,
                     worddic->conf->highlight,
                     match,
                     &iter_from);

    worddic->results = worddic->results->next;
    entry_number++;
  }
}

void highlight_result(GtkTextBuffer *textbuffer,
		      GtkTextTag *highlight,
		      const gchar *text_to_highlight,
                      GtkTextIter *iter_from){
  gboolean has_iter;
  GtkTextIter match_start, match_end;
  
  do{
    //search where the result string is located in the result buffer
    has_iter = gtk_text_iter_forward_search (iter_from,
                                             text_to_highlight,
                                             GTK_TEXT_SEARCH_VISIBLE_ONLY,
                                             &match_start,
                                             &match_end,
                                             NULL);
    
    if(has_iter){
      //highlight at this location
      gtk_text_buffer_apply_tag (textbuffer,
                                 highlight,
                                 &match_start, 
                                 &match_end);

      //next iteration starts from the end of this iteration
      *iter_from = match_end;
      }

    }while(has_iter);
}
