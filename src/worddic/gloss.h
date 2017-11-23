#ifndef GLOSS_H
#define GLOSS_H

#include <gtk/gtk.h>

//todo rename to gloss
typedef struct sub_gloss_t{
  gchar lang[3];   // ISO 639-2 defaults to eng
  gchar *content;
}sub_gloss;

//todo rename to sense
typedef struct gloss_t{
  GSList *sub_gloss;             //sub gloss
  GSList *general_informations;  //General Infor in plain text (to display)
  gint GI:3;                     //in a bits field (to search)
}gloss;

void gloss_free(gloss *p_gloss);

#endif
