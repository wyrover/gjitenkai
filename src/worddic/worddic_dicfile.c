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

    GjitenDicentry* dicentry = parse_line(line);
    entries = g_list_prepend(entries, dicentry);
  }

  entries = g_list_reverse(entries);
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
  
  //detect is the search expression is in japanese or latin char
  gboolean jpsrch = detect_japanese(srchstrg_regex);

  GList* list_dicentry = NULL;
  for(list_dicentry = dicfile->entries;
      list_dicentry != NULL;
      list_dicentry = list_dicentry->next){

    GjitenDicentry* dicentry = list_dicentry->data;
      
    if(jpsrch){
      //if the search expression contains at least a japanese character,
      //search matches in the japanese definition and japanese reading
      match = g_regex_match_full (regex, dicentry->jap_definition,
                                  strlen(dicentry->jap_definition),
                                  start_position, 0,
                                  &match_info, &error);
      if(!match){
        match = g_regex_match_full (regex, dicentry->jap_reading,
                                    strlen(dicentry->jap_reading),
                                    start_position, 0,
                                    &match_info, &error);
      }

    }
    else{
      //if there is no japanese characters, search matches in the translation
      //definitions
      GList *definition = dicentry->definitions;  //browse definitions
      
      while(definition != NULL){
        match = g_regex_match_full (regex, definition->data,
                                    strlen(definition->data),
                                    start_position, 0,
                                    &match_info, &error);

        if(match)break;
        else definition = definition->next; 
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

  return results;
}

GList *dicfile_search(GjitenDicfile *dicfile, const gchar *srchstrg, 
                      gint match_criteria_jp, gint match_criteria_lat, 
                      gint match_type)
{

  GList *results = NULL;      //list of matched dictonnary entries text
  gint srchresp, roff, rlen;  //text, offset, lenght of dictionary entries

  guint32 respos, oldrespos;

  //tells if the matched expression should be added to the result list 
  //according to the match criteria
  gboolean match_criteria;

  //tells if it's the first time the expression is searched
  gint gjit_search = SRCH_START;

  //detect is the search expression is in japanese or latin char
  int jpsrch = FALSE;
  if(detect_japanese(srchstrg)){
    jpsrch = TRUE;
  }

  int srchpos;
  oldrespos = srchpos = 0;
  
  //search loop
  do {
    gchar *repstr = (gchar*)g_new(gchar*, 1024);
    oldrespos = respos;

    
    srchresp = search_string(gjit_search, dicfile, srchstrg, &respos, &roff, &rlen, repstr);

    if (srchresp != SRCH_OK)  {
      g_free(repstr);
      break;
    }
    
    if (gjit_search == SRCH_START) {
      srchpos = respos;
      gjit_search = SRCH_CONT;
    }

    srchpos++;
    if (oldrespos == respos) continue;
    
    // Check match type and search options
    match_criteria = FALSE;
    if (jpsrch) {
      match_type = get_jp_match_type(repstr, srchstrg, roff);
      switch (match_criteria_jp) {
      case EXACT_MATCH : 
        if (match_type == EXACT_MATCH) match_criteria = TRUE;
        break;
      case START_WITH_MATCH:
        if ((match_type == START_WITH_MATCH) || (match_type == EXACT_MATCH)) match_criteria = TRUE;
        break;
      case END_WITH_MATCH:
        if ((match_type == END_WITH_MATCH) || (match_type == EXACT_MATCH)) match_criteria = TRUE;
        break;
      case ANY_MATCH:
        match_criteria = TRUE;
        break;
      }
    }
    else { //Non-japanese search
      switch (match_criteria_lat) {
      case EXACT_MATCH:
        //Must lie between two '/' delimiters
        if ((repstr[roff - 1] == '/') && (repstr[roff + strlen(srchstrg)] == '/')) match_criteria = TRUE;
        //take "/(n) expression/" into accont
        else if ((repstr[roff - 2] == ')') && (repstr[roff + strlen(srchstrg)] == '/')) match_criteria = TRUE;
        //also match verbs starting with a 'to'. eg: "/(n) to do_something/"
        else if ((repstr[roff - 2] == 'o') && (repstr[roff - 3] == 't') && (repstr[roff + strlen(srchstrg)] == '/') 
                 && ((repstr[roff - 5] == ')') || (repstr[roff - 4] == '/'))) match_criteria = TRUE;
        break;
      case WORD_MATCH:
        if ((g_unichar_isalpha(g_utf8_get_char(repstr + roff + strlen(srchstrg))) == FALSE)  &&
            (g_unichar_isalpha(g_utf8_get_char(repstr + roff - 1)) == FALSE)) {
          match_criteria = TRUE;
        }
        break;
      case ANY_MATCH:
        match_criteria = TRUE;
        break;
        //case REGEX:
	//break;
      }
    } 
    
    /*printf("Match -> %s\n", repstr);  
      printf("offset: %d: ", roff);
      printf("jptype: %d\n", match_type);*/

    //append the matched expression in the result list
    if(match_criteria){
      results = g_list_append (results, repstr);
    }

    //if limit reached, break
    if(g_list_length(results) > DEFMAXWORDMATCHES){
      results = g_list_append (results, "-- result truncated --");
      break;
    }

  } while (srchresp == SRCH_OK);

  return results;
}
