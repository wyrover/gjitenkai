//gcc -g parse_line.c ../src/worddic/dicentry.c ../src/worddic/gloss.c $(pkg-config --cflags --libs gtk+-3.0) -I../src/worddic/ -o parse_line
//gcc -g parse_line.c ../src/worddic/dicentry.c ../src/worddic/gloss.c ../src/common/dicutil.c $(pkg-config --cflags --libs gtk+-3.0) -I../src/worddic/ -DMINGW -o parse_line.exe
#include <gtk/gtk.h>

#include "../src/worddic/dicentry.h"
#include "../src/worddic/gloss.h"


#define LINE "１コマ;一コマ;１こま;一こま;一齣;一駒(iK) [ひとコマ(一コマ);ひとこま(一こま,一齣,一駒)] /(n) (1) one scene/one frame/one shot/one exposure/(2) one cell/one panel (comic)/EntL1162000X/"

//#define LINE "チェイサー /(n) (test) (1) Chaser/pursuer/(2) (drk) (lol) Chaser/light drink (often just water) taken after a strong alcoholic drink/"

//#define LINE "[なかぐろ] /(n) (1) middle dot (typographical symbol used between parallel terms, names in katakana, etc.)/full-stop mark at mid-character height/interpoint (interword separation)/"

//#define LINE "肩揉み [かたもみ] /(n) shoulder rub/shoulder massage/"

int main( int argc, char **argv )
{  
  gchar *line = g_strdup(LINE);
  g_printf("Parsing %s\n\n", line);
  
  GjitenDicentry* entry = parse_line(line);

  //check is line was altered
  if(strcmp(line, LINE)){
    g_printf("WARNING: the line was modified !\nwas\n%s\nis%s\n\n", LINE, line);
  }
  
  //free the line
  g_free(line);

  GList *unit = NULL;
  
  g_printf("Definition\n");
  for(unit = entry->jap_definition;
      unit != NULL;
      unit = unit->next){
    g_printf("\t%s\n", unit->data);
  }

  g_printf("Reading (optional): \n");
  if(!entry->jap_reading)g_printf("\t<NO READING>\n");
  for(unit = entry->jap_reading;
      unit != NULL;
      unit = unit->next){
    g_printf("\t%s\n", unit->data);
  }

  g_printf("Entry General Informations: \n");
  for(unit = entry->general_informations;
      unit != NULL;
      unit = unit->next){
    g_printf("\t%s\n", unit->data);
  }

  for(unit = entry->gloss;
      unit != NULL;
      unit = unit->next){
    g_printf("GLOSS\n");
    
    gloss *p_gloss = unit->data;
    g_printf("\tGeneral Informations: \n");
    GSList *GI;
    for(GI = p_gloss->general_informations;
        GI  != NULL;
        GI  = GI->next){
      gchar *text = GI->data;
      g_printf("\t\t(%s)\n", text);
    }

    g_printf("\tSub gloss: \n");
    GSList * sub_gloss =  NULL;
    for(sub_gloss = p_gloss->sub_gloss;
        sub_gloss  != NULL;
        sub_gloss  = sub_gloss->next){
      g_printf("\t\t%s\n", sub_gloss->data);
    }
  }

  //free the entry
  dicentry_free(entry);
  
  return 1;
}
