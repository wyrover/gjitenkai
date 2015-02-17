#ifndef CONF_H
#define CONF_H

#include <gio/gio.h>
#include <pango/pango-font.h>
#include <gtk/gtk.h>

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <glib/gi18n.h>

#include "../../config.h"
#include "error.h"

#include "constants.h"
#include "dicfile.h"
#include "dicutil.h"

struct _GjitenConfig {
  gchar *version;

  //Worddic configuration
  GSList *dicfile_list;

  guint maxwordmatches;
  gchar *resultsfont;
  GtkTextTag *highlight;
  GdkRGBA *results_highlight_color;

  gboolean search_kata_on_hira;
  gboolean search_hira_on_kata;
  gboolean verb_deinflection;

  gboolean searchlimit_enabled;
  gboolean autoadjust_enabled;

  struct _GjitenDicfile *selected_dic;
  struct _GjitenDicfile *mmaped_dicfile;
  PangoFontDescription *normalfont_desc;

  //Kanjidic configuration
  struct _GjitenDicfile *kanjidic;
  GdkRGBA *kanji_color;
  gchar *kanji_font;
  gchar *separator;
};

typedef struct _GjitenConfig GjitenConfig;

GjitenConfig *conf_load();
void conf_save(GjitenConfig *conf);
void conf_save_history(GList *history, GjitenConfig *conf);
void conf_save_options(GjitenConfig *conf);
gboolean conf_init_handler();
void conf_close_handler();
void dicutil_unload_dic();
#endif
