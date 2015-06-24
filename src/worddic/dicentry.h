#ifndef DICENTRY_H
#define DICENTRY_H

#include <gtk/gtk.h>

/*entry in an EDICT dictonary file

http://www.edrdg.org/jmdict/edict_doc.html

type can be: exp, v1, v3, v5, n, adj-i, adj-na
*/
struct _GjitenDicentry {
  gchar *jap_definition;
  gchar *jap_reading;
  GList *definitions;
};

typedef struct _GjitenDicentry GjitenDicentry;

GjitenDicentry* parse_line(const gchar* line);
void dicentry_free(GjitenDicentry* dicentry);
#endif
