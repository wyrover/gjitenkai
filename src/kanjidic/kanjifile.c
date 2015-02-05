#include "kanjifile.h"

//TODO remove those globals
gchar *kdic_line = NULL;  /*size = KCFGNUM * KBUFSIZE */
gchar kanjiselected[2];
extern guint32 srchpos;
extern gchar *strginfo[];

char* get_line_from_dic(gunichar kanji, GjitenDicfile *kanjidic) {
  gint i;
  gint srch_resp, roff, rlen;
  gchar *repstr = g_new0(gchar, 1024);
  guint32 respos;
  gchar kanjistr[6];

  for (i = 0; i < 6; i++) kanjistr[i] = 0;
  g_unichar_to_utf8(kanji, kanjistr);
  
  gint srchpos = 0;
  srch_resp = search_string(SRCH_START, kanjidic, kanji, 
                            &respos, &roff, &rlen, repstr);

  return repstr;
}

kanjifile_entry *do_kdicline(gchar *kstr) {
  //explode this line into an array
  char ** res  = NULL;
  int n_spaces = 0, i, nb_word = 0;
  char *  p = NULL;
  p = strtok (kstr, " ");

  while (p) {
    res = realloc (res, sizeof (char*) * ++n_spaces);

    if (res == NULL)exit (-1); //memory allocation failed

    res[n_spaces-1] = p;
    p = strtok (NULL, " ");
    nb_word++;
  }

  kanjifile_entry *entry = g_new0(kanjifile_entry, 1);
  //for each words in the array, check what information it is 
  //the first word is the kanji
  i=0;
  entry->kanji = res[i];

  for(i=1;i<nb_word;i++){
    //the first character of a word indicates it's purpose
    char *word = res[i];
    char first_char = word[0];

    switch(first_char){
    case 'S':
      //Stroke number
      sscanf(word,"S%d", &entry->stroke);
      break;
    default:
      break;
    }
  }

  return entry;
}
