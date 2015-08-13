#include "inflection.h"

void init_inflection() {
  vinfl_list=NULL;
  gchar *tmp_ptr;
  gchar *vinfl_ptr, *vinfl_end;
  int vinfl_part = 1;
  int conj_type = 40;
  struct vinfl_struct *tmp_vinfl_struct;

  gchar filename[PATH_MAX];
  GET_FILE(GJITENKAI_DATADIR"/gjitenkai/"VINFL_FILENAME, filename);

  vinfl_start = NULL;
  vinfl_start = read_file(filename);

  if(!vinfl_start){
    g_printf("cannot load verbe inflection file '%s'\n",  filename);
  }
  
  vinfl_end = vinfl_start + strlen(vinfl_start);
  vinfl_ptr = vinfl_start;

  vinfl_part = 1;
  while ((vinfl_ptr < vinfl_end) && (vinfl_ptr != NULL)) {
    if (*vinfl_ptr == '#') {                          //ignore comments
      vinfl_ptr = get_EOL(vinfl_ptr, vinfl_end); //Goto next line
      continue;
    }
    if (*vinfl_ptr == '$') vinfl_part = 2;

    //before or after $
    switch (vinfl_part) {
    case 1:
      if (g_ascii_isdigit(*vinfl_ptr) == TRUE) { //Conjugation numbers
        conj_type = atoi(vinfl_ptr);
        if ((conj_type < 0) || (conj_type > VCONJ_TYPE_MAX)) break;

        //skip the number
        while (g_ascii_isdigit(*vinfl_ptr) == TRUE)
          vinfl_ptr = g_utf8_next_char(vinfl_ptr);
        //skip the space
        while (g_ascii_isspace(*vinfl_ptr) == TRUE)
          vinfl_ptr = g_utf8_next_char(vinfl_ptr);
        
        // beginning of conjugation definition;
        tmp_ptr = vinfl_ptr;

        //find end of line
        vinfl_ptr = get_EOL(vinfl_ptr, vinfl_end);
        vconj_types[conj_type] = g_strndup(tmp_ptr, vinfl_ptr - tmp_ptr -1);  
      }
      break;
    case 2:
      if (g_unichar_iswide(g_utf8_get_char(vinfl_ptr)) == FALSE) {
        vinfl_ptr =  get_EOL(vinfl_ptr, vinfl_end);
        break;
      }
      tmp_vinfl_struct = g_new0 (struct vinfl_struct, 1);
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
      tmp_vinfl_struct->itype = atoi(vinfl_ptr);
      vinfl_ptr =  get_EOL(vinfl_ptr, vinfl_end);

      //push the tmp vinfl structure in the list
      vinfl_list = g_slist_prepend(vinfl_list, tmp_vinfl_struct);
      break;
    }
  }
}

GList* search_inflections(WorddicDicfile *dicfile,
                          const gchar *srchstrg) {
  //list to return
  GList *results = NULL;

  //remember previous searches to avoid duplicates
  GSList *previous_search = NULL;

  //declare a search expression variable
  search_expression search_expr;
  search_expr.search_criteria_jp = EXACT_MATCH; //inflection needs to be exact
  search_expr.search_criteria_lat = ANY_MATCH;  //latin search is irrelevent

  //for all the inflections
  GSList *vinfl_list_browser = NULL;
  for(vinfl_list_browser = vinfl_list;
      vinfl_list_browser != NULL;
      vinfl_list_browser = g_slist_next(vinfl_list_browser)){

    struct vinfl_struct * tmp_vinfl_struct = NULL;
    tmp_vinfl_struct = (struct vinfl_struct *) vinfl_list_browser->data;
    
    //if the inflected conjugaison match the end of the string to search
    if(!g_str_has_suffix(srchstrg, tmp_vinfl_struct->conj)){continue;}

    //create a new GString to modify
    GString *deinflected = g_string_new(NULL);
    
    // create deinflected string with the searched expression
    deinflected = g_string_append(deinflected, srchstrg);

    //replace the inflection by the conjonction
    gint radical_pos = strlen(srchstrg) - strlen(tmp_vinfl_struct->conj);
    deinflected = g_string_truncate (deinflected, radical_pos);
    deinflected = g_string_append(deinflected, tmp_vinfl_struct->infl);

    //check if deinflected was previously searched
    GSList *l=NULL;
    for(l=previous_search;
        l != NULL;
        l = l->next){
      if(!strcmp(l->data, deinflected->str)){
        //free memory
        g_string_free(deinflected, TRUE);
        deinflected = NULL;
        break;
      }
    }
    
    //deinflected has been freed because the same string existed in a previous
    //search. skip this iteration
    if(!deinflected)continue;
    
    //comment that explains which inflection was searched
    gchar *comment = g_strdup_printf("%s %s -> %s",
                                     tmp_vinfl_struct->type,
                                     tmp_vinfl_struct->conj,
                                     tmp_vinfl_struct->infl);

    //if the inflection type is from an adj-i, only search for adj-i
    //if not adji-i, assume it's a verbe 
    enum entry_GI entry_type;
    if(tmp_vinfl_struct->itype == ADJ_TO_ADVERB ||
       tmp_vinfl_struct->itype == ADJ_PAST ||
       tmp_vinfl_struct->itype == ADJ_NEGATIVE_PAST ||
       tmp_vinfl_struct->itype == ADJ_NEGATIVE_PAST ||
       tmp_vinfl_struct->itype == ADJ_PAST_KATTA){
      entry_type = ADJI;
    }
    else{
      entry_type = SET_VERBE;
    }
    
    //search in the dictionary
    search_expr.search_text = deinflected->str;
    GList *results_infl = dicfile_search(dicfile,
                                         &search_expr,
                                         comment,
                                         entry_type,
                                         1);

    results = g_list_concat(results, results_infl);

    // add the string to history
    previous_search = g_slist_append(previous_search, deinflected->str);
    
    //free memory
    g_free(comment);
    
    // str is still needed in previous_search 
    g_string_free(deinflected, FALSE);
  }

  //free history
  g_slist_free_full(previous_search, g_free);
  
  return results;
}

void free_inflection(){
  g_free(vinfl_start);
  gint i;
  for(i=0;i<VCONJ_TYPE_MAX;i++){
    g_free(vconj_types[i]);
  }
}

void free_vinfl(struct vinfl_struct *vinfl){
  g_free(vinfl->conj);
  vinfl->conj = NULL;
  g_free(vinfl->infl);
  vinfl->infl = NULL;
  g_free(vinfl->type);
  vinfl->type = NULL;
  g_free(vinfl);
}
