#include "dicentry.h"

GjitenDicentry* parse_line(const gchar* p_line){
  
  gchar *line = g_strdup(p_line);
  //new entry to return
  GjitenDicentry* dicentry = g_new0 (GjitenDicentry, 1);
  //dicentry->priority = FALSE;
  
  //cut until the first '/', separating definiton,reading in the first chunk and
  //glosses in the second chunk
  gchar * saveptr_chunk=NULL;
  gchar *chunk = (gchar*)strtok_r(line, "/", &saveptr_chunk);
  
  ////////
  //read glosses (sub gloss) in the second chunk (one sub gloss per /)
  gchar *sub_gloss = (gchar*)strtok_r(NULL, "/", &saveptr_chunk);

  //is it the first parenthese (among all of the glosses)
  gboolean first_parentheses = TRUE;

  //if the last iteration was a GI (detect begining of new gloss/new sub gloss)
  gboolean start_new_gloss = TRUE;
  
  //pointer to hold a gloss before it's added to the entry
  gloss *p_gloss = NULL;
    
  do{
    
    if(sub_gloss && strcmp(sub_gloss, "\n") && strcmp(sub_gloss, " ")){
      //check if this is an edict2 EntL sequance or a sub_gloss
      if(g_str_has_prefix(sub_gloss, "EntL")){
        dicentry->ent_seq = g_strdup(sub_gloss);
      }
      else{
        char *start = sub_gloss;
        char *end = sub_gloss;
        gboolean in_token = FALSE;
        while(in_token || *sub_gloss == '(' || *sub_gloss == ' '){

          if(*sub_gloss == '('){
            in_token = TRUE;
            start = sub_gloss;
          }
          else if(*sub_gloss == ')'){
            in_token = FALSE;
            end = sub_gloss;
          }
          
          if(start < end && *start) {
            *end = 0;
            gchar * GI = start+1;

            if(!first_parentheses){
              if(start_new_gloss){
                //if new gloss create a new gloss struct
                p_gloss = g_new0(gloss, 1);
                dicentry->gloss = g_slist_prepend(dicentry->gloss, p_gloss);
              }
              
              //Sub_Gloss' General Informations: one per pair of parentheses
              //add this GI in the gloss
              p_gloss->general_informations = g_slist_append(p_gloss->general_informations,
                                                             g_strdup(GI));
              if(!g_strcmp0(GI, "P")){
                dicentry->priority = TRUE;
              }

              //if a GI is detected next, add it in the same gloss
              start_new_gloss = FALSE;
            }
            else{
              //if in first parentheses: General Informations of the whole entry
              //add this GI in the entry
              dicentry->general_informations = g_slist_prepend(dicentry->general_informations,
                                                               g_strdup(GI));

              //Entry General Information: list separated by comma in the first
              //pair of parentheses
              gchar *saveptr_entry_GI=NULL;
              gchar *entry_GI = (gchar*)strtok_r(GI, ",", &saveptr_entry_GI);

              do{
                if(!strcmp(entry_GI, "v1")){
                  dicentry->GI = V1;
                }
                else if(g_str_has_prefix(entry_GI, "v5")){
                  dicentry->GI = V5;
                }
                else if(!strcmp(entry_GI, "adj-i")){
                  dicentry->GI = ADJI;
                }
                entry_GI = (gchar*)strtok_r(NULL, ",", &saveptr_entry_GI);
              }while(entry_GI);
              first_parentheses = FALSE;
            }

            start = sub_gloss = end;
          }
          sub_gloss++;
        } //end () token

        //in case there was no GI for this gloss
        if(!p_gloss){
          p_gloss = g_new0(gloss, 1);
          dicentry->gloss = g_slist_prepend(dicentry->gloss, p_gloss);
        }
        
        //the rest of the string is the sub gloss (sub_gloss point at the end
        //of the last pair of parentheses of this sub gloss)
        p_gloss->sub_gloss = g_slist_prepend(p_gloss->sub_gloss,
                                             g_strdup(sub_gloss));

        //create a new gloss at new GI encounter
        start_new_gloss = TRUE;
      }//end if entl or gloss
    }//end if gloss sub not empty
      
    //get part of line after next /
    sub_gloss = (gchar*)strtok_r(NULL, "/", &saveptr_chunk);

    //reverse the prepended data
    //p_gloss->general_informations = g_slist_reverse(p_gloss->general_informations);
    p_gloss->sub_gloss = g_slist_reverse(p_gloss->sub_gloss);
  }while(sub_gloss);
  
  //reverse the prepended data
  dicentry->general_informations = g_slist_reverse(dicentry->general_informations);
  dicentry->gloss = g_slist_reverse(dicentry->gloss);
  
  ////////
  //read definitions in the first chunk
  char *saveptr_jap_definition;
  gchar* jap_definitions = (gchar*)strtok_r(chunk, " ", &saveptr_chunk);

  ////////
  //read the japanese reading in the first chunk
  gchar* jap_readings = (gchar*)strtok_r(NULL, " ", &saveptr_chunk);
  
  //cut jap definitions and jap readings into a list
  //japanese definition
  gchar *jap_definition = (gchar*)strtok_r(jap_definitions, ";", &saveptr_jap_definition);
  do{
    if(jap_definition && strcmp(jap_definition, "\n")){

        //remove the optional trailing parentheses
        //TODO: put the trailing parentheses content in a variable DEFINITION GI
        gchar **jap_definition__GI = g_strsplit(jap_definition, "(", -1);

        
        dicentry->jap_definition = g_slist_prepend(dicentry->jap_definition,
                                                   g_strdup(jap_definition__GI[0]));
        g_strfreev (jap_definition__GI);
        }
    jap_definition = (gchar*)strtok_r(NULL, ";", &saveptr_jap_definition);
  }while(jap_definition);
  dicentry->jap_definition = g_slist_reverse(dicentry->jap_definition);
  
  //optional japanese reading
  if(jap_readings){
    //trim the bracets []
    size_t len = strlen(jap_readings);
    memmove(jap_readings, jap_readings+1, len-2);
    jap_readings[len-2] = 0;  

    char *saveptr_jap_reading;
    gchar *jap_reading = (gchar*)strtok_r(jap_readings, ";", &saveptr_jap_reading);
    do{
      if(jap_reading && strcmp(jap_reading, "\n")){
        //remove the optional trailing parentheses
        //TODO: put the trailing parentheses content in a variable READING GI
        gchar **jap_reading__GI = g_strsplit(jap_reading, "(", -1);
        
        dicentry->jap_reading = g_slist_prepend(dicentry->jap_reading,
                                                g_strdup(jap_reading__GI[0]));
        g_strfreev (jap_reading__GI);
      }
      //next jap reading
      jap_reading = (gchar*)strtok_r(NULL, ";", &saveptr_jap_reading);
    }while(jap_reading);
    dicentry->jap_reading = g_slist_reverse(dicentry->jap_reading);
    }
  
  g_free(line);
  return dicentry;
}

void dicentry_free(GjitenDicentry* dicentry){
  g_slist_free_full(dicentry->gloss, (GDestroyNotify)gloss_free);
  dicentry->gloss = NULL;

  g_slist_free_full(dicentry->jap_definition, g_free);
  dicentry->jap_definition = NULL;
  
  g_slist_free_full(dicentry->jap_reading, g_free);
  dicentry->jap_reading = NULL;
  
  g_slist_free_full(dicentry->general_informations, g_free);
  dicentry->general_informations = NULL;
  
  g_free(dicentry->ent_seq);
  dicentry->ent_seq = NULL;
  
  g_free(dicentry);
}
