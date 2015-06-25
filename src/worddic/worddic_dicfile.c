#include "worddic_dicfile.h"


GList *worddic_dicfile_parse(GjitenDicfile *dicfile){
  
  //file browsing variable
  gchar *linestart, *lineend;
  gboolean end_of_mem = FALSE;
  gchar *line = NULL;
  lineend = dicfile->mem;
  
  gint string_len = -1;

  GList *entries = NULL;
  
  //until the end of mem chunk is reached
  while(!end_of_mem){
    linestart = lineend;
    // find end of line
    while (*lineend != '\n') {

      //multibyte character or single byte character 
      if (g_unichar_iswide((gunichar)lineend))lineend = (gchar*)g_utf8_next_char(lineend);
      else lineend++;

      if (lineend >= dicfile->mem + dicfile->size) {
	end_of_mem = TRUE;
        break;
      }
      
    }

    lineend++;  //skip the carriage return
    gulong linesize = lineend - linestart;
    line = g_realloc(line, linesize + 1);
    memmove(line, linestart, linesize);
    line[linesize] = '\0';

    if(line){
      GjitenDicentry* dicentry = parse_line(line);
      entries = g_slist_prepend(entries, dicentry);
    }
  }

  entries = g_slist_reverse(entries);
}

GList *dicfile_search_regex(WorddicDicfile *dicfile,
			    const gchar *srchstrg_regex,
			    GList **matched_part){

  //list of matched dictonnary entries
  GList *results = NULL;

  //regex variables
  GError *error = NULL;
  gint start_position = 0;
  gboolean match;
  GMatchInfo *match_info;

  GRegex* regex = g_regex_new (srchstrg_regex,
				G_REGEX_OPTIMIZE,
				0,
				&error);

  if(!regex)return NULL;
  
  //detect is the search expression is in japanese or latin char
  gboolean jpsrch = detect_japanese(srchstrg_regex);

  GList* list_dicentry = NULL;
  for(list_dicentry = dicfile->entries;
      list_dicentry != NULL;
      list_dicentry = list_dicentry->next){

    GjitenDicentry* dicentry = list_dicentry->data;
      
    if(jpsrch){
      //if the search expression contains at least a japanese character,
      //search matches in the japanese definition or japanese reading
      GList *jap_definition = dicentry->jap_definition;
      while(jap_definition != NULL){
        match = g_regex_match (regex, jap_definition->data, 0, &match_info);

        if(match)break;
        else jap_definition = jap_definition->next; 
      }
      
      
      if(!match && dicentry->jap_reading){
        GList *jap_reading = dicentry->jap_reading;
        while(jap_reading != NULL){
          match = g_regex_match (regex, jap_reading->data, 0, &match_info);

          if(match)break;
          else jap_reading = jap_reading->next; 
        }
        
      }
    }
    else{
      //if there is no japanese characters, search matches in the gloss
      GList *gloss = dicentry->gloss;
      while(gloss != NULL){
        match = g_regex_match (regex, gloss->data, 0, &match_info);

        if(match)break;
        else gloss = gloss->next; 
        }
    }
    
    //if there is a match, copy the entry into the result list
    if(match){
      //fetch the matched string
      gchar *word = g_match_info_fetch (match_info, 0);

      //add the matched string to the match part list 
      *matched_part = g_list_append(*matched_part, word);

      //add the result entry in the result list
      results = g_list_append(results, dicentry);
    }
  }

  g_match_info_free(match);
  if(regex != NULL)g_regex_unref (regex);
  
  return results;
}
