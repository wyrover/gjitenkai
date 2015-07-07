#ifndef WORDDIC_H
#define WORDDIC_H

#include <gtk/gtk.h>

#include "conf.h"
#include "../common/conf.h"
#include "../common/dicfile.h"
#include "inflection.h"
#include "unit_style.h"

#define SETTINGS_WORDDIC "gjitenkai.worddic"

#ifdef MINGW
	#define UI_DEFINITIONS_FILE_WORDDIC path_relative("..\\share\\data\\worddic.glade")
#else
	#define UI_DEFINITIONS_FILE_WORDDIC GJITENKAI_DATADIR"/worddic.glade"
#endif

GdkCursor *cursor_selection;
GdkCursor *cursor_default;

typedef struct worddic_t
{
  GtkBuilder *definitions;
  GSettings *settings;
  struct _WorddicConfig *conf;

  gint match_criteria_lat;
  gint match_criteria_jp;
} worddic;


void worddic_init (worddic * );
void init_search_menu(worddic *);
void print_unit(GtkTextBuffer *textbuffer,
                gchar *text, unit_style *style);
void print_entry(GtkTextBuffer *textbuffer, GList *entries, worddic *worddic);
void highlight_result(GtkTextBuffer *textbuffer,
		      GtkTextTag *highlight,
		      const gchar *text_to_highlight,
                      GtkTextIter *iter_from);
#endif
