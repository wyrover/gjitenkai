#include <locale.h>
#include <libintl.h>

#include "gjitenkai.h"

extern gboolean on_gjitenkai_search_results_button_release_event(GtkWidget *text_view,
                                                                 GdkEventButton *event,
                                                                 gjitenkai *gjitenkai);

int main( int argc, char **argv )
{
  bindtextdomain("gjitenkai", GJITENKAI_LOCALDIR);
  textdomain("gjitenkai");
  bindtextdomain("worddic", GJITENKAI_LOCALDIR);
  textdomain("worddic");
  bindtextdomain("kanjidic", GJITENKAI_LOCALDIR);
  textdomain("kanjidic");
  
  gjitenkai gjitenkai;
  gjitenkai.worddic = g_new0(worddic, 1);
  gjitenkai.kanjidic = g_new0(kanjidic, 1);

  //init gtk
  gtk_init (&argc, &argv);

  //init application
  gjitenkai_init (&gjitenkai);
  worddic_init(gjitenkai.worddic);
  kanjidic_init(gjitenkai.kanjidic);
  
  GtkWindow *window = (GtkWindow*)gtk_builder_get_object(gjitenkai.definitions, 
                                                         "gjiten");
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 400);
  GdkPixbuf *pixbuf;
  GError *error = NULL;
  
  pixbuf = gdk_pixbuf_new_from_file(GJITENKAI_PIXMAPDIR"/kai.png", &error);
   
  gtk_window_set_icon(GTK_WINDOW(window), pixbuf);
 
  //get the top level box of worddic and kanjidic
  GtkWidget *box_worddic = (GtkWidget*)gtk_builder_get_object(gjitenkai.worddic->definitions, 
                                                              "box_toplevel");
  GtkWidget *box_kanjidic = (GtkWidget*)gtk_builder_get_object(gjitenkai.kanjidic->definitions, 
                                                               "box_toplevel");

  //create the paned widget and the notebook
  gjitenkai.paned = (GtkPaned*)gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
  gjitenkai.notebook = (GtkNotebook*)gtk_notebook_new();

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

  gtk_box_pack_start(GTK_BOX(box_top), GTK_WIDGET(gjitenkai.paned), TRUE, TRUE, 0);
  
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
  gtk_notebook_append_page_menu (GTK_NOTEBOOK(notebook_apps),
                                 worddic_box_toplevel_prefs,
                                 gtk_label_new("Worddic"),
                                 NULL);  
  gtk_notebook_append_page_menu (GTK_NOTEBOOK(notebook_apps),
                                 kanjidic_box_toplevel_prefs,
                                 gtk_label_new("Kanjidic"),
                                 NULL);  


  //append the search menuitems of worddic to the gjitenkai main menubar
  //main menubar from gjitenkai
  GtkMenuBar* main_menu_bar = (GtkMenuBar*)gtk_builder_get_object(gjitenkai.definitions, 
                                                                  "main_menubar");

  //worddic search menu item
  GtkMenuItem* menu_item_search = (GtkMenuItem*)gtk_builder_get_object(gjitenkai.worddic->definitions, 
                                                                       "menuitem_search");

  //remove the search item from the worddic menubar
  GtkMenuBar* worddic_menubar = (GtkMenuBar*)gtk_builder_get_object(gjitenkai.worddic->definitions, 
                                                                    "menubar");
  g_object_ref(menu_item_search);
  gtk_container_remove (GTK_CONTAINER(worddic_menubar), GTK_WIDGET(menu_item_search));

  //add the search item to the gjitenkai menubar
  gtk_menu_shell_insert(GTK_MENU_SHELL(main_menu_bar),
                        GTK_WIDGET(menu_item_search),
                        3);
  
  //recreate the accelerators
  GtkAccelGroup *accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  
  GtkRadioMenuItem* radio_item = NULL;

  //jap exact
  radio_item = (GtkRadioMenuItem*)gtk_builder_get_object(gjitenkai.worddic->definitions, 
                                                         "menuitem_search_japanese_exact");
  
  gtk_widget_add_accelerator(GTK_WIDGET(radio_item), "activate", accel_group, 
  'x', GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  
  //jap any
  radio_item = (GtkRadioMenuItem*)gtk_builder_get_object(gjitenkai.worddic->definitions, 
                                                         "menuitem_search_japanese_any");
  
  gtk_widget_add_accelerator(GTK_WIDGET(radio_item), "activate", accel_group, 
  'y', GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  //jap start
  radio_item = (GtkRadioMenuItem*)gtk_builder_get_object(gjitenkai.worddic->definitions, 
                                                         "menuitem_search_japanese_start");
  
  gtk_widget_add_accelerator(GTK_WIDGET(radio_item), "activate", accel_group, 
  's', GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  
  //jap end
  radio_item = (GtkRadioMenuItem*)gtk_builder_get_object(gjitenkai.worddic->definitions, 
                                                         "menuitem_search_japanese_end");
  
  gtk_widget_add_accelerator(GTK_WIDGET(radio_item), "activate", accel_group, 
  'e', GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  
  //latin whole expression
  radio_item = (GtkRadioMenuItem*)gtk_builder_get_object(gjitenkai.worddic->definitions, 
                                                         "menuitem_search_latin_whole_expressions");
  
  gtk_widget_add_accelerator(GTK_WIDGET(radio_item), "activate", accel_group, 
  'e', GDK_CONTROL_MASK | GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

  //latin whole words
  radio_item = (GtkRadioMenuItem*)gtk_builder_get_object(gjitenkai.worddic->definitions, 
                                                         "menuitem_search_latin_whole_words");
  
  gtk_widget_add_accelerator(GTK_WIDGET(radio_item), "activate", accel_group, 
  'w', GDK_CONTROL_MASK | GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

  //latin any
  radio_item = (GtkRadioMenuItem*)gtk_builder_get_object(gjitenkai.worddic->definitions, 
                                                         "menuitem_search_latin_any");
  
  gtk_widget_add_accelerator(GTK_WIDGET(radio_item), "activate", accel_group, 
  'y', GDK_CONTROL_MASK | GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

  //callback when the worrdic search result is clicked (search kanji)
  GtkTextView *result_text_view = (GtkTextView *)gtk_builder_get_object(gjitenkai.worddic->definitions,
                                                                        "search_results");
  g_signal_connect(result_text_view, 
                   "button-release-event", 
                   G_CALLBACK(on_gjitenkai_search_results_button_release_event), 
                   &gjitenkai);

  gtk_widget_show_all ((GtkWidget*)window);
  
  gtk_main ();

  return 1;
}
