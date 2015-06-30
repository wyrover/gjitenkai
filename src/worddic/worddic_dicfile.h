#ifndef WORDDIC_DICFILE_H
#define WORDDIC_DICFILE_H

#include <gtk/gtk.h>

#include "dicentry.h"
#include "dicresult.h"
#include "gloss.h"

#include "../common/dicfile.h"

#define PAGE_SIZE 256

struct _WorddicDicfile {
  gchar *path;
  const gchar *name;
  GSList *entries;
};

typedef struct _WorddicDicfile WorddicDicfile;

void worddic_dicfile_parse(WorddicDicfile *dicfile);

GList *add_match(GMatchInfo *match_info,
                        gchar *comment,
                        GjitenDicentry* dicentry,
                        GList *results);

GList *dicfile_search(WorddicDicfile *dicfile,
                      const gchar *srchstrg_regex);
#endif
