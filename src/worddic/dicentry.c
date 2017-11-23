#include "dicentry.h"

GjitenDicentry* parse_line(const gchar* p_line){

  gchar *line = g_strdup(p_line);
  //new entry to return
  GjitenDicentry* dicentry = g_new0 (GjitenDicentry, 1);
  //dicentry->priority = FALSE;

  //cut until the first '/', separating definiton,reading in the first chunk and
  //sensees in the second chunk
  gchar * saveptr_chunk=NULL;
  gchar *chunk = (gchar*)strtok_r(line, "/", &saveptr_chunk);

  ////////
  //read sensees (sub sense) in the second chunk (one sub sense per /)
  gchar *sub_sense_str = (gchar*)strtok_r(NULL, "/", &saveptr_chunk);

  //is it the first parenthese (among all of the sensees)
  gboolean first_parentheses = TRUE;

  //if the last iteration was a GI (detect begining of new sense/new sub sense)
  gboolean start_new_sense = TRUE;

  //pointer to hold a sense before it's added to the entry
  sense *p_sense = NULL;

  do{

    if(sub_sense_str && strcmp(sub_sense_str, "\n") && strcmp(sub_sense_str, " ")){
      //check if this is an edict2 EntL sequance or a sub_sense_str
      if(g_str_has_prefix(sub_sense_str, "EntL")){
        dicentry->ent_seq = g_strdup(sub_sense_str);
      }
      else{
        char *start = sub_sense_str;
        char *end = sub_sense_str;
        gboolean in_token = FALSE;
        while(in_token || *sub_sense_str == '(' || *sub_sense_str == ' '){

          if(*sub_sense_str == '('){
            in_token = TRUE;
            start = sub_sense_str;
          }
          else if(*sub_sense_str == ')'){
            in_token = FALSE;
            end = sub_sense_str;
          }

          if(start < end && *start) {
            *end = 0;
            gchar * GI = start+1;

            if(!first_parentheses){
              if(start_new_sense){
                //if new sense create a new sense struct
                p_sense = g_new0(sense, 1);
                dicentry->sense = g_slist_prepend(dicentry->sense, p_sense);
              }

              //Sub_Sense_Str' General Informations: one per pair of parentheses
              //add this GI in the sense
              p_sense->general_informations = g_slist_append(p_sense->general_informations,
                                                             g_strdup(GI));
              if(!g_strcmp0(GI, "P")){
                dicentry->priority = TRUE;
              }

              //if a GI is detected next, add it in the same sense
              start_new_sense = FALSE;
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

              //by default, set the entry as a noun
              dicentry->GI = NOUN;

              //for all entries, set the GENERAL information
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

            start = sub_sense_str = end;
          }
          sub_sense_str++;
        } //end () token

        //in case there was no GI for this sense
        if(!p_sense){
          p_sense = g_new0(sense, 1);
          dicentry->sense = g_slist_prepend(dicentry->sense, p_sense);
        }

        //the rest of the string is the sub sense (sub_sense_str point at the end
        //of the last pair of parentheses of this sub sense)
	sub_sense *p_sub_sense = g_new0 (sub_sense, 1);
	p_sub_sense->content = g_strdup(sub_sense_str);
        p_sense->sub_sense = g_slist_prepend(p_sense->sub_sense,
                                             p_sub_sense);

        //create a new sense at new GI encounter
        start_new_sense = TRUE;
      }//end if entl or sense
    }//end if sense sub not empty

    //get part of line after next /
    sub_sense_str = (gchar*)strtok_r(NULL, "/", &saveptr_chunk);

    //reverse the prepended data
    //p_sense->general_informations = g_slist_reverse(p_sense->general_informations);
    p_sense->sub_sense = g_slist_reverse(p_sense->sub_sense);
  }while(sub_sense_str);

  //reverse the prepended data
  dicentry->general_informations = g_slist_reverse(dicentry->general_informations);
  dicentry->sense = g_slist_reverse(dicentry->sense);

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

GjitenDicentry* parse_entry_jmdict(xmlNodePtr cur){
  GjitenDicentry* dicentry = g_new0 (GjitenDicentry, 1);
  sense *p_sense = g_new0(sense, 1);
  dicentry->sense = g_slist_prepend(dicentry->sense, p_sense);

  cur = cur->xmlChildrenNode;

  while (cur) {
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"sense"))){
      xmlNodePtr child = cur->xmlChildrenNode;

      while (child){
	if((!xmlStrcmp(child->name, (const xmlChar *)"gloss"))){
	  sub_sense *p_sub_sense = g_new0(sub_sense, 1);
	  p_sense->sub_sense = g_slist_prepend(p_sense->sub_sense, p_sub_sense);

	  gchar *content = (gchar *)xmlNodeGetContent(child);
	  p_sub_sense->content = content;

	  gchar *lang = (gchar *)xmlGetProp(child, (const xmlChar *)"lang");
	  if(lang){
	    strncpy(p_sub_sense->lang, lang, 3);
	    xmlFree(lang);
	  }
	}
	else if((!xmlStrcmp(child->name, (const xmlChar *)"pos")) ||
		(!xmlStrcmp(child->name, (const xmlChar *)"misc"))){
	  gchar *content = (gchar *)xmlNodeGetContent(child);
	  p_sense->general_informations = g_slist_prepend(p_sense->general_informations, content);
	}

	child = child->next;
      }
    }
    else if ((!xmlStrcmp(cur->name, (const xmlChar *)"k_ele"))){
      xmlNodePtr child = cur->xmlChildrenNode;

      while (child){
	if((!xmlStrcmp(child->name, (const xmlChar *)"keb"))){
	  gchar *content = (gchar *)xmlNodeGetContent(child);
	  dicentry->jap_reading = g_slist_prepend(dicentry->jap_reading, content);
	}
	child = child->next;
      }
    }
    else if ((!xmlStrcmp(cur->name, (const xmlChar *)"r_ele"))){
      xmlNodePtr child = cur->xmlChildrenNode;

      while (child){
	if((!xmlStrcmp(child->name, (const xmlChar *)"reb"))){
	  gchar *content = (gchar *)xmlNodeGetContent(child);
	  dicentry->jap_definition = g_slist_prepend(dicentry->jap_definition, content);
	}
	child = child->next;
      }
    }

    cur = cur->next;
  }
  dicentry->GI = GIALL;   //TODO
  return dicentry;
}


void dicentry_free(GjitenDicentry* dicentry){
  g_slist_free_full(dicentry->sense, (GDestroyNotify)sense_free);
  dicentry->sense = NULL;

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
