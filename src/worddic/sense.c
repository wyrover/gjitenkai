#include "sense.h"

void sense_free(sense *p_sense){
  g_slist_free_full(p_sense->sub_sense, g_free);
  p_sense->sub_sense = NULL;
  g_slist_free_full(p_sense->general_informations, g_free);
  p_sense->general_informations = NULL;
  g_free(p_sense);
}
