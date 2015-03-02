#include <locale.h>
#include <libintl.h>

#include "kanjidic.h"

int main( int argc, char **argv )
{
  bindtextdomain("worddic", "/usr/local/share/locale");
  textdomain("worddic");
  
  kanjidic kanjidic;

  gtk_init (&argc, &argv);
  kanjidic_init (&kanjidic);

  //kanjidic window
  GtkWindow *window = (GtkWindow*)gtk_builder_get_object(kanjidic.definitions, 
                                                          "kanjidic");
  //box in the top of the window
  GtkWidget *box_top = (GtkWidget*)gtk_builder_get_object(kanjidic.definitions, 
                                                              "box_top");
  //box at toplevel
  GtkWidget *box_kanjidic = (GtkWidget*)gtk_builder_get_object(kanjidic.definitions, 
                                                              "box_toplevel");
  //Construct the pref dialog
  //pref dialog
  GtkWidget *dialog_vbox_prefs = (GtkWidget*)gtk_builder_get_object(kanjidic.definitions, 
                                                                    "dialog-vbox_prefs");

  //pref content box at toplevel
  GtkWidget *box_toplevel_prefs = (GtkWidget*)gtk_builder_get_object(kanjidic.definitions, 
                                                                     "box_toplevel_prefs");
  //preference pack
  gtk_box_pack_start(GTK_BOX(dialog_vbox_prefs), box_toplevel_prefs, TRUE, TRUE, 0);

  //add the kanjidic box in the topbox
  gtk_box_pack_start(GTK_BOX(box_top), box_kanjidic, TRUE, TRUE, 0);

  gtk_window_set_default_size(GTK_WINDOW(window), 440, 500);
  gtk_widget_show_all ((GtkWidget*)window);
  
  gtk_main ();

  return 1;
}
