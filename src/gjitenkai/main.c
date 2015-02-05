#include "gjitenkai.h"

int main( int argc, char **argv )
{
  gjitenkai gjitenkai;
  worddic   worddic;
  kanjidic  kanjidic;

  gtk_init (&argc, &argv);
  gjitenkai_init (&gjitenkai);
  worddic_init(&worddic);
  kanjidic_init(&kanjidic);
  
  GtkWindow *window = (GtkWindow*)gtk_builder_get_object(gjitenkai.definitions, 
                                                         "gjiten");
  gtk_window_set_default_size(GTK_WINDOW(window), 650, 500);
  //get the top level box of worddic and kanjidic
  GtkWidget *box_worddic = (GtkWidget*)gtk_builder_get_object(worddic.definitions, 
                                                              "box_toplevel");
  GtkWidget *box_kanjidic = (GtkWidget*)gtk_builder_get_object(kanjidic.definitions, 
                                                               "box_toplevel");

  //display worddic and kanjidic in a pane
  GtkWidget *main_container;

  main_container = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_paned_add1(main_container, box_worddic);
  gtk_paned_add2(main_container, box_kanjidic);

  //append the main container to the gjiten box_top
  GtkWindow *box_top = (GtkWindow*)gtk_builder_get_object(gjitenkai.definitions, 
                                                          "box_top");
  //Construct the pref dialog
  //pref content box at toplevel for worddic and kanjidic
  GtkWidget *worddic_box_toplevel_prefs = (GtkWidget*)gtk_builder_get_object(worddic.definitions, 
                                                                             "box_toplevel_prefs");
  GtkWidget *kanjidic_box_toplevel_prefs = (GtkWidget*)gtk_builder_get_object(kanjidic.definitions, 
                                                                              "box_toplevel_prefs");
  //append the worddic and kanjidic preferences boxes in the gjitenkai 
  //applications notebook
  GtkWidget *notebook_apps = (GtkWidget*)gtk_builder_get_object(gjitenkai.definitions, 
                                                                "notebook_apps");
  gtk_notebook_append_page_menu (notebook_apps,
                                 worddic_box_toplevel_prefs,
                                 gtk_label_new("Worddic"),
                                 NULL);  
  gtk_notebook_append_page_menu (notebook_apps,
                                 kanjidic_box_toplevel_prefs,
                                 gtk_label_new("Kanjidic"),
                                 NULL);  


  gtk_box_pack_start(box_top, main_container, TRUE, TRUE, 0);
  gtk_widget_show_all ((GtkWidget*)window);
  
  gtk_main ();

  return 1;
}
