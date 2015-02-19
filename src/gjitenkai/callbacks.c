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

void on_menuitem_view_worddic_toggled(GtkCheckMenuItem *menu_item, gjitenkai *gjitenkai){
  GtkPaned *paned = gjitenkai->main_container;
  //gtk_paned_add1(paned, NULL);
  GtkWidget *box_worddic = (GtkWidget*)gtk_builder_get_object(gjitenkai->worddic->definitions, 
                                                              "box_toplevel");  


  if(gtk_check_menu_item_get_active(menu_item)){
    gtk_widget_show(box_worddic);
  }
  else{
    gtk_widget_hide(box_worddic);
  }
}

void on_menuitem_view_kanjidic_toggled(GtkMenuItem *menu_item, gjitenkai *gjitenkai){
  GtkPaned *paned = gjitenkai->main_container;
  //gtk_paned_add2(paned, NULL);
}

