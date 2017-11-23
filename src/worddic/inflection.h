#ifndef INFLECTION_H
#define INFLECTION_H

#include <glib.h>
#include <glib/gi18n.h>
#include <stdlib.h>
#include <gdk/gdkkeysyms.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "worddic_dicfile.h"
#include "dicentry.h"

#include "../common/constants.h"
#include "../common/dicfile.h"
#include "../common/dicutil.h"

#define VINFL_FILENAME "vconj.utf8"

/**
use a text file (vconj.utf8) which maps the dictionary form of a verb with
diffent forms (which is called inflection).
*/

//map the vconj types
enum conj_type{
  PLAIN_NEGATIVE_NONPAST = 0,
  POLITE_NONPAST,
  CONDITIONAL,
  VOLITIONAL,
  TEFORM,
  PLAIN_PAST,
  PLAIN_NEGATIVE_PAST,
  PASSIVE,
  CAUSATIVE,
  POTENTIAL_OR_IMPERATIVE,
  IMPERATIVE,
  POLITE_PAST,
  POLITE_NEGATIVE_NONPAST,
  POLITE_NEGATIVE_PAST,
  POLITE_VOLITIONAL_UNUSED,
  ADJ_TO_ADVERB,
  ADJ_PAST,
  POLITE,
  POLITE_VOLITIONAL,
  PASSIVE_OR_POTENTIAL,
  PASSIVE_OR_POTENTIAL_GRP_2,
  ADJ_NEGATIVE,
  ADJ_NEGATIVE_PAST,
  ADJ_PAST_KATTA,
  PLAIN_VERB,
  POLITE_TEFORM
};

#define VCONJ_TYPE_MAX 26

struct vinfl_struct {
  gchar *conj;
  gchar *infl;
  gchar *type;
  enum conj_type itype;
};

//globals
GSList *vinfl_list;
gchar *vinfl_start;
gchar *vconj_types[VCONJ_TYPE_MAX];


void init_inflection();
GList* search_inflections(WorddicDicfile *dicfile,
                          const gchar *srchstrg);
void free_inflection();
void free_vinfl(struct vinfl_struct *vinfl);

#endif
