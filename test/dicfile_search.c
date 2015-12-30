//gcc -g dicfile_search.c ../src/worddic/worddic_dicfile.c ../src/worddic/dicentry.c ../src/common/dicutil.c ../src/worddic/gloss.c ../src/worddic/dicresult.c ./print_entry.c $(pkg-config --cflags --libs gtk+-3.0) -lz -I../src/worddic/ -o dicfile_search
#include <gtk/gtk.h>

#include "../src/worddic/worddic_dicfile.h"


#include "print_entry.h"

void results_free(GList *results){
  g_list_free_full(results, (GDestroyNotify)dicresult_free);
  
}

int main( int argc, char **argv )
{
  GList *results = NULL;
  
  g_printf("parameters are: 'dictionary path' and 'search expression'\n\
(parameters are not checked)\n");
  
  //create and open dictionary
  WorddicDicfile *dicfile = g_new0(WorddicDicfile, 1);
  dicfile->path = g_strdup(argv[1]);
  worddic_dicfile_open(dicfile);

  //parse dictionary
  worddic_dicfile_parse_all(dicfile);

  //close dicrionary
  worddic_dicfile_close(dicfile);

  search_expression search_expr;
  search_expr.search_criteria_jp = ANY_MATCH;
  search_expr.search_criteria_lat = ANY_MATCH;
  
  //kana
  //search hiragana on katakana
  if(hasKatakanaString(argv[2])) {
    gchar *hiragana = kata_to_hira(argv[2]);
    search_expr.search_text = hiragana;

    results = g_list_concat(results, dicfile_search(dicfile,
						    &search_expr,
						    "from katakana",
						    GIALL,
						    1)
			    );
    g_free(hiragana);  //free memory
  }
  ///////////////////////////////////
  //search katakana on hiragana
  if (hasHiraganaString(argv[2])) { 
    gchar *katakana = hira_to_kata(argv[2]);
    search_expr.search_text = katakana;
        
    results = g_list_concat(results, dicfile_search(dicfile,
						    &search_expr,
						    "from hiragana",
						    GIALL,
						    1)
			    );
    g_free(katakana); //free memory
  }

  ////////////////////////////////
  
    //standard search
    search_expr.search_text = argv[2];
    results = g_list_concat(results, dicfile_search(dicfile,
                                                    &search_expr,
                                                    NULL,
                                                    GIALL,
                                                    1)
                            );
  //print
  GList *l;
  for(l=results;l!=NULL;l = l->next){
    dicresult *result = l->data;
    
    //print matched part and comment
    g_printf("COMMENT %s\n MATCH %s:\n", result->comment, result->match);
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
