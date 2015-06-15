#ifndef WORDDIC_H
#define WORDDIC_H

#include <gtk/gtk.h>

#include "conf.h"
#include "../common/conf.h"
#include "../common/dicfile.h"
#include "inflection.h"

#define SETTINGS_WORDDIC "apps.gjitenkai.worddic"
#define UI_DEFINITIONS_FILE_WORDDIC GJITENKAI_DATADIR"/worddic.glade"

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
void highlight_result(GtkTextBuffer *textbuffer_search_results,
		      GtkTextTag *highlight,
		      const gchar *text_to_highlight);
#endif
