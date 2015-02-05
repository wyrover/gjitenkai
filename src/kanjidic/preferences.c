#include "kanjidic.h"

void on_kanjidic_button_OK_clicked(GtkButton* button, kanjidic *kanjidic){
  GtkDialog *dialog_prefs = (GtkWindow*)gtk_builder_get_object(kanjidic->definitions, 
                                                                  "dialog_preferences");
  gtk_widget_hide (dialog_prefs);
  
}
