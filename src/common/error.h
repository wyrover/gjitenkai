#ifndef ERROR_H
#define ERROR_H

#include <gtk/gtk.h>

#ifdef DEBUG_GJITEN
#define GJITEN_DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else 
#define GJITEN_DEBUG(...)
#endif

int gjiten_print_error(const char *fmt, ... );
void gjiten_print_error_and_wait(const char *fmt, ... );
gboolean gjiten_print_question(const char *fmt, ... );
void gjiten_add_errormsg(gchar *msg);
void gjiten_flush_errors();
void gjiten_abort_with_msg(const char *fmt, ... );

#endif
