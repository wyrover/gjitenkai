#include "dicentry.h"

GjitenDicentry* parse_line(gchar* line){
  gchar *line_cpy = strdup(line);
  GjitenDicentry* dicentry = g_new0 (GjitenDicentry, 1);

  dicentry->jap_definition = g_strdup_printf("%s", strtok(line_cpy, " "));
  dicentry->jap_reading = g_strdup_printf("%s", strtok(NULL, " "));

  g_free(line_cpy);
  
  return dicentry;
}
