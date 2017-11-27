#ifndef WORDDIC_DICFILE_H
#define WORDDIC_DICFILE_H

#include <glib.h>
#include <zlib.h>
#include <gio/gio.h>
#include <stdio.h>

#include <libxml/tree.h>
#include <libxml/parser.h>

#include "sense.h"
#include "dicresult.h"
#include "dicentry.h"
#include "parser/edict.h"
#include "parser/jmdict.h"
#include "../gjitenkai/dicfile.h"

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
  //path of the dictionary on the coputer
  gchar *path;

  //user defined name
  gchar *name;

  //list of dicentry (on entry per line)
  GSList *entries;

  //search will be performed only on active dictionaries
  gboolean is_active;

  //are the entries loaded ?
  gboolean is_loaded;

  //set to false if magic sequance is not found at the beginning of the file
  gboolean is_valid;

  //file pointer to read the edict dictionary file
  FILE *fp;

  //tells if the dictionary file is utf8 encoded
  gboolean utf8;

  //tells if the dictionary file is gzipped
  gboolean is_gz;

  //tells if the dictionarry file is jmdict or edict
  gboolean is_jmdict;

  //first line of the edict dictionary with:
  //magic number / Dictionary type / Copyright / Creation date
  gchar *informations;
  gchar *type;
  gchar *copyright;
  gchar *creation_date;
}WorddicDicfile;

gboolean worddic_dicfile_open_edict(WorddicDicfile *dicfile);
gboolean worddic_dicfile_open(WorddicDicfile *dicfile);

void worddic_dicfile_parse_all(WorddicDicfile *dicfile);
gboolean worddic_dicfile_parse_next_line(WorddicDicfile *dicfile);

void worddic_dicfile_close(WorddicDicfile *dicfile);

void worddic_dicfile_open_parse_all_close(WorddicDicfile *dicfile);

/**
   Create a dicresult from a GMatchInfo, a comment and an entry and
   prepend it to the list of results
   Prepend is used as it is much faster than append which seeks the end of the
   list at each call.

   @Return the result list.
*/
static inline GList *add_match(GMatchInfo *match_info,
			       gchar *comment,
			       GjitenDicentry* dicentry,
			       GList *results){
  //fetch the matched string
  gchar *word = g_match_info_fetch (match_info, 0);

  //create a new dicresult struct with the entry and the match
  //when freeing the result, do not free the entry
  dicresult *p_dicresult = g_new0(dicresult, 1);
  p_dicresult->match = word;
  p_dicresult->entry = dicentry;
  if(comment)p_dicresult->comment = strdup(comment);
  else p_dicresult->comment = NULL;

  //add the dicentry in the result list
  results = g_list_prepend(results, p_dicresult);

  return results;
}

void dicfile_parse_jmdict(WorddicDicfile *dicfile);

/**
   @param dicfile dictionary file to search to
   @param is_jp
   0 search in sensees
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
