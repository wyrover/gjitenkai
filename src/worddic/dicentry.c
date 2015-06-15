#include "dicentry.h"

GjitenDicentry* parse_line(gchar* line){
  gchar *line_cpy = strdup(line);
  GjitenDicentry* dicentry = g_new0 (GjitenDicentry, 1);

  //g_printf("%s\n", line);
  
  //////////////////////////////
  //get the japanese definition 
  dicentry->jap_definition = g_strdup_printf("%s", strtok(line_cpy, " "));

  /////////////////////////////
  //get the japanese reading
  gchar *reading = strtok(NULL, " ");
  dicentry->jap_reading = g_strdup_printf("%s", reading);
 
  //trim the bracets []
  size_t len = strlen(dicentry->jap_reading);
  memmove(dicentry->jap_reading, dicentry->jap_reading+1, len-2);
  dicentry->jap_reading[len-2] = 0;
  
  /////////////////////////////
  //translated definitions
  gchar *definition=NULL;
  dicentry->definitions = NULL;
  while(definition = strtok(NULL, "/")){
    if(strcmp(definition, "\n")){
      dicentry->definitions = g_list_prepend(dicentry->definitions,
                                             definition);
    }
  }
  
  dicentry->definitions = g_list_reverse(dicentry->definitions);
  
  //g_free(line_cpy);
  
  return dicentry;
}

