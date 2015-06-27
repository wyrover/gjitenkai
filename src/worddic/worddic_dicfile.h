#ifndef WORDDIC_DICFILE_H
#define WORDDIC_DICFILE_H

#include <gtk/gtk.h>

#include "dicentry.h"
#include "../common/dicfile.h"

#include "dicresult.h"

#define DEFMAXWORDMATCHES 100

struct _WorddicDicfile {
  gchar *path;
  const gchar *name;
  gchar *mem;
  int file;
  gint status;
  struct stat stat;
  gint size;

  GSList *entries;
  
};

typedef struct _WorddicDicfile WorddicDicfile;

void worddic_dicfile_parse(WorddicDicfile *dicfile);

inline GList *add_match(GMatchInfo *match_info,
                        GjitenDicentry* dicentry,
                        GList *results);

GList *dicfile_search(WorddicDicfile *dicfile,
                      const gchar *srchstrg_regex);
#endif
