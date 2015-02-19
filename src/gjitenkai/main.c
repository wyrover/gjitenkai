#include "gjitenkai.h"

int main( int argc, char **argv )
{
  gjitenkai gjitenkai;
  gjitenkai.worddic = g_new0(worddic, 1);
  gjitenkai.kanjidic = g_new0(kanjidic, 1);

  gtk_init (&argc, &argv);
  gjitenkai_init (&gjitenkai);
  worddic_init(gjitenkai.worddic);
  kanjidic_init(gjitenkai.kanjidic);
  
  GtkWindow *window = (GtkWindow*)gtk_builder_get_object(gjitenkai.definitions, 
                                                         "gjiten");
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 400);

  //get the top level box of worddic and kanjidic
  GtkWidget *box_worddic = (GtkWidget*)gtk_builder_get_object(gjitenkai.worddic->definitions, 
                                                              "box_toplevel");
  GtkWidget *box_kanjidic = (GtkWidget*)gtk_builder_get_object(gjitenkai.kanjidic->definitions, 
                                                               "box_toplevel");

  //create the paned widget and the notbook
  gjitenkai.paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
  gjitenkai.notebook = gtk_notebook_new();

  //by default, use the paned widget
  gtk_paned_pack1(gjitenkai.paned, box_worddic, TRUE, FALSE);
  gtk_paned_pack2(gjitenkai.paned, box_kanjidic, TRUE, FALSE);
  gtk_paned_set_position(gjitenkai.paned, 400);

  //create the page where to append the worddic and kanjidic boxes if the
  //selected view changes to notebook
  gtk_notebook_append_page_menu (gjitenkai.notebook,
				 gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0),
                                 gtk_label_new("Worddic"),
                                 NULL);  
  gtk_notebook_append_page_menu (gjitenkai.notebook,
                                 gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0),
                                 gtk_label_new("Kanjidic"),
                                 NULL);
  
  //append the main container to the gjiten box_top
  GtkWindow *box_top = (GtkWindow*)gtk_builder_get_object(gjitenkai.definitions, 
                                                          "box_top");

  gtk_box_pack_start(box_top, gjitenkai.paned, TRUE, TRUE, 0);
  
  //Construct the pref dialog
  //pref content box at toplevel for worddic and kanjidic
  GtkWidget *worddic_box_toplevel_prefs = (GtkWidget*)gtk_builder_get_object(gjitenkai.worddic->definitions, 
                                                                             "box_toplevel_prefs");
  GtkWidget *kanjidic_box_toplevel_prefs = (GtkWidget*)gtk_builder_get_object(gjitenkai.kanjidic->definitions, 
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


  gtk_widget_show_all ((GtkWidget*)window);
  
  gtk_main ();

  return 1;
}
