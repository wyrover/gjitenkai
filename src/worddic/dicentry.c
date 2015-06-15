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
    
  /*
  /////////////////////////////
  //types
  gchar *type=NULL;
  while(type = (gchar*)strtok(NULL, " ")){
    if(type[0] != '('){
      break;
    }

    g_printf("type: %s\n", type);
  }
  */
  
  /////////////////////////////
  //translated definitions
  gchar *definition=NULL;
  dicentry->definitions = NULL;
  while(definition = strtok(NULL, "/")){
    //g_printf("definition: %s\n", definition);
    dicentry->definitions = g_list_append(dicentry->definitions,
                                          g_strdup_printf("%s", definition));
  }
  
  g_free(line_cpy);
  
  return dicentry;
}
