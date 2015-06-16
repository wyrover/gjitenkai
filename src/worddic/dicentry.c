#include "dicentry.h"

GjitenDicentry* parse_line(gchar* line){
  gchar *line_cpy = strdup(line);
  GjitenDicentry* dicentry = g_new0 (GjitenDicentry, 1);
  //g_printf("%s\n", line);
  //cut until / 
  gchar *chunk = strtok(line_cpy, "/");
  
  //read the definitions
  dicentry->definitions = NULL;
  gchar *definition = strtok(NULL, "/");
  do{
    //g_printf("def %s\n", definition);
    if(definition && strcmp(definition, "\n")){
      dicentry->definitions = g_list_append(dicentry->definitions,
                                            g_strdup_printf("%s", definition));
      }
    definition = strtok(NULL, "/");
  }while(definition);
  
  dicentry->jap_definition = g_strdup_printf("%s", strtok(chunk, " "));
  dicentry->jap_reading = g_strdup_printf("%s", strtok(NULL, " "));
  
  return dicentry;
}

