#ifndef WORDDIC_DICFILE_H
#define WORDDIC_DICFILE_H

#include <gtk/gtk.h>

#include "dicentry.h"
#include "dicresult.h"
#include "gloss.h"

#include "../common/dicfile.h"

typedef struct _WorddicDicfile {
  const gchar *path;
  const gchar *name;
  GSList *entries;

  gboolean is_active;
  gboolean is_loaded;

  FILE * fp;
  gboolean utf8;
  gchar *informations;
}WorddicDicfile;

void worddic_dicfile_open(WorddicDicfile *dicfile);

void worddic_dicfile_parse_all(WorddicDicfile *dicfile);
gboolean worddic_dicfile_parse_next_line(WorddicDicfile *dicfile, gchar *line);

void worddic_dicfile_close(WorddicDicfile *dicfile);

GList *add_match(GMatchInfo *match_info,
		 gchar *comment,
		 GjitenDicentry* dicentry,
		 GList *results);

GList *dicfile_search(WorddicDicfile *dicfile,
                      const gchar *srchstrg_regex);

void worddic_dicfile_free(WorddicDicfile *dicfile);
void worddic_dicfile_free_entries(WorddicDicfile *dicfile);
#endif
