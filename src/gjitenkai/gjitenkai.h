#ifndef GJITENKAI_H
#define GJITENKAI_H

#include <gtk/gtk.h>

#include "../../config.h"
#include "../worddic/worddic.h"
#include "../kanjidic/kanjidic.h"

#ifdef MINGW
	#define UI_DEFINITIONS_FILE_GJITENKAI path_relative("..\\share\\gjitenkai\\gjitenkai.glade")
#else
	#define UI_DEFINITIONS_FILE_GJITENKAI GJITENKAI_DATADIR"/gjitenkai.glade"
#endif

typedef struct gjitenkai_t
{
  GtkBuilder *definitions;
  worddic   *worddic;
  kanjidic  *kanjidic;

  //display worddic and kanjidic in a pane or notebook
  GtkNotebook *notebook;
  GtkPaned *paned;
  
} gjitenkai;


void gjitenkai_init (gjitenkai * );

#endif
