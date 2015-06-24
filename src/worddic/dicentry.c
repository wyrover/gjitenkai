#include "dicentry.h"

GjitenDicentry* parse_line(const gchar* line){

  //copy the line as strtok will alter the line
  gchar *line_cpy = strdup(line);

  //new entry to return
  GjitenDicentry* dicentry = g_new0 (GjitenDicentry, 1);

  //cut until / 
  gchar *chunk = strtok(line_cpy, "/");
  
  //read the definitions in the other chunks
  dicentry->definitions = NULL;
  gchar *definition = strtok(NULL, "/");
  do{
    if(definition && strcmp(definition, "\n")){
      //check if this is the ent sequance or a gloss
      if(g_str_has_prefix(definition, "EntL")){
        dicentry->ent_seq = g_strdup_printf("%s", definition);
      }
      else{
        dicentry->definitions = g_list_append(dicentry->definitions,
                                              g_strdup_printf("%s", definition));
      }
      }
    definition = strtok(NULL, "/");
  }while(definition);

  ////get data from the first chunk
  //japanese definition
  gchar* jap_definition = strtok(chunk, " ");
  dicentry->jap_definition = g_strdup_printf("%s", jap_definition);

  //optional japanese reading
  gchar* reading = strtok(NULL, " ");
  if(reading){
    dicentry->jap_reading = g_strdup_printf("%s", reading);
    //trim the bracets []
    size_t len = strlen(dicentry->jap_reading);
    memmove(dicentry->jap_reading, dicentry->jap_reading+1, len-2);
    dicentry->jap_reading[len-2] = 0;
  }
  else{
    dicentry->jap_reading = NULL;
  }

  g_free(line_cpy);
  
  return dicentry;
}

void dicentry_free(GjitenDicentry* dicentry){
  g_free(dicentry->jap_definition);
  g_free(dicentry->jap_reading);

  GList *d=NULL;
  for(d = dicentry->definitions;
      d != NULL;
      d = d->next){
    g_free(d->data);
    
    }

  g_free(dicentry);
}
