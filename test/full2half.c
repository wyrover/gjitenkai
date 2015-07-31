//gcc full2half.c -I../src/common/ ../src/common/dicutil.c $(pkg-config --cflags --libs gtk+-3.0) -o full2half
#include <gtk/gtk.h>

#include "../src/common/dicutil.h"

int main( int argc, char **argv )
{
  //gchar * full = "？";
  //gchar * full = "！";
  //gchar * full = "）";
  gchar * full = "｛";
  gchar *half = full2half(full);
  g_printf("%s -> %s\n", full, half);
  if(!g_strcmp0(full, half)){g_printf("warning: unmodified result\n");}
  g_free(half);
  return 1;
}

