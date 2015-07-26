#ifndef DICUTIL_H
#define DICUTIL_H

#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "dicfile.h"
#include "error.h"

gchar *read_file(const gchar *filename);
gchar *get_eof_line(gchar *ptr, gchar *end_ptr);
int get_word(char *dest, const char *src, int size, int pos);
void to_utf8(gunichar c, char* utf8_c);

gchar *hira2kata(const gchar *hirastr);
gchar *kata2hira(const gchar *hirastr);
gchar *full2half(const gchar *instr);
gboolean isKanaChar(const gunichar c);
gboolean isKatakanaChar(const gunichar c);
gboolean isHiraganaChar(const gunichar c);
gboolean isKanjiChar(const gunichar c);
gboolean isJPChar(const gunichar c);
gboolean isOtherChar(const gunichar c);

int strg_end_compare(const gchar *strg1, const gchar *strg2);
int get_jp_match_type(gchar *line, const gchar *srchstrg, int offset);
gboolean is_kanji_only(const gchar *line);
gboolean isHiraganaString(const gchar *strg);
gboolean isKatakanaString(const gchar *strg);
gboolean hasHiraganaString(const gchar *strg);
gboolean hasKatakanaString(const gchar *strg);
gboolean detect_japanese(const gchar *srchstrg);

#ifdef MINGW
char* strtok_r(char *str, const char *delim, char **nextp);
size_t getline(char **lineptr, size_t *n, FILE *stream);
#include <Windows.h>
gchar *path_relative(gchar *path);
#endif


#endif
