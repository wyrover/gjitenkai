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

inline GList *add_match(GMatchInfo *match_info,
		 gchar *comment,
		 GjitenDicentry* dicentry,
		 GList *results){
  //fetch the matched string
  gchar *word = g_match_info_fetch (match_info, 0);

  //create a new dicresult struct with the entry and the match
  dicresult *p_dicresult = g_new0(dicresult, 1);
  p_dicresult->match = word;
  p_dicresult->entry = dicentry;
  p_dicresult->comment = comment;
  
  //add the dicentry in the result list
  results = g_list_prepend(results, p_dicresult);

  return results;
}

GList *dicfile_search(WorddicDicfile *dicfile,
                      const gchar *srchstrg_regex);
#endif
