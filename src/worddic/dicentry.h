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
  ADJI = 0,
  V1,
  V5
};

struct _GjitenDicentry {
  GSList *jap_definition; //kanji (gchar*)
  GSList *jap_reading;    //kana  (gchar*)
  GSList *gloss;          //gloss (struct gloss)
  gchar *ent_seq;         //EntLnnnnnnnnX (gchar*)

  //general informations
  //in plain text (to display)
  GSList *general_informations;
  //in a bits field (to search)
  gint GI:3;

  gboolean priority;
};

typedef struct _GjitenDicentry GjitenDicentry;

GjitenDicentry* parse_line(gchar* line);
void dicentry_free(GjitenDicentry* dicentry);

#endif
