#include "inflection.h"

void init_inflection() {

  gchar *vconj_types[VCONJ_TYPE_MAX];
  
  vinfl_list=NULL;
  gchar *tmp_ptr;
  gchar *vinfl_start, *vinfl_ptr, *vinfl_end;
  int vinfl_part = 1;
  int conj_type = 40;
  struct vinfl_struct *tmp_vinfl_struct;
  GSList *tmp_list_ptr = NULL;

  gchar *vconj = VINFL_FILENAME;
  vinfl_start = read_file(vconj);
  
#ifdef MINGW
  g_free(vconj);
#endif

  vinfl_end = vinfl_start + strlen(vinfl_start);
  vinfl_ptr = vinfl_start;

  vinfl_part = 1;
  while ((vinfl_ptr < vinfl_end) && (vinfl_ptr != NULL)) {
    if (*vinfl_ptr == '#') {                          //ignore comments
      vinfl_ptr = get_eof_line(vinfl_ptr, vinfl_end); //Goto next line
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
        vinfl_ptr = get_eof_line(vinfl_ptr, vinfl_end);
        vconj_types[conj_type] = g_strndup(tmp_ptr, vinfl_ptr - tmp_ptr -1);
  
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
      tmp_vinfl_struct->itype = atoi(vinfl_ptr);
      vinfl_ptr =  get_eof_line(vinfl_ptr, vinfl_end);
  
      tmp_list_ptr = g_slist_append(tmp_list_ptr, tmp_vinfl_struct);
      if (vinfl_list == NULL) vinfl_list = tmp_list_ptr;
      break;
    }
  }
}

GList* search_inflections(WorddicDicfile *dicfile,
                          const gchar *srchstrg) {
  GList *results = NULL;
    
  //for all the inflections
  GSList *vinfl_list_browser = NULL;
  for(vinfl_list_browser = vinfl_list;
      vinfl_list_browser != NULL;
      vinfl_list_browser = g_slist_next(vinfl_list_browser)){

    GString *deinflected = g_string_new(NULL);
 
    struct vinfl_struct * tmp_vinfl_struct = NULL;
    tmp_vinfl_struct = (struct vinfl_struct *) vinfl_list_browser->data;

    //if the inflected conjugaison match the end of the string to search
    if(!g_str_has_suffix(srchstrg, tmp_vinfl_struct->conj)){continue;}

    // create deinflected string with the searched expression
    deinflected = g_string_append(deinflected, srchstrg);

    //replace the inflection by the conjonction
    gint radical_pos = strlen(srchstrg) - strlen(tmp_vinfl_struct->conj);
    deinflected = g_string_truncate (deinflected, radical_pos);
    deinflected = g_string_append(deinflected, tmp_vinfl_struct->infl);
 
    //search exact
    deinflected = g_string_append_c(deinflected, '$');
    deinflected = g_string_prepend_c(deinflected, '^');

    gchar *comment = g_strdup_printf("%s %s -> %s",
                                     tmp_vinfl_struct->type,
                                     tmp_vinfl_struct->conj,
                                     tmp_vinfl_struct->infl);
    
    GList *results_infl = dicfile_search(dicfile,
                                         deinflected->str,
                                         comment);

    results = g_list_concat(results, results_infl);

    g_string_free(deinflected, TRUE);
  }
  return results;
}
