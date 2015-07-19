#ifndef WORDDIC_CONSTANTS_H
#define WORDDIC_CONSTANTS_H

#include "../../config.h"

#ifdef MINGW
	#define VINFL_FILENAME path_relative("..\\share\\data\\vconj.utf8")
	#define RADKFILE_NAME path_relative("..\\share\\data\\radkfile.utf8")
#else
	#define VINFL_FILENAME strdup(GJITENKAI_DATADIR"/vconj.utf8")
	#define RADKFILE_NAME strdup(GJITENKAI_DATADIR"/radkfile.utf8")
#endif


#define SETTINGS_KANJIDIC "gjitenkai.kanjidic"
#define GSETTINGS_ROOT "/gjitenkai"
#define GSETTINGS_PATH_GJITEN GCONF_ROOT"/gjiten"
#define GSETTINGS_PATH_WORDDIC GCONF_ROOT"/worddic"
#define GSETTINGS_PATH_KANJIDIC  GCONF_ROOT"/kanjidic"

#endif
