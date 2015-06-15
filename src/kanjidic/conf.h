#ifndef KANJIDIC_CONF_H
#define KANJIDIC_CONF_H

#include <gio/gio.h>
#include <pango/pango-font.h>
#include <gtk/gtk.h>

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <glib/gi18n.h>

#include "../../config.h"

#include "kanjidic.h"
#include "error.h"

#include "../common/constants.h"
#include "../common/dicfile.h"
#include "../common/dicutil.h"

struct _KanjidicConfig {
  struct _GjitenDicfile *kanjidic;
  GdkRGBA *kanji_color;
  const gchar *kanji_font;
  const gchar *separator;

  GdkRGBA *kanji_result_color;
  const gchar *kanji_result_font;

};

typedef struct _KanjidicConfig KanjidicConfig;

KanjidicConfig *kanjidic_conf_load();
void kanjidic_conf_save(KanjidicConfig *conf, GSettings *settings);

#endif
