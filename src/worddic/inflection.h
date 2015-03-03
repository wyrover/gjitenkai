/**

use a text file (vconj.utf8) which maps the dictionary form of a verb with 
diffent forms (which is called inflection). 

*/

#ifndef INFLECTION_H
#define INFLECTION_H

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <stdlib.h>
#include <gdk/gdkkeysyms.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "../common/constants.h"
#include "../common/dicfile.h"
#include "../common/dicutil.h"

/* verb deinflection */
struct vinfl_struct {
  gchar *conj;
  gchar *infl;
  gchar *type;
};

gchar *vconj_types[40];
GSList *vinfl_list;
int word_matches;
guint32 srchpos;

void Verbinit();

GList* search_verb_inflections(GjitenDicfile *dicfile, const gchar *srchstrg, GList **match);

#endif
