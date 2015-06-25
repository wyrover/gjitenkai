#ifndef DICENTRY_H
#define DICENTRY_H

#include <gtk/gtk.h>

/*entry in an EDICT dictonary file

http://www.edrdg.org/jmdict/edict_doc.html

TODO general information field (now inside gloss)
*/
struct _GjitenDicentry {
  GSList *jap_definition; //kanji
  GSList *jap_reading;    //kana
  GSList *gloss;          //gloss
  gchar *ent_seq;         //EntLnnnnnnnnX

  //general informations
  gint TYPE;    //n, v1, v5, vi, vt, adj-i, adj-na, adj-to
  gint MOD;     //arch, sl
  gint FIELD;   //comp, astro, etc
};

typedef struct _GjitenDicentry GjitenDicentry;

GjitenDicentry* parse_line(const gchar* line);
void dicentry_free(GjitenDicentry* dicentry);
#endif
