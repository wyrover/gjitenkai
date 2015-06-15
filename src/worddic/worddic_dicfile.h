#ifndef WORDDIC_DICFILE_H
#define WORDDIC_DICFILE_H

#include "dicentry.h"
#include "../common/dicfile.h"

GList *dicfile_search_regex(GjitenDicfile *dicfile,
			    const gchar *srchstrg_regex,
			    GList **matched_part);

GList *dicfile_search(GjitenDicfile *dicfile, const gchar *srchstrg, 
                      gint match_criteria_jp, gint match_criteria_lat, 
                      gint match_type);

#endif
