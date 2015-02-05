#include "dicutil.h"

gchar *get_eof_line(gchar *ptr, gchar *end_ptr) {
  static gchar *tmpptr; //FIXME: this is called from kanjidic and worddic!!!
  tmpptr = ptr;
  while (*tmpptr != '\n') {
    if (end_ptr == tmpptr) return NULL;
    tmpptr++;
  }
  tmpptr++;
  return tmpptr;
}

void to_utf8(gunichar c, char* utf8_c){
  int at = g_unichar_to_utf8(c, utf8_c);
  utf8_c[at] = '\0';
}

// Compares strg1 with strg2.
// If strg1 == strg3|strg2 then returns TRUE (End of strg1 matches strg2)
int strg_end_compare(gchar *strg1, gchar *strg2) {
  int i = 0;
  int matching = TRUE;
  gchar *strg1_end, *strg2_end;

  if (strlen(strg1) < strlen(strg2)) return FALSE;

  strg1_end = strg1 + strlen(strg1);
  strg2_end = strg2 + strlen(strg2);

  for (i = 0; i < g_utf8_strlen(strg2, -1); i++) {
    strg1_end = g_utf8_prev_char(strg1_end);
    strg2_end = g_utf8_prev_char(strg2_end);
    if (g_utf8_get_char(strg1_end) != g_utf8_get_char(strg2_end)) 
      matching = FALSE;
  }
  return matching;
}

gboolean is_kanji_only(gchar *line) {
  gchar *currentchar;
	gchar *line_end;

  currentchar = line;
	line_end = line + strlen(line);

  while (g_unichar_isspace(*currentchar) == FALSE) { // find first space
    if (currentchar == line_end) break;
		if (isKanjiChar(g_utf8_get_char(currentchar)) == FALSE) return FALSE;
		currentchar = g_utf8_next_char(currentchar);
  }

	return TRUE;
}

//Finds out if the result is EXACT_MATCH, START_WITH_MATCH, END_WITH_MATCH, ANY_MATCH
int get_jp_match_type(gchar *line, gchar *srchstrg, int offset) {
  int srchstrglen;

  srchstrglen = strlen(srchstrg);
  if (offset == 0) { //can be EXACT or START_WITH
    if ((*(line + srchstrglen)) == ' ') return EXACT_MATCH;
    return START_WITH_MATCH;
  }
  else { //Check for Furigana
    if (g_unichar_isalpha(g_utf8_get_char(g_utf8_prev_char(line + offset))) == FALSE) {
      if (g_unichar_isalpha(g_utf8_get_char(line + offset + srchstrglen)) == FALSE) {
				return EXACT_MATCH;
      }
      else return START_WITH_MATCH;
    }
    else { // has an alpha char before
      if (g_unichar_isalpha(g_utf8_get_char(line + offset + srchstrglen)) == FALSE)
				return END_WITH_MATCH;
    }
  }
  if ((*(line + offset + srchstrglen)) == ' ') return END_WITH_MATCH;
  return ANY_MATCH;
}

int get_word(char *dest, char *src, int size, int pos) {
  int k,j;
  
  k = pos;
  while (src[k] == ' ')  k++;
  if ( (int) (strlen(src) - 1) <= k) return(0);
  
  j = 0;
  if (src[k] == '{') {
	  while ((src[k] != '}') && (j < size))  {
		  dest[j] = src[k];
		  j++;
		  k++;
	  }
  }
  else while ((src[k] != ' ') && (j < size)) {
			dest[j] = src[k];
			j++;
			k++;
		}
	if (j == size) dest[size - 1] = 0;
	else dest[j] = 0;

  return k;
}


gboolean isJPChar(gunichar c) {
  if (isKanaChar(c) == TRUE) return TRUE;
  if (isKanjiChar(c) == TRUE) return TRUE;
  if (isOtherChar(c) == TRUE) return TRUE;
  return FALSE;
}
gboolean isKanaChar(gunichar c) {
  if (isKatakanaChar(c) == TRUE) return TRUE;
  if (isHiraganaChar(c) == TRUE) return TRUE;
  return FALSE;
}
gboolean isKatakanaChar(gunichar c) {
  if ((c >= 0x30A0) && (c <= 0x30FF)) return TRUE; // Full and half Katakana
  if ((c >= 0xFF65) && (c <= 0xFF9F)) return TRUE; // Narrow Katakana
  return FALSE;
}
gboolean isHiraganaChar(gunichar c) {
  if ((c >= 0x3040) && (c <= 0x309F)) return TRUE; // Hiragana
  return FALSE;
}
gboolean isKanjiChar(gunichar c) {
  if ((c >= 0x3300) && (c <= 0x33FF)) return TRUE; //cjk compatibility
  if ((c >= 0x3400) && (c <= 0x4DBF)) return TRUE; //cjk ext A
  if ((c >= 0x4E00) && (c <= 0x9FAF)) return TRUE; // cjk unified
  if ((c >= 0x20000) && (c <= 0x2A6DF)) return TRUE; //cjk ext B
  if ((c >= 0x2F800) && (c <= 0x2FA1F)) return TRUE;  //cjk supplement
  return FALSE;
}
gboolean isOtherChar(gunichar c) {
  if ((c >= 0x2E80) && (c <= 0x2EFF)) return TRUE;  //cjk radical
  if ((c >= 0x2F00) && (c <= 0x2FDF)) return TRUE;  //cjk kangxi radicals
  if ((c >= 0x2FF0) && (c <= 0x2FFF)) return TRUE;  //ideographic
  if ((c >= 0x3000) && (c <= 0x303F)) return TRUE;  //punctuation
  if ((c >= 0x3200) && (c <= 0x32FF)) return TRUE;  //enclosed letters
  if ((c >= 0xFE30) && (c <= 0xFE4F)) return TRUE;  //compatibility forms
  if ((c >= 0xFF00) && (c <= 0xFF64)) return TRUE;  //compatibility forms2
  if ((c >= 0xFFA0) && (c <= 0xFFEF)) return TRUE;  //compatibility forms3
  return FALSE;
}

/* Convert Hiragana -> Katakana.*/
gchar *hira2kata(gchar *hirastr) {
  gchar *hiraptr;
  gchar *kata = g_new0(gchar, strlen(hirastr) + 6);
  gchar *kataptr = kata;
  int length;

  hiraptr = hirastr;
  while (*hiraptr != 0) {
    if (isHiraganaChar(g_utf8_get_char(hiraptr)) == TRUE) {
      g_unichar_to_utf8(g_utf8_get_char(hiraptr) + 96, kataptr);
    }
    else {
      length = g_utf8_next_char(hiraptr) - hiraptr;
      strncat(kataptr, hiraptr, length);
      kataptr[length + 1] = 0;
    }
    kataptr = g_utf8_next_char(kataptr);
    hiraptr = g_utf8_next_char(hiraptr);
    if (hiraptr == NULL) break;
  } 
  return kata;
}

/* Convert Katakana to Hiragana*/
gchar *kata2hira(gchar *katastr) {
  gchar *kataptr;
  gchar *hira = g_new0(gchar, strlen(katastr) + 6);
  gchar *hiraptr = hira;
  int length;

  kataptr = katastr;
  while (*kataptr != 0) {
    if (isKatakanaChar(g_utf8_get_char(kataptr)) == TRUE) {
      g_unichar_to_utf8(g_utf8_get_char(kataptr) - 96, hiraptr);
    }
    else {
      length = g_utf8_next_char(kataptr) - kataptr;
      strncat(hiraptr, kataptr, length);
      hiraptr[length + 1] = 0;
    }
    hiraptr = g_utf8_next_char(hiraptr);
    kataptr = g_utf8_next_char(kataptr);
    if (kataptr == NULL) break;
  } 
  return hira;
}

gboolean isHiraganaString(gchar *strg) {
  gchar *hiraptr;

	hiraptr = strg;
  while (*hiraptr != 0) {
    if (isHiraganaChar(g_utf8_get_char(hiraptr)) == FALSE) return FALSE;
		hiraptr = g_utf8_next_char(hiraptr);
	}
	return TRUE;
}

gboolean isKatakanaString(gchar *strg) {
  gchar *kataptr;

	kataptr = strg;
  while (*kataptr != 0) {
    if (isKatakanaChar(g_utf8_get_char(kataptr)) == FALSE) return FALSE;
		kataptr = g_utf8_next_char(kataptr);
	}
	return TRUE;
}

gboolean detect_japanese(gchar *srchstrg){
  gchar *currchar;
  currchar = srchstrg;
  do { 
    //FIXME: this doesn't detect all Japanese
    if (g_unichar_iswide(g_utf8_get_char(currchar)) == TRUE) {
      return TRUE;
    }
  } while ((currchar = g_utf8_find_next_char(currchar, srchstrg + strlen(srchstrg))) != NULL);
  return FALSE;
}
