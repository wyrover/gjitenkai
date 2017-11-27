#ifndef DICENTRY_H
#define DICENTRY_H

#include <glib.h>

#include "sense.h"

/**
entry in an EDICT dictonary file
http://www.edrdg.org/jmdict/edict_doc.html
*/

enum entry_GI{
  ADJI   = 1 << 0,
  NOUN   = 1 << 1,
  V1     = 1 << 2,
  V5     = 1 << 3,
  GIALL  =  0b1111
};

typedef struct _GjitenDicentry {
  GSList *jap_definition; //kanji (gchar*)
  GSList *jap_reading;    //kana  (gchar*)
  GSList *sense;          //sense (struct sense)
  gchar *ent_seq;         //EntLnnnnnnnnX (gchar*)

  GSList *general_informations;  //General Info in plain text (to display)
  enum entry_GI GI;    //Generak Info in a bits field (to speed up search)

  gboolean priority;
}GjitenDicentry;

void dicentry_free(GjitenDicentry* dicentry);

#endif
