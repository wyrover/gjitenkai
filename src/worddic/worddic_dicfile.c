#include "worddic_dicfile.h"

void worddic_dicfile_open(WorddicDicfile *dicfile){

  dicfile->fp = fopen(dicfile->path, "r");

  //first line is informations (date, author, copyright, ...)
  //It will also be used to check encoding
  gchar *informations = NULL;
  size_t len = 0;
  ssize_t read = getline(&informations, &len, dicfile->fp);
    
  //check if utf8 or not
  dicfile->utf8 = g_utf8_validate(informations, read, NULL);

  if(!dicfile->utf8){
    dicfile->informations = g_convert (informations, -1, "UTF-8", "EUC-JP",
                                       NULL, NULL, NULL);
  }
  else {
    dicfile->informations = g_strdup(informations);
  }
}

void worddic_dicfile_close(WorddicDicfile *dicfile){
  fclose(dicfile->fp);
}

void worddic_dicfile_parse_all(WorddicDicfile *dicfile){
  char * line = NULL;
  gboolean has_line=TRUE;
  while(has_line){
   has_line = worddic_dicfile_parse_next_line(dicfile, line);
  }
  g_free(line);
  dicfile->entries = g_slist_reverse(dicfile->entries);
}

gboolean worddic_dicfile_parse_next_line(WorddicDicfile *dicfile, gchar *line){
  size_t len = 0;
  ssize_t read;

  //get a line in the file
  read = getline(&line, &len, dicfile->fp);

  //if no more characters to read, return false
  if(read == -1)return FALSE;
  
  gchar *utf_line = NULL;
  if(!dicfile->utf8){
    //if not utf8 convert the line (assum it's EUC-JP)
    utf_line = g_convert (line, -1, "UTF-8", "EUC-JP", NULL, NULL, NULL);
  }
  else {
    utf_line = g_strdup(line);
  }
  
  GjitenDicentry* dicentry = parse_line(utf_line);
  dicfile->entries = g_slist_prepend(dicfile->entries, dicentry);
  g_free(utf_line);

  return TRUE;
}

GList *add_match(GMatchInfo *match_info,
		 gchar *comment,
		 GjitenDicentry* dicentry,
		 GList *results){
  //fetch the matched string
  gchar *word = g_match_info_fetch (match_info, 0);

  //create a new dicresult struct with the entry and the match
  //when freeing the result, do not free the entry
  dicresult *p_dicresult = g_new0(dicresult, 1);
  p_dicresult->match = word;
  p_dicresult->entry = dicentry;
  p_dicresult->comment = comment;
  
  //add the dicentry in the result list
  results = g_list_prepend(results, p_dicresult);

  return results;
}

GList *dicfile_search(WorddicDicfile *dicfile, const gchar *srchstrg_regex){

  //list of matched dictonnary entries
  GList *results = NULL;

  //regex variables
  GError *error = NULL;
  gint start_position = 0;
  gboolean match=FALSE;
  GMatchInfo *match_info;

  GRegex* regex = g_regex_new (srchstrg_regex,
                               G_REGEX_OPTIMIZE,
                               start_position,
                               &error);

  if(!regex)return NULL;
  
  //detect is the search expression is in japanese or latin char
  gboolean jpsrch = detect_japanese(srchstrg_regex);

  if(jpsrch){
    //if the search expression contains at least a japanese character,
    //search matches in the japanese definition or japanese reading

    //check if there if the japanese characters are not hiragana or katakana, meaning
    //that there are only kanji except for regex characters. this variable can be used
    //to ignore the reading unit to improve the speed a bit
    gboolean only_kanji = (!hasKatakanaString(srchstrg_regex) &&
                           !hasHiraganaString(srchstrg_regex));

    GSList* list_dicentry = NULL;
    for(list_dicentry = dicfile->entries;
        list_dicentry != NULL;
        list_dicentry = list_dicentry->next){

      GjitenDicentry* dicentry = list_dicentry->data;

      GSList *jap_definition = dicentry->jap_definition;

      //search in the definition
      while(jap_definition != NULL){
        match = g_regex_match (regex, jap_definition->data, 0, &match_info);

        if(match)break;
        else jap_definition = jap_definition->next; 
      }

      //if no match in the definition and if the search string is not only kanji
      //search in the reading (if any)
      if(!match && dicentry->jap_reading && !only_kanji){
        GSList *jap_reading = dicentry->jap_reading;
        while(jap_reading != NULL){
          match = g_regex_match (regex, jap_reading->data, 0, &match_info);

          if(match)break;
          else jap_reading = jap_reading->next; 
        }
      }

      //if there is a match, copy the entry into the result list
      if(match){results = add_match(match_info, NULL, dicentry, results);}
    }
  }
  else{
    //if there are no japanese characters, search matches in the gloss
    GSList* list_dicentry = NULL;
    for(list_dicentry = dicfile->entries;
        list_dicentry != NULL;
        list_dicentry = list_dicentry->next){

      GjitenDicentry* dicentry = list_dicentry->data;

      GSList *gloss_list = dicentry->gloss;
      while(gloss_list != NULL){
        gloss *gloss = gloss_list->data;
        GSList *sub_gloss_list = gloss->sub_gloss;
        while(sub_gloss_list != NULL){
          match = g_regex_match (regex, sub_gloss_list->data, 0, &match_info);
          if(match)break;
          else sub_gloss_list = sub_gloss_list->next;
        }
        
        gloss_list = gloss_list->next; 
      }

      //if there is a match, copy the entry into the result list
      if(match){results = add_match(match_info, NULL, dicentry, results);}
    }
  }

  g_regex_unref(regex);
  
  return results;
}

void worddic_dicfile_free(WorddicDicfile *dicfile){
  g_free(dicfile->informations);
  worddic_dicfile_free_entries(dicfile);
}

void worddic_dicfile_free_entries(WorddicDicfile *dicfile){
  g_slist_free_full(dicfile->entries, (GDestroyNotify)dicentry_free);
  dicfile->entries = NULL;
}
