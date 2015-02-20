#include <locale.h>
#include <libintl.h>

#include "worddic.h"

int main( int argc, char **argv )
{
  //local
  // Set the current local to default
  setlocale(LC_ALL, "fr_FR");

  // bindtextdomain(DOMAINNAME, DIRNAME)
  //
  // Specify that the DOMAINNAME message catalog
  // will be found in DIRNAME rather than in 
  // the system locale data base.
  bindtextdomain("worddic", "/usr/share/locale");

  // testdomain(DOMAINNAME)
  //
  // Set the current default message catalog to DOMAINNAME.
  textdomain("worddic");

  worddic worddic;

  gtk_init (&argc, &argv);
  worddic_init (&worddic);
  
  //construct the worrdic main window
  //worddic window
  GtkWindow *window = (GtkWindow*)gtk_builder_get_object(worddic.definitions, 
                                                          "worddic");
  //box at the top of the worddic window
  GtkWidget *box_top = (GtkWidget*)gtk_builder_get_object(worddic.definitions, 
                                                              "box_top");

  //box at toplevel
  GtkWidget *box_worddic = (GtkWidget*)gtk_builder_get_object(worddic.definitions, 
                                                              "box_toplevel");
  
  gtk_box_pack_start(box_top, box_worddic, TRUE, TRUE, 0);

  gtk_window_set_default_size(GTK_WINDOW(window), 640, 320);

  //construct the worrdic prefs dialog
  //box of the prefs dialog
  GtkWidget *dialog_vbox_prefs = (GtkWidget*)gtk_builder_get_object(worddic.definitions, 
                                                                    "dialog-vbox_prefs");

  //preferences box at toplevel
  GtkWidget *box_toplevel_prefs = (GtkWidget*)gtk_builder_get_object(worddic.definitions, 
                                                                     "box_toplevel_prefs");
  gtk_box_pack_start(dialog_vbox_prefs, box_toplevel_prefs, TRUE, TRUE, 0);


  //show the main window
  gtk_widget_show_all ((GtkWidget*)window);

  gtk_main ();

  return 1;
}
