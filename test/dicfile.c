//gcc -g  dicfile_jmdict.c print_entry.c ../src/worddic/worddic_dicfile.c ../src/worddic/dicentry.c ../src/common/dicutil.c ../src/worddic/gloss.c $(pkg-config --cflags --libs gtk+-3.0 libxml-2.0) -lz -I../src/worddic/ -o dicfile

#include <gtk/gtk.h>

#include "print_entry.h"
#include "../src/worddic/worddic_dicfile.h"
#include "../src/worddic/dicentry.h"

int main( int argc, char **argv )
{
  g_printf("load an edict dictionary.\n parameters are:\n\
'Dicionary path'\n\
'whatever second argument to print all entries'\n");

  char *path = argv[1];
  int print_all = FALSE;
  if(argc > 2)print_all = TRUE;

  WorddicDicfile *dicfile = g_new0(WorddicDicfile, 1);
  dicfile->path = g_strdup(path);
  worddic_dicfile_open(dicfile);
  worddic_dicfile_parse_all(dicfile);

  if(print_all){
    //print all entries
    GSList* list_dicentry = NULL;
    for(list_dicentry = dicfile->entries;
	list_dicentry != NULL;
	list_dicentry = list_dicentry->next){
      print_entry(list_dicentry->data);
    }
  }

  worddic_dicfile_close(dicfile);
  worddic_dicfile_free(dicfile);
  return 1;
}
