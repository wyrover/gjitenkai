#ifndef DICENTRY_H
#define DICENTRY_H

#include <gtk/gtk.h>

#ifndef MINGW
#include <string.h>
#else
#include "../common/dicutil.h"
#endif

#include "gloss.h"

/**
entry in an EDICT dictonary file
http://www.edrdg.org/jmdict/edict_doc.html
*/

enum entry_GI{
  NOUN   = 1 << 0,
  ADJI   = 1 << 1,
  V1     = 1 << 2,
  V5R    = 1 << 3,
  V5M    = 1 << 4,
  V5K    = 1 << 5,
  V5U    = 1 << 6,
  V5S    = 1 << 7,
  V5G    = 1 << 8,
  V5N    = 1 << 9,
  V5T    = 1 << 10,
  V5ARU  = 1 << 11,
  GIALL  =  111111111111
};

#define SET_VERBE_5 V5R | V5M | V5K | V5U | V5S | V5G | V5N | V5T | V5ARU
#define SET_VERBE V1 | SET_VERBE_5

struct _GjitenDicentry {
  GSList *jap_definition; //kanji (gchar*)
  GSList *jap_reading;    //kana  (gchar*)
  GSList *gloss;          //gloss (struct gloss)
  gchar *ent_seq;         //EntLnnnnnnnnX (gchar*)
  
  //general informations
  //in plain text (to display)
  GSList *general_informations;
  //in a bits field (to search)
  enum entry_GI GI;

  gboolean priority;
};

typedef struct _GjitenDicentry GjitenDicentry;

GjitenDicentry* parse_line(const gchar* p_line);
void dicentry_free(GjitenDicentry* dicentry);

#endif
