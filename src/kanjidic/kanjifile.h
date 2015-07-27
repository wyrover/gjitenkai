/**
kanjifile contains functions to read and organize the content of a kanjidic file
(for more information on kanjidic file see http://www.csse.monash.edu.au/~jwb/kanjidic.html)

Each line beggin by a kanji and is followed by several information 
 the kanji itself;
the JIS code of the kanji in hexadecimal;
[U] the Unicode/ISO 10646 code of the kanji in hexadecimal;
[N] the index in Nelson (Modern Reader's Japanese-English Character Dictionary)
[B] the classification radical number of the kanji (as in Nelson);
[C] the "classical" radical number (where this differs from the one used in Nelson);
[S] the total stroke-count of the kanji;
[G] the "grade" of the kanji, In this case, G2 means it is a Jouyou (general use) kanji taught in the second year of elementary schooling in Japan;
[H] the index in Halpern (New Japanese-English Character Dictionary);
[F] the rank-order frequency of occurrence of the kanji in Japanese;
[P] the "SKIP" coding of the kanji, as used in Halpern;
[K] the index in the Gakken Kanji Dictionary (A New Dictionary of Kanji Usage);
[L] the index in Heisig (Remembering The Kanji);
[I] the index in the Spahn & Hadamitsky dictionary.
[Q] the Four-Corner code;
[MN,MP] the index and page number in the 13-volume Morohashi "DaiKanWaJiten";
[E] the index in Henshall (A Guide To Remembering Japanese Characters);
[Y] the PinYin (Chinese) pronunciation(s) of the kanji;
the Japanese pronunciations or "readings" of the kanji. These are in the katakana script for "ON" (i.e of Chinese origin) readings, and hiragana for "KUN" (Japanese origin) readings. 
 */

#ifndef KANJI_FILE
#define KANJI_FILE

#include <gtk/gtk.h>

#include "../common/dicfile.h"

#define KBUFSIZE 500

typedef struct kanjifile_entry_t{
  gunichar *kanji;
  gint stroke;
  gint unicode;
  gint jouyou;
  GSList *translations;
  GSList *kunyomi;
  GSList *onyomi;
}kanjifile_entry;


/**
   Search in the kanjidic the line corresponding of the given kanji
 */
gchar* get_line_from_dic(const gchar *kanji, GjitenDicfile *kanjidic);

/**
   Parse a line of the kdic
   @param line of the kdic
 */
kanjifile_entry* kanjidic_dicfile_parse_line(const gchar *kstr);

void kanjifile_entry_free(kanjifile_entry* p_entry);

#endif
