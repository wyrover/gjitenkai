#include "dicentry.h"

GjitenDicentry* parse_line(gchar* line){
  //new entry to return
  GjitenDicentry* dicentry = g_new0 (GjitenDicentry, 1);

  //cut until the first '/', separating definiton,reading in the first chunk and
  //glosses in the second chunk
  gchar *chunk = strtok(line, "/");
  
  ////////
  //read glosses (sub gloss) in the second chunk (one sub gloss per /)
  gchar *sub_gloss = strtok(NULL, "/");

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
        dicentry->ent_seq = g_strdup_printf("%s", g_strdup_printf("%s", sub_gloss));
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
              p_gloss->general_informations = g_slist_prepend(p_gloss->general_informations,
                                                             g_strdup_printf("%s", GI));

              //if a GI is detected next, add it in the same gloss
              start_new_gloss = FALSE;
            }            
            else{
              //if in first parentheses: General Informations of the whole entry
              //add this GI in the entry
              dicentry->general_informations = g_slist_prepend(dicentry->general_informations,
                                                              g_strdup_printf("%s", GI));

              //Entry General Information: list separated by comma in the first
              //pair of parentheses
              gchar *saveptr;
              gchar *entry_GI = strtok_r(GI, ",", &saveptr);              
              do{
                if(!strcmp(entry_GI, "v1")){
                  dicentry->GI = V1;
                }
                else if(g_str_has_suffix(entry_GI, "v5")){
                  dicentry->GI = V5;
                }
                else if(!strcmp(entry_GI, "adj-i")){
                  dicentry->GI = ADJI;
                }
                entry_GI = strtok_r(NULL, ",", &saveptr);
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
                                            strdup(sub_gloss));

        //create a new gloss at new GI encounter
        start_new_gloss = TRUE;
      }//end if entl or gloss
    }//end if gloss sub not empty
    sub_gloss = strtok(NULL, "/");

    //reverse the prepended data
    p_gloss->general_informations = g_slist_reverse(p_gloss->general_informations);
    p_gloss->sub_gloss = g_slist_reverse(p_gloss->sub_gloss);
  }while(sub_gloss);

  //reverse the prepended data
  dicentry->general_informations = g_slist_reverse(dicentry->general_informations);
  dicentry->gloss = g_slist_reverse(dicentry->gloss);
  
  ////////
  //read definitions in the first chunk
  gchar* jap_definitions = strtok(chunk, " ");

  ////////
  //read the japanese reading in the first chunk
  gchar* jap_readings = strtok(NULL, " ");
  
  //cut jap definitions and jap readings into a list
  //japanese definition
  gchar *jap_definition = strtok(jap_definitions, ";");
  do{
    if(jap_definition && strcmp(jap_definition, "\n")){

        //remove the optional trailing parentheses
        //TODO: put the trailing parentheses content in a variable DEFINITION GI
        gchar **jap_definition__GI = g_strsplit(jap_definition, "(", -1);

        
      dicentry->jap_definition = g_slist_prepend(dicentry->jap_definition,
                                                g_strdup_printf("%s", jap_definition__GI[0]));
    }
    jap_definition = strtok(NULL, ";");
  }while(jap_definition);
  dicentry->jap_definition = g_slist_reverse(dicentry->jap_definition);
  
  //optional japanese reading
  if(jap_readings){
    //trim the bracets []
    size_t len = strlen(jap_readings);
    memmove(jap_readings, jap_readings+1, len-2);
    jap_readings[len-2] = 0;  

    gchar *jap_reading = strtok(jap_readings, ";");
    do{
      if(jap_reading && strcmp(jap_reading, "\n")){
        //remove the optional trailing parentheses
        //TODO: put the trailing parentheses content in a variable READING GI
        gchar **jap_reading__GI = g_strsplit(jap_reading, "(", -1);
        
        dicentry->jap_reading = g_slist_prepend(dicentry->jap_reading,
                                                g_strdup_printf("%s", jap_reading__GI[0]));        
      }
      //next jap reading
      jap_reading = strtok(NULL, ";");
    }while(jap_reading);
    dicentry->jap_reading = g_slist_reverse(dicentry->jap_reading);
  }
  
  return dicentry;
}

void dicentry_free(GjitenDicentry* dicentry){
  g_slist_free_full(dicentry->gloss, gloss_free);
  dicentry->gloss = NULL;
  g_slist_free_full(dicentry->jap_definition, g_free);
  dicentry->jap_definition = NULL;
  g_slist_free_full(dicentry->jap_reading, g_free);
  dicentry->jap_reading = NULL;
  //g_free(dicentry);
}
