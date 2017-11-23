#ifndef SENSE_H
#define SENSE_H

#include <glib.h>

//todo rename to sense
typedef struct sub_sense_t{
  gchar lang[3];   // ISO 639-2 defaults to eng
  gchar *content;
}sub_sense;

//todo rename to sense
typedef struct sense_t{
  GSList *sub_sense;             //sub sense
  GSList *general_informations;  //General Infor in plain text (to display)
  gint GI:3;                     //in a bits field (to search)
}sense;

void sense_free(sense *p_sense);

#endif
