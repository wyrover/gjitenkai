#ifndef DICENTRY_H
#define DICENTRY_H

#include <glib.h>

//entry in a dictonary file
//Jap definition SPACE [Jap reading] SPACE (type1,type2,..) def1/def2/...
//type can be: exp, v1, v3, v5, n, adj-i, adj-na
struct _GjitenDicentry {
  gchar *jap_definition;
  gchar *jap_reading;
  
  gchar **type;
  gchar **definition;

  gint number_of_type;
  gint number_of_definition;

};

typedef struct _GjitenDicentry GjitenDicentry;

GjitenDicentry* parse_line(gchar* line);

#endif
