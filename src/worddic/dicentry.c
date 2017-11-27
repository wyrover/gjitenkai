#include "dicentry.h"

void dicentry_set_GI_flags_from_code(GjitenDicentry* dicentry){
  dicentry->GI = GINONE;  //set all GI Flags to 0

  GSList *unit = NULL;

  for(unit = dicentry->general_informations;
      unit != NULL;
      unit = unit->next){
    gchar *GI_text_code = (gchar*)unit->data;

    if(!strcmp(GI_text_code, "v1")){
      dicentry->GI = V1;
    }
    else if(!strcmp(GI_text_code, "n")){
      dicentry->GI = NOUN;
    }
    else if(g_str_has_prefix(GI_text_code, "v5")){
      dicentry->GI = V5;
    }
    else if(!strcmp(GI_text_code, "adj-i")){
      dicentry->GI = ADJI;
    }
  }
}

void dicentry_free(GjitenDicentry* dicentry){
  g_slist_free_full(dicentry->sense, (GDestroyNotify)sense_free);
  dicentry->sense = NULL;

  g_slist_free_full(dicentry->jap_definition, g_free);
  dicentry->jap_definition = NULL;

  g_slist_free_full(dicentry->jap_reading, g_free);
  dicentry->jap_reading = NULL;

  g_slist_free_full(dicentry->general_informations, g_free);
  dicentry->general_informations = NULL;

  g_free(dicentry->ent_seq);
  dicentry->ent_seq = NULL;

  g_free(dicentry);
}
