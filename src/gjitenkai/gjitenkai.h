#ifndef GJITENKAI_H
#define GJITENKAI_H

#include <gtk/gtk.h>

#include "../worddic/worddic.h"
#include "../kanjidic/kanjidic.h"

#define UI_DEFINITIONS_FILE_GJITENKAI "UI/gjitenkai.glade"

typedef struct gjitenkai_t
{
  GtkBuilder *definitions;
  worddic   *worddic;
  kanjidic  *kanjidic;

  //display worddic and kanjidic in a pane
  GtkWidget *main_container;
  
} gjitenkai;


void gjitenkai_init (gjitenkai * );

#endif
