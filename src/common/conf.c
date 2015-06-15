#include "conf.h"

GSettings* conf_init_handler(const gchar* schema_id) {
  GSettings* settings=NULL;
  
#if !GLIB_CHECK_VERSION(2, 32, 0)
  g_type_init();
#endif

  if (settings == NULL) {
    settings = g_settings_new(schema_id);
  }
  return settings;
}

void conf_close_handler(GSettings* settings) {
  if (settings != NULL) {
    g_object_unref(G_OBJECT(settings));
    settings = NULL;
  }
}

