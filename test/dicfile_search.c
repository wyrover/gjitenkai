//gcc -g dicfile_search.c ../src/worddic/worddic_dicfile.c ../src/worddic/dicentry.c ../src/common/dicutil.c ../src/worddic/gloss.c ../src/worddic/dicresult.c ./print_entry.c $(pkg-config --cflags --libs gtk+-3.0) -I../src/worddic/ -o dicfile_search
#include <gtk/gtk.h>

#include "../src/worddic/worddic_dicfile.h"


#include "print_entry.h"

void results_free(GList *results){
  g_list_free_full(results, (GDestroyNotify)dicresult_free);
  
}

int main( int argc, char **argv )
{
  GList *results = NULL;
  
  g_printf("parameters are dictionary path and search expression\n\
parameters are not checked\n");
  
  //create and open dictionary
  WorddicDicfile *dicfile = g_new0(WorddicDicfile, 1);
  dicfile->path = g_strdup(argv[1]);
  worddic_dicfile_open(dicfile);

  //parse dictionary
  worddic_dicfile_parse_all(dicfile);

  //close dicrionary
  worddic_dicfile_close(dicfile);

  //kana
  //search hiragana on katakana
  if(hasKatakanaString(argv[2])) {
    gchar *hiragana = kata_to_hira(argv[2]);
    results = g_list_concat(results, dicfile_search(dicfile,
                                                    hiragana,
                                                    "from katakana",
                                                    GIALL,
                                                    ANY_MATCH,
                                                    ANY_MATCH,
                                                    1)
                            );
    g_free(hiragana);  //free memory
  }
  ///////////////////////////////////

  
  //search kata
  if(hasHiraganaString(argv[2])) { 
    gchar *katakana = hira_to_kata(argv[2]);
    results = g_list_concat(results, dicfile_search(dicfile,
                                                    katakana,
                                                    "from hiragana",
                                                    GIALL,
                                                    ANY_MATCH,
                                                    ANY_MATCH,
                                                    1)
                            );
    g_free(katakana); //free memory
  }
  ////////////////////////////////
  
  //search 
  results = g_list_concat(results, dicfile_search(dicfile,
                                                  argv[2],
                                                  NULL,
                                                  GIALL,
                                                  ANY_MATCH,
                                                  ANY_MATCH,
                                                  -1)
                          );
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
