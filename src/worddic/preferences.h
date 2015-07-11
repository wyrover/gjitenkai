#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib-object.h>

#include "worddic.h"
#include "../common/dicfile.h"

enum
{
  COL_NAME = 0,
  COL_PATH,
  COL_ACTIVE,
  COL_LOADED
};


/**
   preference window related functions: 
   initialisation and callbacks
 */

void init_prefs_window (worddic * );

#endif
