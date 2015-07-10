#ifndef GLOSS_H
#define GLOSS_H

#include <gtk/gtk.h>

typedef struct gloss_t{
GSList *sub_gloss;  //glosses inside the gloss

//general informations
//in plain text (to display)
GSList *general_informations;
//in a bits field (to search)
gint GI:3;

}gloss;

void gloss_free(gloss *gloss);

#endif
