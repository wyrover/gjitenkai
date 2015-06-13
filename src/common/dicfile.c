#include "dicfile.h"

extern GjitenConfig conf;

int dicfile_load(GjitenDicfile* dicfile){
  //if the dictionary is not initialized, init: open a file descriptor
  if (dicfile->status == DICFILE_NOT_INITIALIZED) {
    if (dicfile_init(dicfile) == FALSE) return SRCH_FAIL; 
  }
  if (dicfile->status != DICFILE_OK) return SRCH_FAIL;

  //if the mapped dictionary is not the requested dictionnary then clear it 
  if ((dicfile != conf.mmaped_dicfile) && (conf.mmaped_dicfile != NULL)) {
    g_printf("free mem of previously used dicfile\n");
    dicutil_unload_dic();
  }

  //if no mapped dictionary, load into memory from the dic's file descriptor
  if (conf.mmaped_dicfile == NULL) {
    g_printf("load dicfile %s %s into memory\n", dicfile->name, dicfile->path);
    conf.mmaped_dicfile = dicfile;
    dicfile->mem = read_file(dicfile->path);

    if (dicfile->mem == NULL) gjiten_abort_with_msg("mmap() failed\n");
    conf.mmaped_dicfile = dicfile;
  }
}


gboolean dicfile_check_all(GSList *dicfile_list) {
  GSList *node;
  GjitenDicfile *dicfile;
  gboolean retval = TRUE;

  GJITEN_DEBUG("dicfile_check_all()\n");

  node = dicfile_list;
  while (node != NULL) {
    if (node->data != NULL) {
      dicfile = node->data;
      if (dicfile_init(dicfile) == FALSE) retval = FALSE;
      if (dicfile_is_utf8(dicfile) == FALSE) {
        dicfile_close(dicfile);
        retval = FALSE;
      }
      dicfile_close(dicfile);
    }
    node = g_slist_next(node);
  }
  GJITEN_DEBUG(" retval: %d\n", retval);
  return retval;
}

gboolean dicfile_is_utf8(GjitenDicfile *dicfile) {
  gchar *testbuffer;
  gint pos, bytesread;

  if (dicfile->file > 0) {
    testbuffer = (gchar *) g_malloc(3000);
    bytesread = read(dicfile->file, testbuffer, 3000); // read a chunk into buffer
    pos = bytesread - 1;
    while (testbuffer[pos] != '\n') pos--;
    if (g_utf8_validate(testbuffer, pos, NULL) == FALSE) {
      gjiten_print_error(_("Dictionary file is non-UTF: %s\nPlease convert it to UTF-8. See the docs for more."), dicfile->path);
      return FALSE;
    }
    g_free(testbuffer);
  }
  return TRUE;
}

gboolean dicfile_init(GjitenDicfile *dicfile) {

  if (dicfile->status != DICFILE_OK) {
    dicfile->file = open(dicfile->path, O_RDONLY);

    if (dicfile->file == -1) {
      gjiten_print_error(_("Error opening dictfile:  %s\nCheck your preferences!"),
			 dicfile->path);
      dicfile->status = DICFILE_BAD;
      return FALSE;
    }
    else {
      if (stat(dicfile->path, &dicfile->stat) != 0) {
        printf("**ERROR** %s: stat() \n", dicfile->path);
        dicfile->status = DICFILE_BAD;
        return FALSE;
      }
      else {
        dicfile->size = dicfile->stat.st_size;
      }
    }
    dicfile->status = DICFILE_OK;
  }
  return TRUE;
}

void dicfile_close(GjitenDicfile *dicfile) {

  if (dicfile->file > 0) {
    close(dicfile->file);
  }
  dicfile->status = DICFILE_NOT_INITIALIZED;
}

void dicfile_list_free(GSList *dicfile_list) {
  GSList *node;
  GjitenDicfile *dicfile;

  node = dicfile_list;
  while (node != NULL) {
    if (node->data != NULL) {
      dicfile = node->data;
      dicfile_close(dicfile);
      g_free(dicfile);
    }
    node = g_slist_next(node);
  }

  g_slist_free(dicfile_list);
}

GList *dicfile_search_regex(GjitenDicfile *dicfile,
			    const gchar *srchstrg_regex,
			    GList **matched_part){
  GList *results = NULL;      //list of matched dictonnary entries text

  gint search_result;
  gchar *linestart, *lineend;

  GError *error = NULL;
  gint string_len = -1;
  gint start_position = 0;
  gboolean match;
  GMatchInfo *match_info;

  GRegex * regex = g_regex_new (srchstrg_regex,
				G_REGEX_OPTIMIZE,
				0,
				&error);
  gboolean end_of_mem = FALSE;
  gchar *line = NULL;

  lineend = dicfile->mem;
    
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
        
    //search match of the regex in the current line
    match = g_regex_match_full (regex, dicentry->jap_definition,
                                strlen(dicentry->jap_definition),
    				start_position, 0,
                                &match_info, &error);

    //if there is a match, copy the line into the result list
    if(match){
      //fetch the matched string
      gchar *word = g_match_info_fetch (match_info, 0);

      //duplicate line as it wil be reallocated
      gchar *line_cpy = strdup(line);
      results = g_list_append(results, line_cpy);

      *matched_part = g_list_append(*matched_part, word);
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
      case REGEX:
	break;
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

gint search_string(gint srchtype, GjitenDicfile *dicfile, gunichar *srchstrg,
                   guint32 *res_index, gint *hit_pos, gint *res_len, gchar *res_str){
  gint search_result;
  gchar *linestart, *lineend; 
  gint copySize = 1023;
  static gchar *linsrchptr;
  
  //if first time this expression is searched
  if (srchtype == SRCH_START) {
    //start the search from the begining 
    linsrchptr = dicfile->mem;
  }

 bad_hit:
  search_result = SRCH_FAIL; // assume search fails 

  //search next occurance of the string
  linsrchptr = strstr(linsrchptr, srchstrg);

  if (linsrchptr != NULL) {  // if we have a match
    linestart = linsrchptr;

    // find beginning of line
    while ((*linestart != '\n') && (linestart != dicfile->mem)) linestart--;
    if (linestart == dicfile->mem) {   
      if ((isKanjiChar(g_utf8_get_char(linestart)) == FALSE) && 
          (isKanaChar(g_utf8_get_char(linestart)) == FALSE)) 
        {
          linsrchptr++;
          goto bad_hit;
        }
    }
		
    linestart++;
    lineend = linestart;
    *hit_pos = linsrchptr - linestart;
    while (*lineend != '\n') { // find end of line
      lineend++;
      if (lineend >= dicfile->mem + dicfile->size) { 
        printf("weird.\n");
        break;
      }
    }
    linsrchptr++;	
    if ((lineend - linestart + 1) < 1023) copySize = lineend - linestart + 1;
    else copySize = 1023;
    strncpy(res_str, linestart, copySize);
    res_str[copySize] = 0;
    *res_index  = (guint32)linestart;
    search_result = SRCH_OK; // search succeeded 
  }

  return search_result;
}
