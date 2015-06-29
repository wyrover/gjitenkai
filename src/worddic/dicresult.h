#ifndef DICRESULT_H
#define DICRESULT_H

#include "dicentry.h"

/**
   A dicresult represent a match in the dictionary from a search
   as the match is differant from the searched expression, notably when 
   searching with a regex or an inflection. 
 */
typedef struct dicresult_t{
  GjitenDicentry *entry;
  gchar *match;
  gchar *comment;
}dicresult;

dicresult_free(dicresult* p_dicresult);

#endif
