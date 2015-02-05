#ifndef KANJIUTILS_H
#define KANJIUTILS_H

#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "constants.h"
#include "kanjifile.h"
#include "../common/dicutil.h"
#include "../common/dicfile.h"

#define KBUFSIZE 500

/**
   Info on a radical
   *The radical
   *Number of stokes
   *List of kanji with this radical
 */
typedef struct _RadInfo {
	gunichar radical;
	gint strokes;
	GList *kanji_info_list;
} RadInfo;

/**
   Info on a kanji
   *The kanji
   *List of radicals of this kanji
 */
typedef struct _KanjiInfo {
	gunichar kanji;
	GList *rad_info_list;
} KanjiInfo;

/**
   load the radicals kanji informations from the radkfile
   The radkfile map radicals with a stroke count and a list of kanji.
   The content of this file is loaded into two hashs: 
   *rad_info_hash : map a radical as a key and a list of KanjiInfo as value
   *kanji_info_hash : map a kanji as a key and a list of RadInfo as value

   With these two hashs, search can be performed on to search a kanji from a radical
   or to get all radical from a kanji.
 */
GList* load_radkfile(GHashTable **pp_rad_info_hash, 
                   GHashTable **pp_kanji_info_hash,
                   GList      *rad_info_list
                   );

/**
   get the radical from a given kanji by looking into the kanji_info_hash 
 */
GList* get_radical_of_kanji(gunichar kanji, GHashTable *kanji_info_hash);

/**
   Search in the dictionary the kanji that matches a key
   (see keys from kanjidic file)
 */
GList* get_kanji_by_key(gchar *srchkey, GList *list, GjitenDicfile *dicfile);

/**
   Search the kanji with a certain amount of strokes +/- plusmin
   in the dicfile and put the results in the list
   This function search kanji by key, with the S key.
 */
GList* get_kanji_by_stroke(int stroke, int plusmin, GList *list, GjitenDicfile *dicfile);

/**
   get kanji from radicals by looking into the rad_info_hash 
 */
GList* get_kanji_by_radical(gchar *radstrg, GHashTable *rad_info_hash);

/**
   Merge two list of kanji. 
   Returns a list that matchs list_a and list_b
 */
GList* list_merge(GList *list_a, GList *list_b);
GList* list_merge_str(GList *list_a, GList *list_b);

#endif
