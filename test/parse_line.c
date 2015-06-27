//gcc parse_line.c ../src/worddic/dicentry.c $(pkg-config --cflags --libs gtk+-3.0) -I../src/worddic/ -o parse_line
#include <gtk/gtk.h>

#include "../src/worddic/dicentry.h"


#define LINE "チェイサー  /(n) (1) chaser/pursuer/(2) chaser/light drink (often just water) taken after a strong alcoholic drink/"

int main( int argc, char **argv )
{

  //const gchar *line = "凜 [りん] /(adj-t,adv-to) (1) cold/frigid/bracing/(2) dignified/";
  
  const gchar const *line = LINE;

  //const gchar *line = "強盗返し [がんとうがえし] /(oK) (n) rotating stage machinery, which rolls backwards 90 degrees to reveal the next scene/";
  
  GjitenDicentry* entry = parse_line(line);

  g_printf("KANJI\n");
  GList *d = NULL;
  for(d = entry->jap_definition;
      d != NULL;
      d = d->next){
    g_printf("%s\n", d->data);
    
    }

  g_printf("KANA\n");

  for(d = entry->jap_reading;
      d != NULL;
      d = d->next){
    g_printf("%s\n", d->data);
    
    }

  g_printf("GLOSS\n");
  

  for(d = entry->gloss;
      d != NULL;
      d = d->next){
    g_printf("%s\n", d->data);
    
    }
  

  g_printf("original line was %s\n", line);

  if(strcmp(line, LINE)){
    g_printf("WARNING: the line was modified\n");
  }
  
  return 1;
}
