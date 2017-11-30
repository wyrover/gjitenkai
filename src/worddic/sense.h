#ifndef SENSE_H
#define SENSE_H

#include <glib.h>

//TODO rename to sense GI
enum entry_GI{
  ADJI    = 1 << 0,
  NOUN    = 1 << 1,
  V1      = 1 << 2,
  V5      = 1 << 3,
  GIALL   =  0b1111,
  GINONE  =  0b0000
};

typedef struct _lang{
  gchar code[3];
  gchar *name;
  gboolean active;
}lang;

//TODO rename to gloss
typedef struct gloss_t{
  gchar lang[3];   // ISO 639-2
  gchar *content;
}gloss;

typedef struct sense_t{
  GSList *gloss;             //sub sense
  GSList *general_informations;  //General Infor in plain text (to display)
  gint GI:3;                     //in a bits field (to search)
}sense;

/**
   Set a sense General Info  bit field from it's General Info text code
   for exemple "adj-i" will set the flag ADJI
   In edict General Information text code are characters.
   In JMdict General Info text code are XML entities
*/
void sense_set_GI_flags_from_code(sense *p_sense);


void sense_free(sense *p_sense);

#endif
