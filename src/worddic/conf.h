#ifndef WORDDIC_CONF_H
#define WORDDIC_CONF_H

#include <gio/gio.h>
#include <pango/pango-font.h>
#include <gtk/gtk.h>

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <glib/gi18n.h>

#include "../../config.h"

#include "worddic.h"
#include "worddic_dicfile.h"
#include "error.h"

#include "../common/conf.h"
#include "../common/constants.h"
#include "../common/dicfile.h"
#include "../common/dicutil.h"

struct _WorddicConfig {
  gchar *version;

  GSList *dicfile_list;

  guint maxwordmatches;

  //font
  //default
  gchar *resultsfont;

  //highlight
  GtkTextTag *highlight;
  GdkRGBA *results_highlight_color;

  ////japanese definition
  GtkTextTag *jap_def;
  gchar *jap_def_font;
  GdkRGBA *jap_def_color;
  gchar *jap_def_start;
  gchar *jap_def_end;

  ////japanese reading
  GtkTextTag *jap_reading;
  gchar *jap_reading_font;
  GdkRGBA *jap_reading_color;
  gchar *jap_reading_start;
  gchar *jap_reading_end;

  ////translation
  GtkTextTag *translation;
  gchar *translation_font;
  GdkRGBA *translation_color;  
  gchar *translation_start;
  gchar *translation_end;

  //search options
  gboolean search_kata_on_hira;
  gboolean search_hira_on_kata;
  gboolean verb_deinflection;

  gboolean searchlimit_enabled;
  gboolean autoadjust_enabled;

  struct _GjitenDicfile *selected_dic;
  struct _GjitenDicfile *mmaped_dicfile;
  PangoFontDescription *normalfont_desc;
};

typedef struct _WorddicConfig WorddicConfig;

WorddicConfig *worddic_conf_load(struct worddic_t *p_worddic);
void worddic_conf_save(struct worddic_t *p_worddic);

#endif
