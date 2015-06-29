#include "dicresult.h"

dicresult_free(dicresult* p_dicresult){
  g_free(p_dicresult->match);
  g_free(p_dicresult->comment);
}

