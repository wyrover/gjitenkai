#ifndef DICUTIL_H
#define DICUTIL_H

#include <glib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>

#include "dicfile.h"
#include "error.h"

gchar *get_eof_line(gchar *ptr, gchar *end_ptr);
void to_utf8(gunichar c, char* utf8_c);

gchar *hira2kata(gchar *hirastr);
gchar *kata2hira(gchar *hirastr);
gchar *full2half(gchar *instr);
gboolean isKanaChar(gunichar c);
gboolean isKatakanaChar(gunichar c);
gboolean isHiraganaChar(gunichar c);
gboolean isKanjiChar(gunichar c);
gboolean isJPChar(gunichar c);
gboolean isOtherChar(gunichar c);

/*0 if no more words in src, else new pos*/
int get_word(char *dest, char *src, int size, int pos);
int strg_end_compare(gchar *strg1, gchar *strg2);
int get_jp_match_type(gchar *line, gchar *srchstrg, int offset);
gboolean is_kanji_only(gchar *line);
gboolean isHiraganaString(gchar *strg);
gboolean isKatakanaString(gchar *strg);
gboolean detect_japanese(gchar *srchstrg);
#endif
