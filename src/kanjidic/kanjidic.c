#include "kanjidic.h"

void kanjidic_init (kanjidic *kanjidic)
{
  GError *err = NULL;
  kanjidic->definitions = gtk_builder_new ();
  gtk_builder_add_from_file (kanjidic->definitions,
                             UI_DEFINITIONS_FILE_KANJIDIC, &err);
  if (err != NULL) {
    g_printerr
      ("Error while loading kanjidic definitions file: %s\n",
       err->message);
    g_error_free (err);
    gtk_main_quit ();
  }
  gtk_builder_connect_signals (kanjidic->definitions, kanjidic);

  //init the configuration handler
  kanjidic->settings = conf_init_handler(SETTINGS_KANJIDIC);

  //load configuration 
  kanjidic->conf = kanjidic_conf_load(kanjidic);

  //load the kanji dictionary
  dicfile_load(kanjidic->conf->kanjidic, NULL);  
  
  //init the radical and kanji hash
  kanjidic->kanji_info_hash = g_hash_table_new((GHashFunc)g_str_hash,
                                               (GEqualFunc)g_str_equal);
  kanjidic->rad_info_hash = g_hash_table_new((GHashFunc)g_str_hash,
                                             (GEqualFunc)g_str_equal);

  //load radical and kanji from the radkfile
  kanjidic->rad_info_list = NULL;
  kanjidic->rad_info_list = load_radkfile(&kanjidic->rad_info_hash, 
					  &kanjidic->kanji_info_hash,
					  kanjidic->rad_info_list);

  //init the kanji display style
  GtkTextBuffer *textbuffer_kanji_display = (GtkTextBuffer*)gtk_builder_get_object(kanjidic->definitions, 
                                                                                   "textbuffer_kanji_display");
  kanjidic->texttag_kanji = gtk_text_buffer_create_tag (textbuffer_kanji_display,
                                                        "kanji_tag",
                                                        "foreground-rgba",
                                                        kanjidic->conf->kanji_color,
                                                        "font", 
                                                        kanjidic->conf->kanji_font,
                                                        NULL);

  //filters default value
  kanjidic->filter_by_stroke = FALSE;
  kanjidic->filter_by_radical = TRUE;
  kanjidic->filter_by_key = FALSE;

  //init the UI
  //filters sensitivity
  set_ui_radical_filter_sensitivity(kanjidic->filter_by_radical, kanjidic);
  set_ui_stroke_filter_sensitivity(kanjidic->filter_by_stroke, kanjidic);
  set_ui_key_filter_sensitivity(kanjidic->filter_by_key, kanjidic);

  //check/uncheck checkbutton with default values
  GtkButton *checkbutton_filter_radicals = (GtkButton*)gtk_builder_get_object(kanjidic->definitions, 
                                                                              "checkbutton_filter_radicals");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_filter_radicals),
                               kanjidic->filter_by_radical);
  
  GtkButton *checkbutton_filter_strokes = (GtkButton*)gtk_builder_get_object(kanjidic->definitions, 
                                                                             "checkbutton_filter_strokes");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_filter_strokes), 
                               kanjidic->filter_by_stroke);

  GtkButton *checkbutton_filter_key = (GtkButton *)gtk_builder_get_object(kanjidic->definitions, 
                                                                          "checkbutton_filter_key");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_filter_key), 
                               kanjidic->filter_by_key);

  //init the radical window with the radical buttons
  radical_list_init(kanjidic);

  //init the preference window's widgets
  init_prefs_kanjidic(kanjidic);
}

void set_ui_radical_filter_sensitivity(gboolean sensitivity, kanjidic *kanjidic){

  GtkEntry *entry_filter_radical = (GtkEntry *)gtk_builder_get_object(kanjidic->definitions, 
                                                                      "entry_filter_radical");
  GtkButton *button_clear = (GtkButton *)gtk_builder_get_object(kanjidic->definitions, 
                                                                "button_clear_radical");
  GtkButton *button_show_radical_list = (GtkButton *)gtk_builder_get_object(kanjidic->definitions, 
                                                                            "button_show_radical_list");

  gtk_widget_set_sensitive(GTK_WIDGET(entry_filter_radical), sensitivity);
  gtk_widget_set_sensitive(GTK_WIDGET(button_clear), sensitivity);
  gtk_widget_set_sensitive(GTK_WIDGET(button_show_radical_list), sensitivity);
}

void set_ui_stroke_filter_sensitivity(gboolean sensitivity, kanjidic *kanjidic){
  GtkSpinButton *spinbutton_filter_stroke = (GtkSpinButton *)gtk_builder_get_object(kanjidic->definitions, 
                                                                                    "spinbutton_filter_stroke");
  GtkSpinButton *spinbutton_filter_stroke_diff = (GtkSpinButton *)gtk_builder_get_object(kanjidic->definitions, 
                                                                                         "spinbutton_filter_stroke_diff");

  gtk_widget_set_sensitive(GTK_WIDGET(spinbutton_filter_stroke), sensitivity);
  gtk_widget_set_sensitive(GTK_WIDGET(spinbutton_filter_stroke_diff), sensitivity);
}

void set_ui_key_filter_sensitivity(gboolean sensitivity, kanjidic *kanjidic){
  GtkEntry *entry_filter_key = (GtkEntry*)gtk_builder_get_object(kanjidic->definitions, 
                                                                 "entry_filter_key");
  gtk_widget_set_sensitive(GTK_WIDGET(entry_filter_key), sensitivity);
}

void search_and_display_kanji(kanjidic *kanjidic){
  GList *kanji_list=NULL;

  //mouse cursor
  GdkCursor * cursor;

  //anchor in the result textview where to append the 'kanji buttons'
  GtkTextChildAnchor *kanji_results_anchor;
  GtkTextIter kanji_results_iter;
  
  //filter by strokes
  if(kanjidic->filter_by_stroke){
    GtkSpinButton *spinbutton_filter_stroke = (GtkSpinButton*)gtk_builder_get_object(kanjidic->definitions, 
                                                                                     "spinbutton_filter_stroke");
    GtkSpinButton *spinbutton_filter_stroke_diff = (GtkSpinButton*)gtk_builder_get_object(kanjidic->definitions, 
                                                                                          "spinbutton_filter_stroke_diff");

    gint stroke_filter = gtk_spin_button_get_value_as_int(spinbutton_filter_stroke);
    gint stroke_filter_diff = gtk_spin_button_get_value_as_int(spinbutton_filter_stroke_diff);
    
    //get all kanji with the entered radicals
    GList *kanji_by_stroke_list=NULL;
    kanji_by_stroke_list = get_kanji_by_stroke(stroke_filter, stroke_filter_diff, 
                                               kanji_by_stroke_list,
                                               kanjidic->conf->kanjidic);

    kanji_list = list_merge_str(kanji_list, kanji_by_stroke_list);
  }
  
  //filter by radical
  if(kanjidic->filter_by_radical){
    GtkEntry *entry_filter_radical = (GtkEntry*)gtk_builder_get_object(kanjidic->definitions, 
                                                                       "entry_filter_radical");
    const gchar *radicals = gtk_entry_get_text(entry_filter_radical);
    //if the entry is empty, ignore the filter
    if(strcmp(radicals, "")){
      //get all kanji with the entered radicals
      GList *kanji_by_radical_list=NULL;
      kanji_by_radical_list = get_kanji_by_radical(radicals, kanjidic->rad_info_hash);
      kanji_list = list_merge_str(kanji_list, kanji_by_radical_list);
    }
  }

  //filter by key
  if(kanjidic->filter_by_key){
    GtkEntry *entry_filter_key = (GtkEntry*)gtk_builder_get_object(kanjidic->definitions, 
                                                                   "entry_filter_key");
    const gchar *key = gtk_entry_get_text(entry_filter_key);

    //if the entry is empty, ignore the filter
    if(strcmp(key, "")){
      GList *kanji_by_key_list=NULL;
      kanji_by_key_list = get_kanji_by_key(key, 
                                           kanji_by_key_list, 
                                           kanjidic->conf->kanjidic);
      kanji_list = list_merge_str(kanji_list, kanji_by_key_list);
    }
  }

  //with the list of kanji found from the radicals and/or strokes, append a gtk 
  //button for each kanji, with the kanji as label

  //get the widget where to append the 'kanji button'
  GtkTextView *textview_kanji_result = (GtkTextView*)gtk_builder_get_object(kanjidic->definitions, 
                                                                            "textview_kanji_result");
  GtkTextBuffer *textbuffer_kanji_result = (GtkTextBuffer*)gtk_builder_get_object(kanjidic->definitions, 
                                                                                  "textbuffer_kanji_result");
  
  //clear the results and get the set the iterator at the begining
  gtk_text_buffer_set_text(textbuffer_kanji_result, "", 0);
  gtk_text_buffer_get_start_iter(textbuffer_kanji_result, &kanji_results_iter);

  if(kanji_list == NULL){
    return;
  }
  
  //set the mouse pointer
  cursor = gdk_cursor_new(GDK_LEFT_PTR);
  GdkWindow *gdk_window = gtk_text_view_get_window (textview_kanji_result,
                                                    GTK_TEXT_WINDOW_TEXT);
  gdk_window_set_cursor(gdk_window, cursor);

  //for each kanji in the list
  for (kanji_list;
       kanji_list != NULL;
       kanji_list = g_list_next(kanji_list)) {

    //create the kanji button
    PangoFontDescription *fd = NULL;
    fd = pango_font_description_from_string (kanjidic->conf->kanji_result_font);
    
    GtkButton *button_kanji = (GtkButton*)gtk_button_new_with_label(kanji_list->data);
    gtk_widget_modify_font (button_kanji, fd);
    
    g_signal_connect(button_kanji, 
                     "clicked", 
                     G_CALLBACK(on_button_kanji_clicked), 
                     kanjidic);

    //add the button in the textview at the anchor position
    kanji_results_anchor = gtk_text_buffer_create_child_anchor(textbuffer_kanji_result, 
                                                               &kanji_results_iter);

    gtk_text_view_add_child_at_anchor(textview_kanji_result, 
                                      GTK_WIDGET(button_kanji), 
                                      kanji_results_anchor);
  }
  
  //show what has been added
  gtk_widget_show_all(GTK_WIDGET(textview_kanji_result));
}

void display_kanji(kanjidic *kanjidic, const gchar* kanji)
{
  //add a button in the history box
  //TODO from g_slist of buttons
  GtkButton *button_history = (GtkButton*)gtk_button_new_with_label(kanji);
  g_signal_connect(button_history, 
                   "clicked", 
                   G_CALLBACK(on_button_kanji_clicked),
                   kanjidic);
  
  GtkBox *box_history = (GtkBox*)gtk_builder_get_object(kanjidic->definitions, 
                                                        "box_history");
  gtk_box_pack_start(box_history, GTK_WIDGET(button_history), FALSE, FALSE, 0);
  gtk_widget_show_all(GTK_WIDGET(box_history));
  
  //get the area where to display the kanji
  GtkTextBuffer *textbuffer_kanji_display = (GtkTextBuffer*)gtk_builder_get_object(kanjidic->definitions, 
                                                                                   "textbuffer_kanji_display");
  
  //get the  kanji informations from kdic
  gchar *kanji_info_line = get_line_from_dic(kanji, kanjidic->conf->kanjidic);
  kanjifile_entry *kanji_data= do_kdicline(kanji_info_line);

  //iterators to apply tag between
  GtkTextIter start, end;
  
  //display the kanji in the kanji display buffer
  gtk_text_buffer_set_text(textbuffer_kanji_display, "", 0);
  gtk_text_buffer_insert_at_cursor(textbuffer_kanji_display, kanji, strlen(kanji));

  //Apply a tag to change the style of the displayed kanji  
  //gint cursor_pos;
  //g_object_get(textbuffer_kanji_display ,"cursor-position", &cursor_pos, NULL);
  gtk_text_buffer_get_start_iter (textbuffer_kanji_display, &start);
  gtk_text_buffer_get_end_iter (textbuffer_kanji_display, &end);
  gtk_text_buffer_apply_tag_by_name (textbuffer_kanji_display,
                                     "kanji_tag",
                                     &start,
                                     &end);


  //Display informations on the kanji 
  gtk_text_buffer_insert_at_cursor(textbuffer_kanji_display,
                                   "\n",
                                   strlen("\n"));

  GSList *kanji_item_head;  //browse thought the kanji items 
  for (kanji_item_head = kanji_item_list;
       kanji_item_head != NULL;
       kanji_item_head = g_slist_next(kanji_item_head)){

    GSList *item = NULL;   //the kanji item content to list
    
    kanji_item *ki = kanji_item_head->data;
    if(ki->active){
      gtk_text_buffer_insert_at_cursor(textbuffer_kanji_display,
                                       ki->name,
                                       strlen(ki->name));
      gtk_text_buffer_insert_at_cursor(textbuffer_kanji_display,
                                       ": ",
                                       strlen(": "));
      
      if(!strcmp(ki->gsettings_name, "radical")){
        //list radicals without separation chars
        KanjiInfo *kanji_info = g_hash_table_lookup(kanjidic->kanji_info_hash, kanji);
        GList *kanji_info_list;
        for (kanji_info_list = kanji_info->rad_info_list;
             kanji_info_list != NULL;
             kanji_info_list = kanji_info_list->next) { 
          gtk_text_buffer_insert_at_cursor(textbuffer_kanji_display, 
                                           (const char*)((RadInfo *) kanji_info_list->data)->radical, 
                                           strlen((const char*)((RadInfo *) kanji_info_list->data)->radical)); 
        }
      }
      else if(!strcmp(ki->gsettings_name, "strokes")){
        //strokes count
        gchar *tmp_entry = g_strdup_printf("%d", kanji_data->stroke);
        gtk_text_buffer_insert_at_cursor(textbuffer_kanji_display, 
                                         tmp_entry, 
                                         strlen(tmp_entry));
        g_free(tmp_entry);
      }
      else if(!strcmp(ki->gsettings_name, "onyomi")){
        item = kanji_data->onyomi;
      }
      else if(!strcmp(ki->gsettings_name, "kunyomi")){
        item = kanji_data->kunyomi;
      }
      else if(!strcmp(ki->gsettings_name, "translation")){
        item = kanji_data->translations;
      }

      //item point to one of the kanji_entry's list to display or NULL if no match
      for (item;
           item != NULL;
           item = g_slist_next(item)){
    
        gtk_text_buffer_insert_at_cursor(textbuffer_kanji_display,
                                         item->data,
                                         strlen(item->data));

        //if there is another entry for this definition, append a separation char
        if(g_slist_next(item) != NULL){
          gtk_text_buffer_insert_at_cursor(textbuffer_kanji_display,
                                           kanjidic->conf->separator,
                                           strlen(kanjidic->conf->separator));
        }
      }
      gtk_text_buffer_insert_at_cursor(textbuffer_kanji_display,
                                       "\n",
                                       strlen("\n"));      
      
    }
  }
}
