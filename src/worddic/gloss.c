#include "gloss.h"

void gloss_free(gloss *p_gloss){
  g_slist_free_full(p_gloss->sub_gloss, g_free);
  p_gloss->sub_gloss = NULL;
  g_slist_free_full(p_gloss->general_informations, g_free);
  p_gloss->general_informations = NULL;
}


