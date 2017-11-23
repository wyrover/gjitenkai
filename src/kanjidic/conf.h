#ifndef KANJIDIC_CONF_H
#define KANJIDIC_CONF_H

#include <gio/gio.h>
#include <pango/pango-font.h>
#include <glib.h>

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <glib/gi18n.h>

#include "../config.h"

#include "kanjidic.h"

#include "../common/error.h"
#include "../common/constants.h"
#include "../common/dicfile.h"
#include "../common/dicutil.h"

struct _KanjidicConfig {
  struct _GjitenDicfile *kanjidic;

  const gchar *kanji_font;          //kanji to display' style
  GdkRGBA *kanji_color;

  const gchar *separator;           //separator between entry (katakana & hiragana)
  const gchar *kanji_result_font;   //font for the candidats

};

typedef struct _KanjidicConfig KanjidicConfig;

KanjidicConfig *kanjidic_conf_load();
void kanjidic_conf_save(KanjidicConfig *conf, GSettings *settings);

#endif
