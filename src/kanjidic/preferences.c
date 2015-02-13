#include "kanjidic.h"
//init
void init_prefs_kanjidic(kanjidic *kanjidic){
  //init the kdic file chooser button title with the path of the dict
  GtkFileChooserButton *filechooserbutton = gtk_builder_get_object(kanjidic->definitions, 
								   "filechooserbutton_kdic");
  gtk_file_chooser_select_filename(filechooserbutton, kanjidic->conf->kanjidic->path);
}

//callback
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
