#include "gjitenkai.h"

void gjitenkai_init (gjitenkai *gjitenkai)
{
  GError *err = NULL;
  gjitenkai->definitions = gtk_builder_new ();
  gtk_builder_add_from_file (gjitenkai->definitions,
                             UI_DEFINITIONS_FILE_GJITENKAI, &err);  
  if (err != NULL) {
    g_printerr
      ("Error while loading gjitenkai definitions file: %s\n",
       err->message);
    g_error_free (err);
    gtk_main_quit ();
  }
  gtk_builder_connect_signals (gjitenkai->definitions, gjitenkai);
}

