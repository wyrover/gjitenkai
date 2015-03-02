#include "inflection.h"

// Load & initialize verb inflection details
void Verbinit() {
  vinfl_list=NULL;
  static int verbinit_done = FALSE;
  gchar *tmp_ptr;
  int vinfl_size = 0;
  struct stat vinfl_stat;
  gchar *vinfl_start, *vinfl_ptr, *vinfl_end;
  int vinfl_part = 1;
  int conj_type = 40;
  struct vinfl_struct *tmp_vinfl_struct;
  GSList *tmp_list_ptr = NULL;

  if (verbinit_done == TRUE) {
    //printf("Verbinit already done!\n");
    return;
  }

  vinfl_start = read_file(VINFL_FILENAME);

  vinfl_end = vinfl_start + strlen(vinfl_start);
  vinfl_ptr = vinfl_start;

  vinfl_part = 1;
  while ((vinfl_ptr < vinfl_end) && (vinfl_ptr != NULL)) {
    if (*vinfl_ptr == '#') {  //find $ as first char on the line
      vinfl_ptr = get_eof_line(vinfl_ptr, vinfl_end); //Goto next line
      continue;
    }
    if (*vinfl_ptr == '$') vinfl_part = 2;
    
    switch (vinfl_part) {
    case 1:
      if (g_ascii_isdigit(*vinfl_ptr) == TRUE) { //Conjugation numbers
        conj_type = atoi(vinfl_ptr);
        if ((conj_type < 0) || (conj_type > 39)) break;
        while (g_ascii_isdigit(*vinfl_ptr) == TRUE) vinfl_ptr = g_utf8_next_char(vinfl_ptr); //skip the number
        while (g_ascii_isspace(*vinfl_ptr) == TRUE) vinfl_ptr = g_utf8_next_char(vinfl_ptr); //skip the space
        tmp_ptr = vinfl_ptr; // beginning of conju	gation definition;
        vinfl_ptr = get_eof_line(vinfl_ptr, vinfl_end); //	find end of line
        vconj_types[conj_type] = g_strndup(tmp_ptr, vinfl_ptr - tmp_ptr -1);
        //printf("%d : %s\n", conj_type, vconj_types[conj_	type]);
      }
      break;
    case 2:
      if (g_unichar_iswide(g_utf8_get_char(vinfl_ptr)) == FALSE) {
        vinfl_ptr =  get_eof_line(vinfl_ptr, vinfl_end);
        break;
      }
      tmp_vinfl_struct = (struct vinfl_struct *) malloc (sizeof(struct vinfl_struct));
      tmp_ptr = vinfl_ptr;
      while (g_unichar_iswide(g_utf8_get_char(vinfl_ptr)) == TRUE) {
        vinfl_ptr = g_utf8_next_char(vinfl_ptr); //skip the conjugation
      }
      tmp_vinfl_struct->conj = g_strndup(tmp_ptr, vinfl_ptr - tmp_ptr); //store the conjugation
      while (g_ascii_isspace(*vinfl_ptr) == TRUE) {
        vinfl_ptr = g_utf8_next_char(vinfl_ptr); //skip the space
      }
      tmp_ptr = vinfl_ptr;
      while (g_unichar_iswide(g_utf8_get_char(vinfl_ptr)) == TRUE) {
        vinfl_ptr = g_utf8_next_char(vinfl_ptr); //skip the inflection	
      }
      tmp_vinfl_struct->infl = g_strndup(tmp_ptr, vinfl_ptr - tmp_ptr); //store the inflection
      while (g_ascii_isspace(*vinfl_ptr) == TRUE) {
        vinfl_ptr = g_utf8_next_char(vinfl_ptr); //skip the space
      }
      tmp_vinfl_struct->type = vconj_types[atoi(vinfl_ptr)];
      vinfl_ptr =  get_eof_line(vinfl_ptr, vinfl_end);
      //printf("%s|%s|%s\n", tmp_vinfl_struct->conj, tmp_vinfl_struct->infl, tmp_vinfl_struct->type);
      tmp_list_ptr = g_slist_append(tmp_list_ptr, tmp_vinfl_struct);
      if (vinfl_list == NULL) vinfl_list = tmp_list_ptr;
      break;
    }
  } 
  verbinit_done = TRUE;
}

GList* search_verb_inflections(GjitenDicfile *dicfile, const gchar *srchstrg, GList **match) {
  GList *results = NULL;      //list of inflections
  int srchresp, roff, rlen;
  guint32 oldrespos, respos;
  int gjit_search = SRCH_START;
  GSList *tmp_list_ptr;
  struct vinfl_struct *tmp_vinfl_struct;
  gchar *deinflected, *prevresult;
  int printit = TRUE;

  tmp_list_ptr = vinfl_list;
  if (vinfl_list == NULL) { 
    printf("VINFL LIST == NULL\n");
    return;
  }

  deinflected = (gchar *) g_malloc(strlen(srchstrg) + 20);
    
  do {
    tmp_vinfl_struct = (struct vinfl_struct *) tmp_list_ptr->data;
    if (strg_end_compare(srchstrg, tmp_vinfl_struct->conj) == TRUE) {

      // create deinflected string
      strncpy(deinflected, srchstrg, strlen(srchstrg) - strlen(tmp_vinfl_struct->conj));
      strcpy(deinflected + strlen(srchstrg) - strlen(tmp_vinfl_struct->conj), 
             tmp_vinfl_struct->infl);

      oldrespos = srchpos = 0;    
      gjit_search = SRCH_START;
      prevresult = NULL;
      do { // search loop
        oldrespos = respos;
        gchar *repstr = (gchar *) g_malloc(1024);
        srchresp = search_string(gjit_search, dicfile, deinflected, &respos, &roff, &rlen, repstr);
        //g_printf("desinflected:%s, respos:%d, oldrespos:%d, roffset:%d, rlen:%d\nrepstr:%s\n", deinflected, respos, oldrespos, roff, rlen, repstr);
        if (srchresp != SRCH_OK)  {
          break;   //No more matches
        }
        if (gjit_search == SRCH_START) {
          srchpos = respos;
          gjit_search = SRCH_CONT;
        }
        srchpos++;
        if (oldrespos == respos) continue;

        printit = TRUE;

        if (is_kanji_only(repstr) == TRUE) {
          printit = FALSE;
        }
        else if (strlen(tmp_vinfl_struct->conj) == strlen(srchstrg)) 
          printit = FALSE; // don't display if conjugation is the same length as the srchstrg
        else if (get_jp_match_type(repstr, deinflected, roff) != EXACT_MATCH) 
          printit = FALSE; // Display only EXACT_MATCHes

        if (printit == TRUE) {
          
          gchar *str_inflection = (gchar*)g_new(gchar*, 2048);
          sprintf(str_inflection, 
                  "possible inflected verb of adjective: %s %s -> %s\n%s", 
		  tmp_vinfl_struct->type, 
		  tmp_vinfl_struct->conj, 
		  tmp_vinfl_struct->infl,
		  repstr);
          
          //append the deinflection string
          results = g_list_append (results, str_inflection);

	  //append the deinflected part that matched the search
	  *match = g_list_append(*match, strdup(deinflected));
	  
          word_matches++;
        }
      } while (srchresp == SRCH_OK);
        }
  } while ((tmp_list_ptr = g_slist_next(tmp_list_ptr)) != NULL);
  
  g_free(deinflected);
  
  return results;
}
