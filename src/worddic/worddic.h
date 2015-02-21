#ifndef WORDDIC_H
#define WORDDIC_H

#include <gtk/gtk.h>

#include "../common/conf.h"
#include "../common/dicfile.h"
#include "inflection.h"

#define UI_DEFINITIONS_FILE_WORDDIC "UI/worddic.glade"

typedef struct worddic_t
{
  GtkBuilder *definitions;

  GjitenConfig *conf;

  gint match_criteria_lat;
  gint match_criteria_jp;
} worddic;


void worddic_init (worddic * );
void highlight_result(GtkTextBuffer *textbuffer_search_results,
		      GtkTextTag *highlight,
		      gchar *text_to_highlight);
#endif
