#include "worddic_dicfile.h"

void worddic_dicfile_parse(WorddicDicfile *dicfile){
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;

  fp = fopen(dicfile->path, "r");
  if (fp == NULL)
    exit(EXIT_FAILURE);
  
  read = getline(&line, &len, fp);

  while ((read = getline(&line, &len, fp)) != -1) {
    GjitenDicentry* dicentry = parse_line(line);
    dicfile->entries = g_slist_prepend(dicfile->entries, dicentry);
  }

  fclose(fp);
       
  dicfile->entries = g_slist_reverse(dicfile->entries);
}

inline GList *add_match(GMatchInfo *match_info,
                        GjitenDicentry* dicentry,
                        GList *results){
  //fetch the matched string
  gchar *word = g_match_info_fetch (match_info, 0);

  //create a new dicresult struct with the entry and the match
  dicresult *p_dicresult = g_new0(dicresult, 1);
  p_dicresult->match = word;
  p_dicresult->entry = dicentry;
      
  //add the dicentry in the result list
  results = g_list_append(results, p_dicresult);
}

GList *dicfile_search(WorddicDicfile *dicfile, const gchar *srchstrg_regex){

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

  if(jpsrch){

    //if the search expression contains at least a japanese character,
    //search matches in the japanese definition or japanese reading

    //check if there if the japanese characters are not hiragana or katakana, meaning
    //that there are only kanji except for regex characters. this variable can be used
    //to ignore the reading unit to improve the speed a bit
    gboolean only_kanji = (!hasKatakanaString(srchstrg_regex) &&
                           !hasHiraganaString(srchstrg_regex));

    GList* list_dicentry = NULL;
    for(list_dicentry = dicfile->entries;
        list_dicentry != NULL;
        list_dicentry = list_dicentry->next){

      GjitenDicentry* dicentry = list_dicentry->data;

      GList *jap_definition = dicentry->jap_definition;

      //search in the definition
      while(jap_definition != NULL){
        match = g_regex_match (regex, jap_definition->data, 0, &match_info);

        if(match)break;
        else jap_definition = jap_definition->next; 
      }

      //if no match in the definition, search in the reading
      if(!match && dicentry->jap_reading && !only_kanji){
        GList *jap_reading = dicentry->jap_reading;
        while(jap_reading != NULL){
          match = g_regex_match (regex, jap_reading->data, 0, &match_info);

          if(match)break;
          else jap_reading = jap_reading->next; 
        }
      }

      //if there is a match, copy the entry into the result list
      if(match){results = add_match(match_info, dicentry, results);}
    }
  }
  else{
    //if there is no japanese characters, search matches in the gloss
    GList* list_dicentry = NULL;
    for(list_dicentry = dicfile->entries;
        list_dicentry != NULL;
        list_dicentry = list_dicentry->next){

      GjitenDicentry* dicentry = list_dicentry->data;

      GList *gloss = dicentry->gloss;
      while(gloss != NULL){
        match = g_regex_match (regex, gloss->data, 0, &match_info);

        if(match)break;
        else gloss = gloss->next; 
      }

      //if there is a match, copy the entry into the result list
      if(match){results = add_match(match_info, dicentry, results);}
    }
  }

  g_match_info_free(match);
  if(regex != NULL)g_regex_unref(regex);
  
  return results;
}
