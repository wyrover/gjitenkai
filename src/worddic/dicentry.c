#include "dicentry.h"

GjitenDicentry* parse_line(const gchar* line){
      
  //copy the line as strtok will alter the line
  gchar *line_cpy = strdup(line);

  //new entry to return
  GjitenDicentry* dicentry = g_new0 (GjitenDicentry, 1);
  dicentry->jap_reading = NULL;
  dicentry->jap_definition = NULL;
  dicentry->gloss = NULL;

  //cut until the first '/', separating definiton,reading in the first chunk and
  //gloss in the second chunk
  gchar *chunk = strtok(line_cpy, "/");

  ////////
  //read gloss in the second  chunk
  gchar *gloss = strtok(NULL, "/");
  do{
    if(gloss && strcmp(gloss, "\n")){
      //check if this is the ent sequance or a gloss
      if(g_str_has_prefix(gloss, "EntL")){
        dicentry->ent_seq = g_strdup_printf("%s", gloss);
      }
      else{
        dicentry->gloss = g_slist_append(dicentry->gloss,
                                         g_strdup_printf("%s", gloss));
      }
    }
    gloss = strtok(NULL, "/");
  }while(gloss);
  
  ////////
  //read definitions in the first chunk
  gchar* jap_definitions = strtok(chunk, " ");

  ////////
  //read the reading in the first chunk
  gchar* jap_readings = strtok(NULL, " ");  

  
  //cut jap definitions and jap readings into a list
  //japanese definition
  gchar *jap_definition = strtok(jap_definitions, ";");
  do{
    if(jap_definition && strcmp(jap_definition, "\n")){
      dicentry->jap_definition = g_slist_append(dicentry->jap_definition,
                                                g_strdup_printf("%s", jap_definition));
    }
    jap_definition = strtok(NULL, ";");
  }while(jap_definition);
  
  //optional japanese reading        
  if(jap_readings){
    //trim the bracets []
    size_t len = strlen(jap_readings);
    memmove(jap_readings, jap_readings+1, len-2);
    jap_readings[len-2] = 0;  

    gchar *jap_reading = strtok(jap_readings, ";");
    do{
      if(jap_reading && strcmp(jap_reading, "\n")){
        dicentry->jap_reading = g_slist_append(dicentry->jap_reading,
                                               g_strdup_printf("%s", jap_reading));        
      }
      jap_reading = strtok(NULL, ";");
    }while(jap_reading);
    
  }
    
  g_free(line_cpy);
  
  return dicentry;
}

void dicentry_free(GjitenDicentry* dicentry){
  g_free(dicentry->jap_definition);
  g_free(dicentry->jap_reading);

  GList *d=NULL;
  for(d = dicentry->gloss;
      d != NULL;
      d = d->next){
    g_free(d->data);
    
  }

  g_free(dicentry);
}
