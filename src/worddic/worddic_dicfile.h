#ifndef WORDDIC_DICFILE_H
#define WORDDIC_DICFILE_H

#include <gtk/gtk.h>

#include "dicentry.h"
#include "../common/dicfile.h"

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

GList *worddic_dicfile_parse(WorddicDicfile *dicfile);

GList *dicfile_search_regex(WorddicDicfile *dicfile,
			    const gchar *srchstrg_regex,
			    GList **matched_part);
#endif
