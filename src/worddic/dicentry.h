#ifndef DICENTRY_H
#define DICENTRY_H

#include <glib.h>

#include <libxml/tree.h>
#include <libxml/parser.h>

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
  ADJI   = 1 << 0,
  NOUN   = 1 << 1,
  V1     = 1 << 2,
  V5     = 1 << 3,
  GIALL  =  0b1111
};

struct _GjitenDicentry {
  GSList *jap_definition; //kanji (gchar*)
  GSList *jap_reading;    //kana  (gchar*)
  GSList *gloss;          //gloss (struct gloss)
  gchar *ent_seq;         //EntLnnnnnnnnX (gchar*)

  GSList *general_informations;  //General Info in plain text (to display)
  enum entry_GI GI;    //Generak Info in a bits field (to speed up search)

  gboolean priority;
};

typedef struct _GjitenDicentry GjitenDicentry;

GjitenDicentry* parse_line(const gchar* p_line);

/**
   return a GjitenDicEntry from an entry in a JMdict
   cur must point to an entry
*/
GjitenDicentry* parse_entry_jmdict(xmlNodePtr cur);

void dicentry_free(GjitenDicentry* dicentry);

#endif
