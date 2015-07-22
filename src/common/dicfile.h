#ifndef DICFILE_H
#define DICFILE_H

#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdio.h>
#include <glib/gi18n.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "conf.h"
#include "error.h"

enum dicfile_search_criteria{
  EXACT_MATCH = 1,   //jp latin (whole expression)
  START_WITH_MATCH,  //jp
  END_WITH_MATCH,    //jp
  ANY_MATCH,         //jp latin (any)
  WORD_MATCH 	     //latin (whole word)
};

enum dicfie_search_result{
  SRCH_OK = 0,
  SRCH_FAIL,
  SRCH_START,
  SRCH_CONT
};

enum dicfile_status{
  DICFILE_NOT_INITIALIZED = 0,
  DICFILE_BAD,
  DICFILE_OK
};

struct _GjitenDicfile {
  gchar *path;
  const gchar *name;
  gchar *mem;
  int file;
  enum dicfile_status status;
  struct stat stat;
  gint size;
};

typedef struct _GjitenDicfile GjitenDicfile;

gboolean dicfile_load(GjitenDicfile* dicfile, GjitenDicfile *mmaped_dicfile);
void dicutil_unload_dic(GjitenDicfile *dicfile);

gboolean dicfile_init(GjitenDicfile *dicfile);
void dicfile_close(GjitenDicfile *dicfile);
gint search_string(gint srchtype, GjitenDicfile *dicfile, const gchar *srchstrg,
                     guint32 *res_index, gint *hit_pos, gint *res_len, gchar *res_str);

#endif
