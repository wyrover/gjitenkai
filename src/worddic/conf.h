#ifndef WORDDIC_CONF_H
#define WORDDIC_CONF_H

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <pango/pango-font.h>

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <glib/gi18n.h>

#include "../../config.h"

#include "worddic.h"
#include "worddic_dicfile.h"
#include "unit_style.h"

#include "../common/error.h"
#include "../common/conf.h"
#include "../common/constants.h"
#include "../common/dicfile.h"
#include "../common/dicutil.h"

typedef struct _WorddicConfig {
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
  unit_style jap_def;
  unit_style jap_reading;
  unit_style gloss;
  unit_style subgloss;
  unit_style notes;
  
  //search options
  gboolean search_kata_on_hira;
  gboolean search_hira_on_kata;
  gboolean verb_deinflection;

  gboolean searchlimit_enabled;
  gboolean autoadjust_enabled;

  //struct _GjitenDicfile *selected_dic;
  PangoFontDescription *normalfont_desc;
}WorddicConfig;

WorddicConfig *worddic_conf_load(struct worddic_t *p_worddic);
void worddic_conf_save(struct worddic_t *p_worddic);

#endif
