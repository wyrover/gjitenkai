#ifndef WORDDIC_CONSTANTS_H
#define WORDDIC_CONSTANTS_H

#include "../../config.h"

#define DEFWORDFONTNAME "-*-fixed-medium-r-normal-*-14-*-*-*-*-*-jisx0208.1983-0"
#define DEFBIGFONTNAME "-*-fixed-medium-r-normal-*-24-*-*-*-*-*-jisx0208.1983-0"

#define VINFL_FILENAME GJITENKAI_DATADIR"/vconj.utf8"
#define RADKFILE_NAME GJITENKAI_DATADIR"/radkfile.utf8"

//TODO rename to GSETTINGS
#define SETTINGS_WORDDIC "apps.gjitenkai.worddic"
#define GCONF_ROOT "/apps/gjitenkai"
#define GCONF_PATH_GJITEN GCONF_ROOT"/gjiten"
#define GCONF_PATH_WORDDIC GCONF_ROOT"/worddic"
#define GCONF_PATH_KANJIDIC  GCONF_ROOT"/kanjidic"

#endif
