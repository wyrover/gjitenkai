//gcc -g dicfile_search_inflection.c ../src/worddic/worddic_dicfile.c ../src/worddic/dicentry.c ../src/common/dicutil.c ../src/worddic/gloss.c ../src/worddic/dicresult.c ../src/worddic/inflection.c ./print_entry.c $(pkg-config --cflags --libs gtk+-3.0) -I../src/worddic/ -o dicfile_search_inflection
#include <gtk/gtk.h>

#include "../src/worddic/worddic_dicfile.h"


#include "print_entry.h"

void results_free(GList *results){
  g_list_free_full(results, (GDestroyNotify)dicresult_free);
  
}

int main( int argc, char **argv )
{
  if(!g_strcmp0(argv[1], "ぐ")){
    g_printf("GU!\n");
    return 1;
  }
    
  //create and open dictionary
  WorddicDicfile *dicfile = g_new0(WorddicDicfile, 1);
  dicfile->path = g_strdup(argv[1]);
  worddic_dicfile_open(dicfile);

  //parse dictionary
  worddic_dicfile_parse_all(dicfile);

  //close dicrionary
  worddic_dicfile_close(dicfile);

  //init inflection engine
  init_inflection();
  
  //search for infections 
  GList *results = search_inflections(dicfile, argv[2]);
  
  //print
  GList *l;
  for(l=results;l!=NULL;l = l->next){
    dicresult *result = l->data;
    
    //print matched part and comment
    g_printf("(%s) %s:\n", result->comment, result->match);
    //print the entry
    print_entry(result->entry);
  }
  
  //free dicresult
  //when freeing a dicreslt, do not free the dicentry as it will be used again
  //at the next search, just free the comment and the match
  results_free(results);
  results = NULL;
  
  //free dictionary
  worddic_dicfile_free(dicfile);
  
  return 1;
}
