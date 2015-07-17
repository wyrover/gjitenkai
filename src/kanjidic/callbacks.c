#include "kanjidic.h"
#include "kanjiutils.h"
#include "kanjifile.h"
#include "radical_list.h"
#include "kanji_item.h"

//////////////////////
//Menu items callbacks
//Edit
///Preferences
G_MODULE_EXPORT void on_menuitem_edit_prefs_activate(GtkMenuItem *menuitem, kanjidic *kanjidic){
  GtkDialog *prefs = (GtkDialog*)gtk_builder_get_object(kanjidic->definitions, 
                                                        "dialog_preferences");
  //set size and display the preference window
  gtk_window_set_default_size(GTK_WINDOW(prefs), 360, 320);
  gtk_widget_show_all ((GtkWidget*)prefs);
}

///////////////////
//Filters callbacks
G_MODULE_EXPORT void on_checkbutton_filter_strokes_toggled(GtkCheckButton *check_button, 
                                           kanjidic *kanjidic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);

  //set widgets sensitivity
  set_ui_stroke_filter_sensitivity(toggled, kanjidic);
  
  //set the boolean variable
  kanjidic->filter_by_stroke = toggled;
  
}

G_MODULE_EXPORT void on_checkbutton_filter_radicals_toggled(GtkCheckButton *check_button, 
                                            kanjidic *kanjidic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);

  //set the boolean variable
  kanjidic->filter_by_radical = toggled;

  //set widgets sensitivity
  set_ui_radical_filter_sensitivity(toggled, kanjidic);
}

G_MODULE_EXPORT void on_checkbutton_filter_key_toggled(GtkCheckButton *check_button, 
                                            kanjidic *kanjidic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);

  //set the boolean variable
  kanjidic->filter_by_key = toggled;

  //set widgets sensitivity
  set_ui_key_filter_sensitivity(toggled, kanjidic);
}


/**
   When a character is inserted in the filter by radical entry 
   -Check if this is a kanji, if not, remove it
   -if this is a non-radical kanji, replace it by it's radicals
 */
G_MODULE_EXPORT void on_entry_filter_radical_insert_text(GtkEntry    *entry,
                                         const gchar *text,
                                         gint         length,
                                         gint        *position,
                                         kanjidic    *kanjidic){

  //if unicode special character, do nothing
  char carriage_return[] = "\x0D";
  char backspace[] = "\x08";
  char delete[] = "\x7f";

  if((!strcmp(text, carriage_return)) || 
     (!strcmp(text, backspace)) || 
     (!strcmp(text, delete))){
       return;
     }

  //get the entered character in utf8 format
  gunichar unichar = g_utf8_get_char(text);
  gchar utf8char[3];
  int at = g_unichar_to_utf8(unichar, utf8char);
  utf8char[at] = '\0';

  //get the editable object from the entry to edit the content
  GtkEditable *editable = GTK_EDITABLE(entry);

  //if the kanji is a radical (kanji is found in the rad_info_hash), quit
  GList *all_radical_list = g_hash_table_get_keys(kanjidic->rad_info_hash);
  for (all_radical_list; all_radical_list != NULL; 
       all_radical_list = g_list_next(all_radical_list)) {
    if(!strcmp(all_radical_list->data, text)){
      return;
    }
  }

  
  //if this is not a kanji, do not insert
  if(!isKanjiChar(unichar)){
    g_signal_stop_emission_by_name (G_OBJECT (editable), "insert_text");
    return;
  }
  
  //if the entered character is alderly in the list entry, do not insert it
  const gchar *radicals = gtk_entry_get_text(entry);
  const gchar* radstrg_ptr = radicals;
  gint radnum;                   //number of character in radstrg

  radnum = g_utf8_strlen(radicals, -1); 
  if (radnum != 0){

    //to navigate in the entry
    int i;
    radstrg_ptr = radicals;

    //for every characters in the entry
    gunichar uniradical;
    while(uniradical = g_utf8_get_char(radstrg_ptr)){

      gchar radical[3];
      int at = g_unichar_to_utf8(uniradical, radical);
      radical[at] = '\0';

      if(!strcmp(radical, text)){
        g_signal_stop_emission_by_name (G_OBJECT (editable), "insert_text");
      }

      radstrg_ptr = g_utf8_next_char(radstrg_ptr);
    }
  }

  //get the radicals of the kanji and insert them into the entry
  GList* kanji_radical_list = get_radical_of_kanji(unichar, 
                                             kanjidic->kanji_info_hash);

  //if the kanji_radical_list size if of only one character, 
  //and the kanji inserted is the radical found, return. 
  //(prevent infinit recursivity)
  if(g_list_length(kanji_radical_list) == 1 && 
     !strcmp(utf8char, kanji_radical_list->data))return;

  //insert radicals into the entry
  for (kanji_radical_list;
       kanji_radical_list != NULL;
       kanji_radical_list = g_list_next(kanji_radical_list)) {

    gtk_editable_insert_text(editable, 
                             kanji_radical_list->data, 
                             strlen(kanji_radical_list->data), 
                             position);
  }

  //do not insert the kanji 
  g_signal_stop_emission_by_name (G_OBJECT (editable), "insert_text");

}

G_MODULE_EXPORT void on_button_clear_radical_clicked(GtkButton* button, kanjidic *kanjidic){
  //get the radical entry
  GtkEntry *entry_filter_radical = (GtkEntry*)gtk_builder_get_object(kanjidic->definitions, 
                                                                     "entry_filter_radical");

  gtk_entry_set_text(entry_filter_radical, "");

  radical_list_update_sensitivity(kanjidic);

}

G_MODULE_EXPORT void on_entry_filter_radical_activate(GtkWidget *entry, kanjidic *kanjidic){
  //search_and_display_kanji(kanjidic);
}

//click on a 'kanji button': display the kanji information of this kanji
G_MODULE_EXPORT void on_button_kanji_clicked(GtkButton *button, kanjidic *kanjidic) {    
  //the label of the button is the kanji to be searched/displayed
  const gchar* kanji = gtk_button_get_label(button);

  display_kanji(kanjidic, kanji);
}

G_MODULE_EXPORT void on_button_search_clicked(GtkWidget *widget, kanjidic *kanjidic) {
  GSList * kanji_list = search_kanji(kanjidic);

  //mouse cursor
  GdkCursor * cursor;

  //anchor in the result textview where to append the 'kanji buttons'
  GtkTextChildAnchor *kanji_results_anchor;
  GtkTextIter kanji_results_iter;
  
  //DISPLAY
  //with the list of kanji found from the radicals and/or strokes, append a
  //button for each kanji, with the kanji as label

  //get the widget where to append the 'kanji button'
  GtkTextView *textview_kanji_result = (GtkTextView*)
    gtk_builder_get_object(kanjidic->definitions, "textview_kanji_result");
  GtkTextBuffer *textbuffer_kanji_result = (GtkTextBuffer*)
    gtk_builder_get_object(kanjidic->definitions, "textbuffer_kanji_result");
  
  //clear the results and set the iterator at the begining
  gtk_text_buffer_set_text(textbuffer_kanji_result, "", 0);
  gtk_text_buffer_get_start_iter(textbuffer_kanji_result, &kanji_results_iter);
  
  //set the mouse pointer
  cursor = gdk_cursor_new(GDK_LEFT_PTR);
  GdkWindow *gdk_window = gtk_text_view_get_window (textview_kanji_result,
                                                    GTK_TEXT_WINDOW_TEXT);
  gdk_window_set_cursor(gdk_window, cursor);
    
  //for each kanji in the list
  for (kanji_list;
       kanji_list != NULL;
       kanji_list = g_list_next(kanji_list)) {

    //create a 'candidate kanji' button
    PangoFontDescription *fd = NULL;
    fd = pango_font_description_from_string (kanjidic->conf->kanji_result_font);
    
    GtkWidget *button_kanji = gtk_button_new_with_label(kanji_list->data);
    gtk_widget_override_font (button_kanji, fd);
    
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

G_MODULE_EXPORT void on_button_show_radical_list_clicked(GtkButton *button, kanjidic *kanjidic){
  radical_list_update_sensitivity(kanjidic);

  GtkWindow *radicals = (GtkWindow*)gtk_builder_get_object(kanjidic->definitions, 
                                                         "radical_list");

  //set size and display the preference window
  gtk_window_set_default_size(GTK_WINDOW(radicals), 320, 220);
  gtk_widget_show_all ((GtkWidget*)radicals);

  GdkWindow *gdk_radicals = gtk_widget_get_window(GTK_WIDGET(radicals));
  gdk_window_raise(gdk_radicals);
}

//Radical list callbacks
//prevent delete and hide
G_MODULE_EXPORT gboolean on_radical_list_delete_event(GtkWindow *window, kanjidic *kanjidic){
  gtk_widget_hide(GTK_WIDGET(window));
  return TRUE;
}

//a radical button in the radical button list has been clicked
G_MODULE_EXPORT void on_radical_button_clicked(GtkButton *button, kanjidic *kanjidic){

  //get the clicked kanji
  const gchar* radical = gtk_button_get_label(button);

  //add it to the entry filter
  GtkEntry *entry_filter_radical = (GtkEntry*)gtk_builder_get_object(kanjidic->definitions, 
                                                                     "entry_filter_radical");
  GtkEditable *editable = GTK_EDITABLE(entry_filter_radical);

  gint position = 1;

  gtk_editable_insert_text(editable, 
                           radical, 
                           strlen(radical),
                           &position);

  //update the radical list window button
  radical_list_update_sensitivity(kanjidic);

}

//about
G_MODULE_EXPORT void on_kanjidic_menuitem_help_about_activate(GtkMenuItem *menuitem, 
                                                              kanjidic *kanjidic){
  GtkWindow *window_about = (GtkWindow*)gtk_builder_get_object(kanjidic->definitions,
                                                               "aboutdialog");
  gtk_dialog_run(GTK_DIALOG(window_about));
  gtk_widget_hide (GTK_WIDGET(window_about)); 
}
