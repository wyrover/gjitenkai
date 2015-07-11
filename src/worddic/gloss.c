#include "gloss.h"

void gloss_free(gloss *gloss){
  g_slist_free_full(gloss->sub_gloss, g_free);
  gloss->sub_gloss = NULL;
  g_slist_free_full(gloss->general_informations, g_free);
  gloss->general_informations = NULL;
  g_free(gloss);
}


