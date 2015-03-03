#include "gjitenkai.h"

void on_gjitenkai_menuitem_prefs_activate(GtkButton *button, gjitenkai *gjitenkai){
  //set size and display the preference window
  GtkDialog *prefs = (GtkDialog*)gtk_builder_get_object(gjitenkai->definitions, 
                                                               "prefs");
  gtk_window_set_default_size(GTK_WINDOW(prefs), 420, 280);
  gtk_widget_show_all ((GtkWidget*)prefs);
}

void on_gjitenkai_button_prefs_OK_clicked(GtkButton *button, gjitenkai *gjitenkai){
 GtkDialog *prefs = (GtkDialog*)gtk_builder_get_object(gjitenkai->definitions, 
                                                               "prefs");
 gtk_widget_hide(GTK_WIDGET(prefs));
}

void on_menuitem_view_worddic_toggled(GtkCheckMenuItem *menu_item, gjitenkai *gjitenkai){
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
  GtkWidget *box_kanjidic = (GtkWidget*)gtk_builder_get_object(gjitenkai->kanjidic->definitions, 
							       "box_toplevel");  
  if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menu_item))){
    gtk_widget_show(box_kanjidic);
  }
  else{
    gtk_widget_hide(box_kanjidic);
  }
}

void on_menuitem_paned_toggled(GtkMenuItem *menu_item, gjitenkai *gjitenkai){
  GtkWidget *box_worddic = (GtkWidget*)gtk_builder_get_object(gjitenkai->worddic->definitions, 
                                                              "box_toplevel");
  GtkWidget *box_kanjidic = (GtkWidget*)gtk_builder_get_object(gjitenkai->kanjidic->definitions, 
                                                               "box_toplevel");
  GtkWindow *box_top = (GtkWindow*)gtk_builder_get_object(gjitenkai->definitions, 
                                                          "box_top");  
  //remove the worddic and kanjidic boxes from the notebook pages'box
  GtkWidget *page_worddic = gtk_notebook_get_nth_page(gjitenkai->notebook, 0);
  GtkWidget *page_kanjidic = gtk_notebook_get_nth_page(gjitenkai->notebook, 1);
  
  g_object_ref(box_worddic);
  g_object_ref(box_kanjidic);  
  gtk_container_remove (GTK_CONTAINER(page_worddic), GTK_WIDGET(box_worddic));
  gtk_container_remove (GTK_CONTAINER(page_kanjidic), GTK_WIDGET(box_kanjidic));
  
  //remove the notebook from the window top box
  g_object_ref(gjitenkai->notebook);
  gtk_container_remove (GTK_CONTAINER(box_top), GTK_WIDGET(gjitenkai->notebook));

  //add worddic and kanjidic to the pane
  gtk_paned_add1(gjitenkai->paned, box_worddic);
  gtk_paned_add2(gjitenkai->paned, box_kanjidic);  
  
  //add the pane to the box top
  gtk_box_pack_start(GTK_BOX(box_top), GTK_WIDGET(gjitenkai->paned), TRUE, TRUE, 0);

  //show
  gtk_widget_show_all(GTK_WIDGET(box_top));
}

void on_menuitem_notebook_toggled(GtkMenuItem *menu_item, gjitenkai *gjitenkai){

  GtkWidget *box_worddic = (GtkWidget*)gtk_builder_get_object(gjitenkai->worddic->definitions, 
                                                              "box_toplevel");
  GtkWidget *box_kanjidic = (GtkWidget*)gtk_builder_get_object(gjitenkai->kanjidic->definitions, 
                                                               "box_toplevel");
  GtkWindow *box_top = (GtkWindow*)gtk_builder_get_object(gjitenkai->definitions, 
                                                          "box_top");  

  //remove the worddic and kanjidic from the pane
  g_object_ref(box_worddic);
  g_object_ref(box_kanjidic);
  gtk_container_remove (GTK_CONTAINER(gjitenkai->paned), box_worddic);
  gtk_container_remove (GTK_CONTAINER(gjitenkai->paned), box_kanjidic);

  //add the worddic and kanjidic box to the notebook pages' box
  GtkWidget *page_worddic = gtk_notebook_get_nth_page(gjitenkai->notebook, 0);
  GtkWidget *page_kanjidic = gtk_notebook_get_nth_page(gjitenkai->notebook, 1);

  gtk_box_pack_start(GTK_BOX(page_worddic), box_worddic, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(page_kanjidic), box_kanjidic, TRUE, TRUE, 0);
  
  //remove the pane from the window top box
  g_object_ref(gjitenkai->paned);
  gtk_container_remove (GTK_CONTAINER(box_top), GTK_WIDGET(gjitenkai->paned));
    
  //add the notebook to the top box
  gtk_box_pack_start(GTK_BOX(box_top), GTK_WIDGET(gjitenkai->notebook), TRUE, TRUE, 0);
  
  gtk_widget_show_all(GTK_WIDGET(box_top));
}
