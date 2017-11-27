#ifndef DICENTRY_H
#define DICENTRY_H

#include <glib.h>

#include "sense.h"

/**
entry in an EDICT dictonary file
http://www.edrdg.org/jmdict/edict_doc.html
*/


/**
   General Info flags.
   Used internally to speed-up de-inflection search
 */
enum entry_GI{
  ADJI    = 1 << 0,
  NOUN    = 1 << 1,
  V1      = 1 << 2,
  V5      = 1 << 3,
  GIALL   =  0b1111,
  GINONE  =  0b0000

};

typedef struct _GjitenDicentry {
  GSList *jap_definition; //kanji (gchar*)
  GSList *jap_reading;    //kana  (gchar*)
  GSList *sense;          //sense (struct sense)
  gchar *ent_seq;         //EntLnnnnnnnnX (gchar*)


  //TODO Move to sense
  //GSList *general_informations;  //General Info in plain text (to display)
  //enum entry_GI GI;    //Generak Info in a bits field (to speed up search)

  gboolean priority;
}GjitenDicentry;


/**
   Set a dicentry General Info  bit field from it's General Info text code
   for exemple "adj-i" will set the flag ADJI

   In edict General Information text code are characters.
   In JMdict General Info text code are XML entities
 */
void dicentry_set_GI_flags_from_code(GjitenDicentry* dicentry);

void dicentry_free(GjitenDicentry* dicentry);

#endif
