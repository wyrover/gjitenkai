
#ifndef DICFILE_H
#define DICFILE_H

#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
//#include <sys/mman.h>
#include <stdio.h>
#include <stdio.h>
#include <glib/gi18n.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "conf.h"
#include "error.h"

#define DEFMAXWORDMATCHES 500
#define MAXDICFILES 100

#define EXACT_MATCH 1 		//jp latin
#define START_WITH_MATCH 2 	//jp
#define END_WITH_MATCH 3 	//jp
#define ANY_MATCH 4 		//jp latin
#define WORD_MATCH 5 		//latin
#define REGEX 6                 //jp latin

#define SRCH_OK		0
#define SRCH_FAIL	1
#define SRCH_START	2
#define SRCH_CONT	3

struct _GjitenDicfile {
  gchar *path;
  const gchar *name;
  gchar *mem;
  int file;
  gint status;
  struct stat stat;
  gint size;
};

typedef struct _GjitenDicfile GjitenDicfile;

enum {
  DICFILE_NOT_INITIALIZED,
  DICFILE_BAD,
  DICFILE_OK
  };

int dicfile_load(GjitenDicfile* dicfile);
gboolean dicfile_is_utf8(GjitenDicfile *dicfile);
gboolean dicfile_init(GjitenDicfile *dicfile);
void dicfile_close(GjitenDicfile *dicfile);
void dicfile_list_free(GSList *dicfile_list);
gboolean dicfile_check_all(GSList *dicfile_list);

GList *dicfile_search_regex(GjitenDicfile *dicfile,
			    const gchar *srchstrg_regex,
			    GList **matched_part);

GList *dicfile_search(GjitenDicfile *dicfile, const gchar *srchstrg, 
                        gint match_criteria_jp, gint match_criteria_lat, 
                        gint match_type);

gint search_string(gint srchtype, GjitenDicfile *dicfile, const gchar *srchstrg,
                     guint32 *res_index, gint *hit_pos, gint *res_len, gchar *res_str);

#endif
