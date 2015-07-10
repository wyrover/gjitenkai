#include "gloss.h"

void gloss_free(gloss *gloss){
  g_slist_free_full(gloss->sub_gloss, g_free);
  g_slist_free_full(gloss->general_informations, g_free);
  g_free(gloss);
}


