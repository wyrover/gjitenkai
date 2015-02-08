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
  char word[KBUFSIZE];
  gint pos=sizeof(gunichar);
  //g_printf("%s\n", kstr);
  
  kanjifile_entry *entry = g_new0(kanjifile_entry, 1);
  gchar *translation;

  //for each words in the array, check what information it is 
  //the first word is the kanji  
  //entry->kanji = g_utf8_get_char(word);

  while(pos = get_word(word, kstr, sizeof(word), pos)){
    //g_printf("%d\n", pos);
  
    //the first character of a word indicates it's purpose
    char first_char = word[0];
    
    switch(first_char){
    case 'S':
      //Stroke number
      sscanf(word,"S%d", &entry->stroke);
      break;
    case '{':
      translation = strdup(word+1);  //+1 to skip the { character. todo Free me 
      entry->translations = g_list_append(entry->translations, translation);
      break;
    default:
      break;
      }
  }
    
  return entry;
}
