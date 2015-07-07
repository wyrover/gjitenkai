#include "dicfile.h"

int dicfile_load(GjitenDicfile* dicfile, GjitenDicfile *mmaped_dicfile){
  //if the dictionary is not initialized, init: open a file descriptor
  if (dicfile->status == DICFILE_NOT_INITIALIZED) {
    if (dicfile_init(dicfile) == FALSE) return SRCH_FAIL; 
  }
  if (dicfile->status != DICFILE_OK) return SRCH_FAIL;

  //if the mapped dictionary is not the requested dictionnary then clear the
  //previously mapped dictionary
  if ((dicfile != mmaped_dicfile) && (mmaped_dicfile != NULL)) {
    g_printf("free mem of previously used dicfile\n");
    dicutil_unload_dic(mmaped_dicfile);
  }
  
  
  //if no mapped dictionary, load into memory from the dic's file descriptor
  if (mmaped_dicfile == NULL) {
    g_printf("load dicfile %s %s into memory\n", dicfile->name, dicfile->path);
    mmaped_dicfile = dicfile;
    dicfile->mem = read_file(dicfile->path);

    if (dicfile->mem == NULL) gjiten_abort_with_msg("mmap() failed\n");
    mmaped_dicfile = dicfile;
    }
}


void dicutil_unload_dic(GjitenDicfile *dicfile) {
  if (dicfile != NULL) {
    //free mem of previously used dicfile	
    #ifdef USE_MMAP
    munmap(dicfile->mem, dicfile->size);
    #else
    free(dicfile->mem);
    #endif

    dicfile->mem = NULL;
    dicfile = NULL;
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
      gjiten_print_error(_("Error opening dictfile:  %s\nCheck your preferences. "),
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
