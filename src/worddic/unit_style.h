#ifndef UNIT_STYLE_H
#define UNIT_STYLE_H

typedef struct unit_style_t{
  GtkTextTag *tag;
  gchar *font;
  GdkRGBA *color;
  gchar *start;
  gchar *end;
}unit_style;

#endif
