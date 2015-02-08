#include "kanjidic.h"
#include "kanjiutils.h"
#include "kanjifile.h"
#include "radical_list.h"

//////////////////////
//Menu items callbacks
//Edit
///Preferences
void on_menuitem_edit_prefs_activate(GtkMenuItem *menuitem, kanjidic *kanjidic){
  GtkDialog *prefs = (GtkWindow*)gtk_builder_get_object(kanjidic->definitions, 
                                                               "dialog_preferences");
  //set size and display the preference window
  gtk_window_set_default_size(GTK_WINDOW(prefs), 320, 220);
  gtk_widget_show_all ((GtkWidget*)prefs);
}

///////////////////
//Filters callbacks
void on_checkbutton_filter_strokes_toggled(GtkCheckButton *check_button, 
                                           kanjidic *kanjidic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);

  //set widgets sensitivity
  set_ui_stroke_filter_sensitivity(toggled, kanjidic);
  
  //set the boolean variable
  kanjidic->filter_by_stroke = toggled;
  
}

void on_checkbutton_filter_radicals_toggled(GtkCheckButton *check_button, 
                                            kanjidic *kanjidic){
  gboolean toggled = gtk_toggle_button_get_active((GtkToggleButton*)check_button);

  //set the boolean variable
  kanjidic->filter_by_radical = toggled;

  //set widgets sensitivity
  set_ui_radical_filter_sensitivity(toggled, kanjidic);
}

void on_checkbutton_filter_key_toggled(GtkCheckButton *check_button, 
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
void on_entry_filter_radical_insert_text(GtkEntry    *entry,
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
  
  //if this is not a kanji (do not check yet if it's a radical or not)
  if(!isKanjiChar(unichar)){
    //do not allow this character in the entry widget
    g_signal_stop_emission_by_name (G_OBJECT (editable), "insert_text");
    return;
  }
  
  //if the entered character is alderly in the list entry, do not insert it
  gchar *radicals = gtk_entry_get_text(entry);
  gchar* radstrg_ptr = radicals;
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

  //if the kanji is a radical (kanji is found in the rad_info_hash), quit
  GList *all_radical_list = g_hash_table_get_keys(kanjidic->rad_info_hash);
  for (all_radical_list; all_radical_list != NULL; 
       all_radical_list = g_list_next(all_radical_list)) {
    if(!strcmp(all_radical_list->data, text)){
      return;
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

void on_button_clear_radical_clicked(GtkButton* button, kanjidic *kanjidic){
  //get the radical entry
  GtkEntry *entry_filter_radical = gtk_builder_get_object(kanjidic->definitions, 
                                                          "entry_filter_radical");

  gtk_entry_set_text(entry_filter_radical, "");

  radical_list_update_sensitivity(kanjidic);

}

void on_entry_filter_radical_activate(GtkWidget *entry, kanjidic *kanjidic){
  search_and_display_kanji(kanjidic);
}

//click on a 'kanji button': display the kanji information of this kanji
void on_button_kanji_clicked(GtkButton *button, kanjidic *kanjidic) {
  
  //the label of the button is the kanji to be searched/displayed
  gchar* kanji = gtk_button_get_label(button);

  //TODO from g_slist of buttons
  //add a button in the history box
  GtkButton *button_history = gtk_button_new_with_label(kanji);
  g_signal_connect(button_history, 
                   "clicked", 
                   on_button_kanji_clicked, 
                   kanjidic);

  GtkBox *box_history = gtk_builder_get_object(kanjidic->definitions, 
                                               "box_history");
  gtk_box_pack_start(box_history, button_history, FALSE, FALSE, 0);
  gtk_widget_show_all(box_history);

  //get the area where to display the kanji
  GtkTextBuffer *textbuffer_kanji_display = gtk_builder_get_object(kanjidic->definitions, 
                                                   "textbuffer_kanji_display");
  //get the  kanji informations from kdic
  gchar *kanji_info_line = get_line_from_dic(kanji, kanjidic->conf->kanjidic);
  kanjifile_entry *kanji_data= do_kdicline(kanji_info_line);

  //display the kanji display buffer
  gtk_text_buffer_set_text(textbuffer_kanji_display, "", 0);
  gtk_text_buffer_insert_at_cursor(textbuffer_kanji_display, kanji, strlen(kanji));
  gtk_text_buffer_insert_at_cursor(textbuffer_kanji_display, "\n", strlen("\n"));

  //Display informations on the kanji 
  //TODO: filter what to be displayed TODO: set font

  //strokes count
  gchar *tmp_entry = g_strdup_printf("Strokes:\t%d\n", kanji_data->stroke);
  gtk_text_buffer_insert_at_cursor(textbuffer_kanji_display, 
                                   tmp_entry, 
                                   strlen(tmp_entry));

  //translations
  for (kanji_data->translations;
       kanji_data->translations != NULL;
       kanji_data->translations = g_list_next(kanji_data->translations)) {

    gtk_text_buffer_insert_at_cursor(textbuffer_kanji_display,
                                     strcat(kanji_data->translations->data, "\n"), 
                                     strlen(kanji_data->translations->data) + 1);
  }

  g_free(tmp_entry);
}

void on_button_search_clicked(GtkWidget *widget, kanjidic *kanjidic) {
  search_and_display_kanji(kanjidic);
}

void on_button_show_radical_list_clicked(GtkButton *button, kanjidic *kanjidic){
  radical_list_update_sensitivity(kanjidic);

  GtkWindow *radicals = (GtkWindow*)gtk_builder_get_object(kanjidic->definitions, 
                                                         "radical_list");

  //set size and display the preference window
  gtk_window_set_default_size(GTK_WINDOW(radicals), 320, 220);
  gtk_widget_show_all ((GtkWidget*)radicals);
}

//Radical list callbacks
//the radical list is never deleted, just hidded (so it does not have to be 
//reconstructed from the builder and repopulated 
gboolean on_radical_list_delete_event(GtkWindow *window, kanjidic *kanjidic){
  gtk_widget_hide(window);
  return TRUE;
}

//a radical button in the radical button list has been clicked
void on_radical_button_clicked(GtkButton *button, kanjidic *kanjidic){

  //get the clicked kanji
  gchar* radical = gtk_button_get_label(button);

  //add it to the entry filter
  GtkEntry *entry_filter_radical = gtk_builder_get_object(kanjidic->definitions, 
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
