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
  conf_init_handler();

  //load configuration 
  kanjidic->conf = conf_load();

  //init the radical and kanji hash
  kanjidic->kanji_info_hash = g_hash_table_new((GHashFunc)g_str_hash,
                                               (GEqualFunc)g_str_equal);
  kanjidic->rad_info_hash = g_hash_table_new((GHashFunc)g_str_hash,
                                             (GEqualFunc)g_str_equal);
  kanjidic->rad_info_list = NULL;

  //load radical and kanji from the radkfile
  kanjidic->rad_info_list = load_radkfile(&kanjidic->rad_info_hash, 
                &kanjidic->kanji_info_hash,
                kanjidic->rad_info_list);  

  //init the kanji display style
  GtkTextBuffer *textbuffer_kanji_display = gtk_builder_get_object(kanjidic->definitions, 
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
  GtkButton *checkbutton_filter_radicals = gtk_builder_get_object(kanjidic->definitions, 
                                                                  "checkbutton_filter_radicals");
  gtk_toggle_button_set_active(checkbutton_filter_radicals, kanjidic->filter_by_radical);
  GtkButton *checkbutton_filter_strokes = gtk_builder_get_object(kanjidic->definitions, 
                                                                  "checkbutton_filter_strokes");
  gtk_toggle_button_set_active(checkbutton_filter_strokes, kanjidic->filter_by_stroke);
  GtkButton *checkbutton_filter_key = gtk_builder_get_object(kanjidic->definitions, 
                                                                  "checkbutton_filter_key");
  gtk_toggle_button_set_active(checkbutton_filter_key, kanjidic->filter_by_key);

  //init the radical window with the radical buttons
  radical_list_init(kanjidic);
}

void set_ui_radical_filter_sensitivity(gboolean sensitivity, kanjidic *kanjidic){

  GtkEntry *entry_filter_radical = gtk_builder_get_object(kanjidic->definitions, 
                                                          "entry_filter_radical");
  GtkButton *button_clear = gtk_builder_get_object(kanjidic->definitions, 
                                                   "button_clear_radical");
  GtkButton *button_show_radical_list = gtk_builder_get_object(kanjidic->definitions, 
                                                   "button_show_radical_list");

  gtk_widget_set_sensitive(entry_filter_radical, sensitivity);
  gtk_widget_set_sensitive(button_clear, sensitivity);
  gtk_widget_set_sensitive(button_show_radical_list, sensitivity);
}

void set_ui_stroke_filter_sensitivity(gboolean sensitivity, kanjidic *kanjidic){
  GtkSpinButton *spinbutton_filter_stroke = gtk_builder_get_object(kanjidic->definitions, 
                                                                   "spinbutton_filter_stroke");
  GtkSpinButton *spinbutton_filter_stroke_diff = gtk_builder_get_object(kanjidic->definitions, 
                                                                        "spinbutton_filter_stroke_diff");
  gtk_widget_set_sensitive(spinbutton_filter_stroke, sensitivity);
  gtk_widget_set_sensitive(spinbutton_filter_stroke_diff, sensitivity);
}

void set_ui_key_filter_sensitivity(gboolean sensitivity, kanjidic *kanjidic){
  GtkEntry *entry_filter_key = gtk_builder_get_object(kanjidic->definitions, 
                                                          "entry_filter_key");
  gtk_widget_set_sensitive(entry_filter_key, sensitivity);
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
    GtkSpinButton *spinbutton_filter_stroke = gtk_builder_get_object(kanjidic->definitions, 
                                                               "spinbutton_filter_stroke");
    GtkSpinButton *spinbutton_filter_stroke_diff = gtk_builder_get_object(kanjidic->definitions, 
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
    GtkEntry *entry_filter_radical = gtk_builder_get_object(kanjidic->definitions, 
                                                            "entry_filter_radical");
    gchar *radicals = gtk_entry_get_text(entry_filter_radical);
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
    GtkEntry *entry_filter_key = gtk_builder_get_object(kanjidic->definitions, 
                                                            "entry_filter_key");
    gchar *key = gtk_entry_get_text(entry_filter_key);

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
  GtkTextView *textview_kanji_result = gtk_builder_get_object(kanjidic->definitions, 
                                                              "textview_kanji_result");
  GtkTextBuffer *textbuffer_kanji_result = gtk_builder_get_object(kanjidic->definitions, 
                                                                  "textbuffer_kanji_result");
  
  //clear the results and get the set the iterator at the begining
  gtk_text_buffer_set_text(textbuffer_kanji_result, "", 0);
  gtk_text_buffer_get_start_iter(textbuffer_kanji_result, &kanji_results_iter);

  if(kanji_list == NULL){
    return;
  }
  
  //set the pointer over the results kanji a pointer
  cursor = gdk_cursor_new(GDK_LEFT_PTR);
  GdkWindow *gdk_window = gtk_text_view_get_window (textview_kanji_result,
                                                    GTK_TEXT_WINDOW_TEXT);
  gdk_window_set_cursor(gdk_window, cursor);

  //for each kanji in the list
  for (kanji_list;
       kanji_list != NULL;
       kanji_list = g_list_next(kanji_list)) {

    //create the kanji button
    GtkButton *button_kanji = gtk_button_new_with_label(kanji_list->data);
    g_signal_connect(button_kanji, 
                     "clicked", 
                     on_button_kanji_clicked, 
                     kanjidic);

    //add the button in the textview at the anchor position
    kanji_results_anchor = gtk_text_buffer_create_child_anchor(textbuffer_kanji_result, 
                                                               &kanji_results_iter);

    gtk_text_view_add_child_at_anchor(textview_kanji_result, 
                                      button_kanji, 
                                      kanji_results_anchor);
  }
  
  //show what has been added
  gtk_widget_show_all(textview_kanji_result);
}
