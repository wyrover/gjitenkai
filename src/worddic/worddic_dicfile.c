#include "worddic_dicfile.h"

void worddic_dicfile_open(WorddicDicfile *dicfile){

  if(g_str_has_suffix(dicfile->path, ".gz")){
    dicfile->is_gz = TRUE;
  }
  else{
    dicfile->is_gz = FALSE;
  }

  if(dicfile->is_gz){
    dicfile->fp = (FILE*)gzopen(dicfile->path, "r");
  }
  else{
    dicfile->fp = fopen(dicfile->path, "r");
  }
  
  //first line is informations (date, author, copyright, ...)
  //It will also be used to check encoding
  gchar *informations = NULL;
  size_t len = 0;
  ssize_t read;
  
  if(!dicfile->is_gz){
    read = getline(&informations, &len, dicfile->fp);
  }
  else{
    char buffer[GZLEN];
    gchar *tmpline = gzgets ((gzFile)dicfile->fp, buffer, GZLEN - 1);
    if(tmpline){
      informations = strdup(tmpline);
      read = strlen(informations);
    }
    else {
      read = -1;
      return;
    }
  }
  
  //check if utf8 or not
  dicfile->utf8 = g_utf8_validate(informations, read, NULL);

  if(!dicfile->utf8){
    dicfile->informations = g_convert (informations, -1, "UTF-8", "EUC-JP",
                                       NULL, NULL, NULL);
    g_free(informations);
  }
  else {
    if(!dicfile->informations){
      dicfile->informations = informations;
    }
  }
}

void worddic_dicfile_close(WorddicDicfile *dicfile){
  if(dicfile->is_gz)gzclose((gzFile)dicfile->fp);
  else fclose(dicfile->fp);
}

void worddic_dicfile_parse_all(WorddicDicfile *dicfile){
  gboolean has_line=TRUE;
  while(has_line){
    has_line = worddic_dicfile_parse_next_line(dicfile);
  }
  dicfile->entries = g_slist_reverse(dicfile->entries);
}

gboolean worddic_dicfile_parse_next_line(WorddicDicfile *dicfile){
  size_t len = 0;
  ssize_t read;

  gchar *line=NULL;
  //get a line in the file
  if(!dicfile->is_gz){
    read = getline(&line, &len, dicfile->fp);
  }
  else{    
    gchar buffer[GZLEN];
    gchar *tmpline = (gchar*)gzgets ((gzFile)dicfile->fp, buffer, GZLEN - 1);
   
    if(tmpline){
      line = strdup(tmpline);
      read = strlen(line);
    }
    else {
      read = -1;
    }
  }
  
  //if no more characters to read, return false
  if(read == -1){
    g_free(line);
    return FALSE;
  }
  
  gchar *utf_line = NULL;
  if(!dicfile->utf8){
    //if not utf8 convert the line (assum it's EUC-JP)
    utf_line = g_convert (line, -1, "UTF-8", "EUC-JP", NULL, NULL, NULL);
    g_free(line);
  }
  else {
    utf_line = line;
  }

  if(utf_line){
    GjitenDicentry* dicentry = parse_line(utf_line);
    dicfile->entries = g_slist_prepend(dicfile->entries, dicentry);
    g_free(utf_line);
  }
  
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
  if(comment)p_dicresult->comment = strdup(comment);
  else p_dicresult->comment = NULL;
  
  //add the dicentry in the result list
  results = g_list_prepend(results, p_dicresult);

  return results;
}

GList *dicfile_search(WorddicDicfile *dicfile,
                      search_expression *p_search_expression,
                      gchar *comment,
                      enum entry_GI itype,
                      gint is_jp){

  //variable from search expression structure
  const gchar *search_text = p_search_expression->search_text;
  enum dicfile_search_criteria search_criteria_jp = p_search_expression->search_criteria_jp;
  enum dicfile_search_criteria search_criteria_lat = p_search_expression->search_criteria_lat;
  
  //list of matched dictonnary entries
  GList *results = NULL;

  //detect is the search expression is in japanese or latin char
  if(is_jp <= -1)is_jp = detect_japanese(search_text);

  /////////////////////////////////////////////////////////////////
  //modify the expression with anchors according to the search criteria
  //create a GString to do so
  GString *entry_string = g_string_new(search_text);
  if(is_jp){
    if(search_criteria_jp == EXACT_MATCH){
      entry_string = g_string_prepend_c(entry_string, '^');
      entry_string = g_string_append_c(entry_string, '$');
    }
    else if(search_criteria_jp == START_WITH_MATCH){
      entry_string = g_string_prepend_c(entry_string, '^');
    }
  
    else if(search_criteria_jp == END_WITH_MATCH){
      entry_string = g_string_append_c(entry_string, '$');
    }
  }
  else{
    if(search_criteria_lat == EXACT_MATCH){
      entry_string = g_string_prepend_c(entry_string, '^');
      entry_string = g_string_append_c(entry_string, '$');      
    }
    else if(search_criteria_lat == WORD_MATCH){
      entry_string = g_string_prepend(entry_string, "\\b");
      entry_string = g_string_append(entry_string, "\\b");
    }
  }
  ////////

  //regex variables
  GError *error = NULL;
  gint start_position = 0;
  gboolean has_matched=FALSE;
  GMatchInfo *match_info = NULL;
  
  GRegex* regex = g_regex_new (entry_string->str,
                               G_REGEX_OPTIMIZE|
                               G_REGEX_NO_AUTO_CAPTURE|
                               G_REGEX_CASELESS,
                               start_position,
                               &error);
  //free memory
  g_string_free(entry_string, TRUE);

  //cannot continue the search if the regex is invalid
  if(!regex)return NULL;
  
  if(is_jp){
    //search matches in the japanese definition or japanese reading
    //if the search expression contains at least a japanese character

    //check if there if the japanese characters are not hiragana or katakana, meaning
    //that there are only kanji except for regex characters. this variable can be used
    //to ignore the reading unit to improve the speed a bit
    gboolean only_kanji = (!hasKatakanaString(search_text) &&
                           !hasHiraganaString(search_text));

    GSList* list_dicentry = NULL;
    for(list_dicentry = dicfile->entries;
        list_dicentry != NULL;
        list_dicentry = list_dicentry->next){
      
      GjitenDicentry* dicentry = list_dicentry->data;

      //skip this entry if the type is not what we are searching
      if(!(dicentry->GI & itype))continue;

      has_matched = FALSE;
            
      //search in the definition
      GSList *jap_definition = dicentry->jap_definition;
      while(jap_definition && !has_matched){
        has_matched = g_regex_match (regex, jap_definition->data, 0, &match_info);

        //if there is a match, copy the entry into the result list
        if(has_matched){
          results = add_match(match_info, comment, dicentry, results);
        }
        else{
          jap_definition = jap_definition->next;
        }
        
        g_match_info_unref(match_info);
      }
      
      //search in the japanese reading (if any)
      //if no match in the definition and if the search string is not only kanji
      if(!has_matched && dicentry->jap_reading && !only_kanji){
        GSList *jap_reading = dicentry->jap_reading;
        while(jap_reading && !has_matched){
          has_matched = g_regex_match (regex, jap_reading->data, 0, &match_info);

          if(has_matched){
            results = add_match(match_info, comment, dicentry, results);
          }
          else{
            jap_reading = jap_reading->next;
          }
          
          g_match_info_unref(match_info);
        
        }
      }
      
    } //for all dictionary entries
  }
  else{
    //if there are no japanese characters, search matches in the glosses
    //(latin characters search)
    GSList* list_dicentry = NULL;
    for(list_dicentry = dicfile->entries;
        list_dicentry != NULL;
        list_dicentry = list_dicentry->next){
            
      has_matched = FALSE;
      GjitenDicentry* dicentry = list_dicentry->data;

      //check if the type match what we are searching
      if(!(dicentry->GI & itype))continue;
      
      GSList *gloss_list = dicentry->gloss;
      //search in the gloss list
      while(gloss_list && !has_matched){
        gloss *gloss = gloss_list->data;
        GSList *sub_gloss_list = gloss->sub_gloss;
        //search in the sub glosses
        while(sub_gloss_list && !has_matched){
          has_matched = g_regex_match (regex, sub_gloss_list->data, 0, &match_info);
          
          if(has_matched){
            results = add_match(match_info, comment, dicentry, results);
          }
          else {
            sub_gloss_list = sub_gloss_list->next;
          }

          g_match_info_unref(match_info);
        }
        
        gloss_list = gloss_list->next;
      }
    }
  }

  //free memory
  g_regex_unref(regex);
  
  return results;
}

void worddic_dicfile_free(WorddicDicfile *dicfile){
  g_free(dicfile->name);
  dicfile->name = NULL;

  g_free(dicfile->path);
  dicfile->path = NULL;
  
  g_free(dicfile->informations);
  dicfile->informations = NULL;
  
  worddic_dicfile_free_entries(dicfile);
  
  g_free(dicfile);
}

void worddic_dicfile_free_entries(WorddicDicfile *dicfile){
  g_slist_free_full(dicfile->entries, (GDestroyNotify)dicentry_free);
  dicfile->entries = NULL;
}
