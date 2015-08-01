#ifndef WORDDIC_CONSTANTS_H
#define WORDDIC_CONSTANTS_H

#include "../../config.h"

#ifdef MINGW
	#define RADKFILE_NAME path_relative("..\\share\\gjitenkai\\radkfile.utf8")
#else
	#define RADKFILE_NAME strdup(GJITENKAI_DATADIR"/radkfile.utf8")
#endif

#define SETTINGS_KANJIDIC "gjitenkai.kanjidic"
#define GSETTINGS_ROOT "/gjitenkai"
#define GSETTINGS_PATH_GJITEN GSETTINGS_ROOT"/gjiten"
#define GSETTINGS_PATH_WORDDIC GSETTINGS_ROOT"/worddic"
#define GSETTINGS_PATH_KANJIDIC  GSETTINGS_ROOT"/kanjidic"

#endif
