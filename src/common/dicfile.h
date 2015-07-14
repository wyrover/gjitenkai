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

#define EXACT_MATCH 1 	    //jp latin (whole expression)
#define START_WITH_MATCH 2  //jp
#define END_WITH_MATCH 3    //jp
#define ANY_MATCH 4 	    //jp latin (any)
#define WORD_MATCH 5 	    //latin (whole word)

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

int dicfile_load(GjitenDicfile* dicfile, GjitenDicfile *mmaped_dicfile);
void dicutil_unload_dic(GjitenDicfile *dicfile);

gboolean dicfile_init(GjitenDicfile *dicfile);
void dicfile_close(GjitenDicfile *dicfile);
gint search_string(gint srchtype, GjitenDicfile *dicfile, gunichar *srchstrg,
                     guint32 *res_index, gint *hit_pos, gint *res_len, gchar *res_str);

#endif
