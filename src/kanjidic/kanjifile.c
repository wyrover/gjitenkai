#include "kanjifile.h"

char* get_line_from_dic(const gchar *kanji, GjitenDicfile *kanjidic) {
  gint roff, rlen;
  gchar *repstr = g_new0(gchar, 1024);
  guint32 respos;
  
  search_string(SRCH_START, kanjidic, kanji, 
                &respos, &roff, &rlen, repstr);

  return repstr;
}

kanjifile_entry *do_kdicline(const gchar *kstr) {  
  kanjifile_entry *entry = g_new0(kanjifile_entry, 1);
  gchar *translation;
  
  //for each words in the array, check what information it is 
  //the first word is the kanji
  gunichar utf8kanji = g_utf8_get_char(kstr);
  gchar *p_str_kanji = g_new0(gchar, sizeof(gunichar));
  g_unichar_to_utf8(utf8kanji, p_str_kanji);
  entry->kanji = (gunichar*)p_str_kanji;

  gchar **words = g_strsplit(kstr, " ", -1);

  while(words){
    gchar *word = *words;
    if(!g_strcmp0(word, "\n"))break;
    
    //the first character of a word indicates it's purpose
    char first_char = word[0];
    
    switch(first_char){
    case 'S':
      //Stroke number
      sscanf(word, "S%d", &entry->stroke);
      break;
    case 'U':
      //Unicode number
      sscanf(word, "U%d", &entry->unicode);
      break;
    case 'G':
      //Grade (jouyou)
      sscanf(word, "G%d", &entry->jouyou);
      break;
    case '{':
      translation = strdup(word+1);  //+1 to skip the { character.
        entry->translations = g_slist_append(entry->translations, translation);
      break;
    default:
      //check if katakana (onyomi) or hiragana (kunyomi)
      if (hasKatakanaString(word)){
        entry->onyomi = g_slist_append(entry->onyomi, strdup(word));
      }
      else if (hasHiraganaString(word)){
        entry->kunyomi = g_slist_append(entry->kunyomi, strdup(word));
      }
      
      break;
      }
    words++;
  }
  return entry;
}
