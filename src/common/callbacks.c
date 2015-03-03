#include <gtk/gtk.h>

/*
  Callback function called when a character is inserted in the entry widget
  check if the entered character is equals to the unicode character of a special
  character (CARRIAGE RETURN, BACKSPACE, DELETE), if so prevent these characters 
  to be added in the entry box (happend when using SCIM-Anthy with GTK3)
*/
G_MODULE_EXPORT void on_text_insert_detect_unicode (GtkEntry    *entry,
                                    const gchar *text,
                                    gint         length,
                                    gint        *position,
                                    gpointer     data)
{
  GtkEditable *editable = GTK_EDITABLE(entry);

  //unicode representation of special characters
  char carriage_return[] = "\x0D";
  char backspace[] = "\x08";
  char delete[] = "\x7f";

  if(!g_strcmp0(text, carriage_return)){
    //unicode character #u0D (CARRIAGE RETURN) detected
    //do not display this character in the entry widget
    g_signal_stop_emission_by_name (G_OBJECT (editable), "insert_text");

    //emit the activate signal for the entry widget
    g_signal_emit_by_name(editable, "activate");
  }
  else if(!g_strcmp0(text, backspace)){
    //unicode character #u08 (BACKSPACE) detected
    //do not display this character in the entry widget
    g_signal_stop_emission_by_name (G_OBJECT (editable), "insert_text");

    //emit the backspace signal for the entry widget 
    //and adjust the cursor position
    g_signal_emit_by_name(editable, "backspace");
    (*position)--;
  }
  else if(!g_strcmp0(text, delete)){
    //unicode character #u7f (DELETE) detected
    //do not display this character in the entry widget
    g_signal_stop_emission_by_name (G_OBJECT (editable), "insert_text");

    //emit the delete signal of one character for the entry widget
    g_signal_emit_by_name(editable, "delete-from-cursor", GTK_DELETE_CHARS, 1);
  }
}
