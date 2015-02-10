#include "kanjidic.h"

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
