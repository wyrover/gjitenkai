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


/**
   treeview to pass to the thread callback function to update the UI
 */
typedef struct dic_state_ui_t{
  GtkCellRendererToggle *cell;
  GtkTreeView *treeview;
}dic_state_ui;

gpointer proxy_worddic_dicfile_parse_all(WorddicDicfile *dicfile);
static gboolean cb_load_dic_timeout( dic_state_ui *ui );
 
void init_prefs_window (worddic * );

#endif
