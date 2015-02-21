#include "kanjiutils.h"

GList* load_radkfile(GHashTable **pp_rad_info_hash, 
                   GHashTable **pp_kanji_info_hash,
                   GList      *rad_info_list) {
  int error = FALSE;
  struct stat radk_stat;
  gint rad_cnt = 0;
  gchar *radkfile_name = RADKFILE_NAME;
  gchar *radkfile_ptr;
  gchar *radkfile_end;
  RadInfo *rad_info = NULL;
  KanjiInfo *kanji_info;

  gchar *radkfile = NULL;
  guint32 radkfile_size;
  int radkfile_fd = -1;

  GHashTable *rad_info_hash   = *pp_rad_info_hash;
  GHashTable *kanji_info_hash = *pp_kanji_info_hash;
  //  GList      *rad_info_list   = *pp_rad_info_list;

  //is the radkfile present ? 
  if (stat(radkfile_name, &radk_stat) != 0) {
    g_error("**ERROR** radkfile: stat() \n");
    error = TRUE;
  }

  //open radkfile and get the content
  radkfile_size = radk_stat.st_size;
  radkfile_fd = open(radkfile_name, O_RDONLY);
  if (radkfile_fd == -1) {
    g_error("**ERROR** radkfile: open()\n");
    error = TRUE;
  }
  radkfile = (gchar *) mmap(NULL, radkfile_size, PROT_READ, MAP_SHARED, radkfile_fd, 0);
  if (radkfile == NULL) gjiten_abort_with_msg("mmap() failed for radkfile\n");
  if ( radkfile_fd != -1 ) {
    close(radkfile_fd);
  }
  radkfile_fd = -1;
  
  if (error == TRUE) {
    gjiten_print_error("Error opening %s.\n "                           \
                       "Check your preferences or read the documentation!",
                       radkfile_name);
    return;
  }

  radkfile_end = radkfile + strlen(radkfile); //FIXME: lseek
  radkfile_ptr = radkfile;
    
  //read the content of the file
  while((radkfile_ptr < radkfile_end) && (radkfile_ptr != NULL)) {

    //if comment (first char on this line is #), skip this line
    if (*radkfile_ptr == '#') {  
      radkfile_ptr = get_eof_line(radkfile_ptr, radkfile_end); 
      continue;
    }
    
    //if radical info line (first char on this line is $)
    if (*radkfile_ptr == '$') {

      rad_cnt++;          //Increase number of radicals found
      radkfile_ptr = g_utf8_next_char(radkfile_ptr);

      //move the pointer forward until the character is wide (kanji)
      while (g_unichar_iswide(g_utf8_get_char(radkfile_ptr)) == FALSE) {
        radkfile_ptr = g_utf8_next_char(radkfile_ptr);
      }
      
      //new rad_info to be stored in the rad_info_hash and rad_info_list
      rad_info = g_new0(RadInfo, 1);
      rad_info->kanji_info_list = NULL;
      rad_info_list = g_list_prepend(rad_info_list, rad_info);

      //store radical character
      //the characters in the file are in UTF8 format. We need unicode.  
      gunichar utf8radical = g_utf8_get_char(radkfile_ptr);
      gunichar *p_str_radical = g_new0(gunichar, 1);
      g_unichar_to_utf8(utf8radical, p_str_radical);
      rad_info->radical = p_str_radical;
      
      //g_printf("(%s)", rad_info->radical);
      
      //Find stroke number (move until digit detected)
      while (g_ascii_isdigit(*radkfile_ptr) == FALSE) {
        radkfile_ptr = g_utf8_next_char(radkfile_ptr);
      }

      //Store the stroke number
      rad_info->strokes = atoi(radkfile_ptr);  

      //insert this radical as key and the info as value
      g_hash_table_insert(rad_info_hash, rad_info->radical, rad_info);
      
      //Goto next line
      radkfile_ptr = get_eof_line(radkfile_ptr, radkfile_end);
    }
    else {
      //search the kanji to be stored in the list of the kanji key / radical info list
      //the kanji are located between radical $ markers and the radical info
      while ((*radkfile_ptr != '$') && (radkfile_ptr < radkfile_end)) {
        if (*radkfile_ptr == '\n') {
          radkfile_ptr++;
          continue;
        }

        gunichar utf8kanji = g_utf8_get_char(radkfile_ptr);

        gchar *kanji = g_new0(gchar, 6);
        g_unichar_to_utf8(utf8kanji, kanji);
        
        //search in the kanji infohash if this kanji is alderly present, 
        //if not, create a new kanji and add it
        kanji_info = g_hash_table_lookup(kanji_info_hash, kanji);
        if (kanji_info == NULL) {
          kanji_info = g_new0(KanjiInfo, 1);
          kanji_info->rad_info_list = NULL;
          kanji_info->kanji = kanji;

          //insert this kanji as a key and the kanji info as value
          g_hash_table_insert(kanji_info_hash, (gpointer) kanji, (gpointer) kanji_info);
        }

        //add the kanji and the radical info in their respective lists
        kanji_info->rad_info_list = g_list_prepend(kanji_info->rad_info_list, rad_info);
        rad_info->kanji_info_list = g_list_prepend(rad_info->kanji_info_list, kanji_info);

        //navigate to next character
        radkfile_ptr = g_utf8_next_char(radkfile_ptr);
      }
    }
  }

  return rad_info_list;
}

GList* get_radical_of_kanji(gunichar kanji, GHashTable *kanji_info_hash) {
  GList *kanji_info_list = NULL;
  GList *radical_list = NULL; //list of radical to be returned

  //convert to UTF8
  gchar utf8kanji[3];
  int at = g_unichar_to_utf8(kanji, utf8kanji);
  utf8kanji[at] = '\0';

  //lookup in the kanji info hash and get the kanji info
  KanjiInfo *kanji_info = g_hash_table_lookup(kanji_info_hash, utf8kanji);

  if(kanji_info == NULL){
    g_printf("no radical found\n");
    return NULL;
  }
  
  //puts the radical field of the kanji_info into a list
  for (kanji_info_list = kanji_info->rad_info_list;
       kanji_info_list != NULL;
       kanji_info_list = kanji_info_list->next) {
    radical_list = g_list_prepend(radical_list, 
                                        (gpointer) ((RadInfo *) kanji_info_list->data)->radical
                                        );
  }
  
  return radical_list;
}

GList* get_kanji_by_key(gchar *srchkey, GList *list, GjitenDicfile *dicfile)  {
  gint srch_resp = 0, roff = 0, rlen = 0;
  gchar repstr[1024];
  guint32 respos, oldrespos; 
  gint loopnum = 0;
  gint srchpos = 0;

  srch_resp = search_string(SRCH_START, dicfile, srchkey, &respos, &roff, &rlen, repstr);
  //g_printf("F: Returning:srch_resp:%d\n respos:%ld\n roff:%d rlen:%d\n repstr:%s\n", srch_resp,respos,roff,rlen,repstr); 
  if (srch_resp != SRCH_OK) return;
  oldrespos = srchpos = respos;
  
  char *repstr2 = strdup(repstr);
  kanjifile_entry* entry = do_kdicline(repstr2);
  list = g_list_prepend(list, entry->kanji);
  
  while (roff != 0) {
    oldrespos = respos;
    srchpos++;
    loopnum++;
    srch_resp = search_string(SRCH_CONT, dicfile, srchkey, &respos, &roff, &rlen, repstr);
    //g_printf("srch_resp:%d\n respos:%ld\n roff:%d rlen:%d\n repstr:%s\n",srch_resp,respos,roff,rlen,repstr);
    if (srch_resp != SRCH_OK) break;
    if (oldrespos == respos) continue;

    char *repstr2 = strdup(repstr);
    kanjifile_entry* entry = do_kdicline(repstr2);
    list = g_list_prepend(list, entry->kanji);
  }

  return list;
}

GList* get_kanji_by_stroke(int stroke, int plusmin, GList *list, GjitenDicfile *dicfile) {
  static char srchkey[10];
  int i, lowerlim, upperlim;
 
  upperlim = stroke + plusmin;
  if (upperlim > 30) upperlim = 30;
  lowerlim = stroke - plusmin;
  if (lowerlim < 1) lowerlim = 1;
 
  //find the kanji by key, with the 'S' key (strokes)
  for (i = lowerlim; i <= upperlim ; i++) {
    snprintf(srchkey, 10, " S%d ", i);
    list = get_kanji_by_key(srchkey, list, dicfile);
  }
  
  return list;
}

GList* get_kanji_by_radical(gchar *radstrg, GHashTable *rad_info_hash) { 
  gint radnum;                   //number of character in radstrg
  RadInfo *rad_info;             
  GList *kanji_info_list = NULL;
  GList *result = NULL;          //list of matched kanji to return
  gchar *radstrg_ptr;            //pointer to browse radstrg

  radnum = g_utf8_strlen(radstrg, -1); 
  if (radnum == 0) return;  //no character in radstrg

  //to navigate in the string
  int i;
  radstrg_ptr = radstrg;

  //for every characters in the string
  for(i=0;i<radnum; i++){

    //get the radical in utf8 format
    gunichar uniradical = g_utf8_get_char(radstrg_ptr);
    gchar radical[3];
    int at = g_unichar_to_utf8(uniradical, radical);
    radical[at] = '\0';
  
    //lookup the radical (key) and get the radical info (value)
    rad_info = g_hash_table_lookup(rad_info_hash, radical);

    if (rad_info == NULL) {
      g_printf("unknown radical %s\n", radical);
    }
    else{
    
      //contains all the kanji of the current radical
      GList *radical_kanji_list = NULL;

      //add all the kanji from the radical info list to the tmp list
      for (kanji_info_list = rad_info->kanji_info_list;
           kanji_info_list != NULL;
           kanji_info_list = kanji_info_list->next) {
        radical_kanji_list = g_list_prepend(radical_kanji_list, 
                                            (gpointer) ((KanjiInfo *) kanji_info_list->data)->kanji
                                            );
      }
    
      //if the result list is empty (first iteration)
      if(result == NULL){
        //the result is all the kanji for current radical
        result = radical_kanji_list;
      }
      else{
        //the result list is not empty, remove the kanji in result that are not
        //in the radical_kanji_list
        result = list_merge(result, radical_kanji_list);
      }
    }

    radstrg_ptr = g_utf8_next_char(radstrg_ptr);
  }

  return result;
}

GList* list_merge(GList *list_a, GList *list_b) {
  GList *ptr1, *ptr2, *nextptr;
  int found;

  if(!list_a)return list_b;
  if(!list_b)return list_a;

  ptr1 = list_a;
  while (ptr1 != NULL) {
    nextptr = g_list_next(ptr1);
    found = FALSE;
    ptr2 = list_b;
    while (ptr2 != NULL) {
      if ((gunichar) ptr1->data == (gunichar) ptr2->data) {
        found = TRUE;
        break;
      }
      ptr2 = g_list_next(ptr2);
    }
    if (found == FALSE) {
      list_a = g_list_remove(list_a, ptr1->data);
    }
    ptr1 = nextptr;
  }
  g_list_free(list_b);
  list_b = NULL;

  return list_a;
}

GList* list_merge_str(GList *list_a, GList *list_b) {
  GList *ptr1, *ptr2, *nextptr;
  int found;

  if(!list_a)return list_b;
  if(!list_b)return list_a;

  ptr1 = list_a;
  while (ptr1 != NULL) {
    nextptr = g_list_next(ptr1);
    found = FALSE;
    ptr2 = list_b;
    while (ptr2 != NULL) {
      if (!strcmp(ptr1->data, ptr2->data)) {
        found = TRUE;
        break;
      }
      ptr2 = g_list_next(ptr2);
    }
    if (found == FALSE) {
      list_a = g_list_remove(list_a, ptr1->data);
    }
    ptr1 = nextptr;
  }
  g_list_free(list_b);
  list_b = NULL;

  return list_a;
}
