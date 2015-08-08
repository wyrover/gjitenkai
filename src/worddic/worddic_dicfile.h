#ifndef WORDDIC_DICFILE_H
#define WORDDIC_DICFILE_H

#include <gtk/gtk.h>
#include <zlib.h>

#include "dicentry.h"
#include "dicresult.h"
#include "gloss.h"

#include "../common/dicfile.h"

#define GZLEN 1600

enum dicfile_search_criteria{
  EXACT_MATCH = 1,   //jp latin (whole expression)
  START_WITH_MATCH,  //jp
  END_WITH_MATCH,    //jp
  ANY_MATCH,         //jp latin (any)
  WORD_MATCH 	     //latin (whole word)
};

typedef struct search_expression_t{
  const gchar *search_text;
  enum  dicfile_search_criteria search_criteria_jp;
  enum  dicfile_search_criteria search_criteria_lat;
}search_expression;

typedef struct _WorddicDicfile {
  gchar *path;
  gchar *name;
  GSList *entries;

  gboolean is_active;
  gboolean is_loaded;

  FILE * fp;
  gboolean utf8;
  gboolean is_gz;
  gchar *informations;
}WorddicDicfile;

void worddic_dicfile_open(WorddicDicfile *dicfile);

void worddic_dicfile_parse_all(WorddicDicfile *dicfile);
gboolean worddic_dicfile_parse_next_line(WorddicDicfile *dicfile);

void worddic_dicfile_close(WorddicDicfile *dicfile);

GList *add_match(GMatchInfo *match_info,
		 gchar *comment,
		 GjitenDicentry* dicentry,
		 GList *results);

/**
   @param dicfile dictionary file to search to
   @param is_jp 
   0 search in glosses
   1 search in japanese definition and reading
   -1 auto detect
 */
GList *dicfile_search(WorddicDicfile *dicfile,
                      search_expression *p_seach_expression,
                      gchar *comment,
                      enum entry_GI itype,
                      gint is_jp);

void worddic_dicfile_free(WorddicDicfile *dicfile);
void worddic_dicfile_free_entries(WorddicDicfile *dicfile);
#endif
