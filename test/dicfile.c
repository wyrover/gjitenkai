//gcc -g  dicfile.c ../src/worddic/worddic_dicfile.c ../src/worddic/dicentry.c ../src/common/dicutil.c ../src/worddic/gloss.c $(pkg-config --cflags --libs gtk+-3.0) -lz -I../src/worddic/ -o dicfile
#include <gtk/gtk.h>

#include "../src/worddic/worddic_dicfile.h"

int main( int argc, char **argv )
{
  WorddicDicfile *dicfile = g_new0(WorddicDicfile, 1);
  dicfile->path = g_strdup(argv[1]);
  worddic_dicfile_open(dicfile);
  worddic_dicfile_parse_all(dicfile);
  worddic_dicfile_close(dicfile);
  worddic_dicfile_free(dicfile);
  return 1;
}
