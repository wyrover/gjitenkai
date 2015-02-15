#include "kanjidic.h"
#include "kanji_item.h"

gint cmp_name(gconstpointer a,
                 gconstpointer b){
  kanji_item *ki1 = a;
  kanji_item *ki2 = b;
  return strcmp(ki1->name, ki2->name);
}

void on_kanji_item_toggled(GtkCheckButton* checkbutton, kanjidic *kanjidic){
  gboolean toggled = gtk_toggle_button_get_active(checkbutton);
  gchar* name = gtk_button_get_label(checkbutton);

  //search for the kanji item from the button name
  kanji_item tmp_ki;
  tmp_ki.name = name;
  GSList* kanji_item_head = g_slist_find_custom (kanji_item_list, &tmp_ki, cmp_name);
  
  if(kanji_item_head){
    kanji_item *ki = kanji_item_head->data;
    ki->active = toggled;
    kanji_item_save(ki);
  }
}

//init
void init_prefs_kanjidic(kanjidic *kanjidic){
  //init the kdic file chooser button title with the path of the dict
  GtkFileChooserButton *filechooserbutton = gtk_builder_get_object(kanjidic->definitions, 
								   "filechooserbutton_kdic");
  gtk_file_chooser_select_filename(filechooserbutton, kanjidic->conf->kanjidic->path);

  //init the kanji font chooser
  GtkFontButton *font_button = gtk_builder_get_object(kanjidic->definitions, "fontbutton_kanji");
  gtk_font_button_set_font_name (font_button, kanjidic->conf->kanji_font);
  
  //init the kanji color chooser
  GtkColorChooser *color_chooser = (GtkColorChooser*)gtk_builder_get_object(kanjidic->definitions, 
                                                                            "colorbutton_kanji");
  
  gtk_color_chooser_set_rgba(color_chooser, kanjidic->conf->kanji_color);
  
  //init the separator entry
  GtkEntry *entry_separator = gtk_builder_get_object(kanjidic->definitions, 
                                                          "entry_separator");  
  gtk_entry_set_text(entry_separator, kanjidic->conf->separator);
  
  //init the item list, expose what must be displayed in the kanji area

  GtkListBox *listbox_item = gtk_builder_get_object(kanjidic->definitions,
						     "listbox_kdic_item");
  kanji_item_list_init();
  GSList* kanji_item_head = kanji_item_list;
  while (kanji_item_head != NULL) {
    //add the kanji item with a checkbox TODO position
    GtkBox *box_item = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    kanji_item *kanji_item = kanji_item_head->data;
    GtkCheckButton *display_item = gtk_check_button_new_with_label(kanji_item->name);
    gtk_toggle_button_set_active (display_item, kanji_item->active);
    //GtkButton *btn_up = gtk_button_new_with_label("UP");
    //GtkButton *btn_down = gtk_button_new_with_label("DOWN");
    gtk_box_pack_start(box_item, display_item, TRUE, FALSE, 0);
    //gtk_box_pack_start(box_item, btn_up, FALSE, FALSE, 0);
    //gtk_box_pack_start(box_item, btn_down, FALSE, FALSE, 0);
    //insert the listbox in the list
    gtk_list_box_insert (listbox_item, box_item, -1);
    g_signal_connect(display_item, "toggled", on_kanji_item_toggled, kanjidic);

    //gtk_drag_source_set(box_item, GDK_BUTTON1_MASK, NULL, 0, GDK_ACTION_MOVE);

    kanji_item_head = g_slist_next(kanji_item_head);
  }
}

//callback
void on_entry_separator_activate(GtkEntry *entry, kanjidic *kanjidic){
  kanjidic->conf->separator = gtk_entry_get_text(entry);

  conf_save(kanjidic->conf);
}

void on_kanjidic_button_OK_clicked(GtkButton* button, kanjidic *kanjidic){
  GtkDialog *dialog_prefs = (GtkWindow*)gtk_builder_get_object(kanjidic->definitions, 
                                                                  "dialog_preferences");
  gtk_widget_hide (dialog_prefs);
  
}

void on_fontbutton_kanji_font_set(GtkFontButton *font_button, 
                                  kanjidic *kanjidic){
  const gchar *font_name= gtk_font_button_get_font_name (font_button);
  kanjidic->conf->kanji_font = font_name;

  g_object_set(kanjidic->texttag_kanji, "font",
               kanjidic->conf->kanji_font, NULL);

  conf_save(kanjidic->conf);
}

void on_colorbutton_kanji_color_set(GtkColorChooser *color_chooser, 
                                  kanjidic *kanjidic){
  //get the color
  gtk_color_chooser_get_rgba(color_chooser, 
                             kanjidic->conf->kanji_color);

  //set the foreground color of the kanji texttag
  g_object_set(kanjidic->texttag_kanji, "foreground-rgba",
               kanjidic->conf->kanji_color, NULL);

  //save this value
  conf_save(kanjidic->conf);  
}

void on_filechooserbutton_kdic_file_set(GtkFileChooserButton *filechooserbutton,
					kanjidic *kanjidic){
  GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(filechooserbutton));

  //reload kanjidic
  g_free(kanjidic->conf->kanjidic);
  kanjidic->conf->kanjidic = g_new0(GjitenDicfile, 1);
  kanjidic->conf->kanjidic->path = g_file_get_path(file);
  kanjidic->conf->kanjidic->name = g_file_get_basename(file);

  //unload dictionary in conf
  dicutil_unload_dic();

  //init the kanjidic with the selected path
  dicfile_init(kanjidic->conf->kanjidic);
  
  
  conf_save(kanjidic->conf);
}
