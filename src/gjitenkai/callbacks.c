#include "gjitenkai.h"

void on_gjitenkai_menuitem_prefs_activate(GtkButton *button, gjitenkai *gjitenkai){
  //set size and display the preference window
  GtkDialog *prefs = (GtkWindow*)gtk_builder_get_object(gjitenkai->definitions, 
                                                               "prefs");
  gtk_window_set_default_size(GTK_WINDOW(prefs), 420, 280);
  gtk_widget_show_all ((GtkWidget*)prefs);
}

void on_gjitenkai_button_prefs_OK_clicked(GtkButton *button, gjitenkai *gjitenkai){
 GtkDialog *prefs = (GtkWindow*)gtk_builder_get_object(gjitenkai->definitions, 
                                                               "prefs");
 gtk_widget_hide(prefs);
}
