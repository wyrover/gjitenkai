#ifndef GLOSS_H
#define GLOSS_H

#include <gtk/gtk.h>

//todo rename to sense
typedef struct gloss_t{
  //todo rename to gloss
  GSList *sub_gloss;    //glosses inside the gloss

  //general informations
  //in plain text (to display)
  GSList *general_informations;

  //in a bits field (to search)
  gint GI:3;

  // ISO 639-2 defaults to eng
  gchar lang[3];  //todo but in sub gloss (renamed to gloss)
}gloss;

void gloss_free(gloss *p_gloss);

#endif
