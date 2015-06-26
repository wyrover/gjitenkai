#include "dicresult.h"

dicresult_free_match(dicresult* p_dicresult){
  g_free(p_dicresult->match);
}

