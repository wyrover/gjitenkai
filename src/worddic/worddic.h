#ifndef WORDDIC_H
#define WORDDIC_H

#include <gtk/gtk.h>

#include <locale.h>
#include <libintl.h>

#include "conf.h"
#include "inflection.h"
#include "unit_style.h"
#include "../common/conf.h"
#include "../common/dicfile.h"

#define SETTINGS_WORDDIC "gjitenkai.worddic"

#ifdef MINGW
	#define UI_DEFINITIONS_FILE_WORDDIC path_relative("..\\share\\gjitenkai\\worddic.glade")
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

  GThread   *thread_load_dic;
  
  GList *results;
  gint entries_per_page;   //number of entries to display per page
  gint current_page;       //current page to be displayed
} worddic;


extern void init_prefs_window(worddic *worddic);

void worddic_init (worddic * );
void init_search_menu(worddic *);
void print_unit(GtkTextBuffer *textbuffer,
                gchar *text, unit_style *style);
void print_entries(GtkTextBuffer *textbuffer, worddic *p_worddic);
void highlight_result(GtkTextBuffer *textbuffer,
		      GtkTextTag *highlight,
		      const gchar *text_to_highlight,
                      GtkTextIter *iter_from);

#if GTK_MAJOR_VERSION >= 3 && GTK_MINOR_VERSION >= 16
extern void on_worddic_search_results_edge_reached(GtkScrolledWindow* sw,
						   GtkPositionType pos,
						   worddic* p_worddic);
#endif

#endif
