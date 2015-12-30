#include "print_entry.h"

void print_entry(GjitenDicentry* entry){
  GSList *unit = NULL;
  
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
  g_printf("General Information ENUM TYPE :\n\t%d\n", entry->GI);

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
}
