#ifndef KANJIDIC_H
#define KANJIDIC_H

#include <gtk/gtk.h>

#include "../common/conf.h"

#include "constants.h"
#include "kanjiutils.h"

#define UI_DEFINITIONS_FILE_KANJIDIC "UI/kanjidic.glade"

typedef struct kanjidic_t
{
  GtkBuilder *definitions;

  //TODO create a struct KanjidicConfig
  GjitenConfig *conf;

  //with these two hash, search can be performed on kanji and radicals
  GHashTable *kanji_info_hash;  //kanji   -> list of kanjiinfo
  GHashTable *rad_info_hash;    //radical ->  list of radicalinfo
  GList *rad_info_list;         //list of radicalinfo

  //search options
  gboolean filter_by_stroke;
  gboolean filter_by_radical;
  gboolean filter_by_key;

  //kanji tag style in the kanji display
  GtkTextTag *texttag_kanji; 

} kanjidic;


extern void on_button_kanji_clicked(GtkButton *button, kanjidic *kanjidic);

void kanjidic_init (kanjidic * );

/**
   Set the sensitivity of the stroke filter widgets
 */
void set_ui_stroke_filter_sensitivity(gboolean sensitivity, kanjidic *kanjidic);

/**
   Set the sensitivity of the radical filter widgets
 */
void set_ui_radical_filter_sensitivity(gboolean sensitivity, kanjidic *kanjidic);

/**
   Set the sensitivity of the key filter widgets
 */
void set_ui_key_filter_sensitivity(gboolean sensitivity, kanjidic *kanjidic);

/**
   Search kanji with the search filter values and display a list of buttons in 
   the search result area
 */
void search_and_display_kanji(kanjidic *kanjidic);


#endif
