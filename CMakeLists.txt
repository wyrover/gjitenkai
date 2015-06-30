# Set the name and the supported language of the project
project(gjiten-kai C)

# Set the minimum version of cmake required to build this project
cmake_minimum_required(VERSION 2.6)

#install directories variables
include(GNUInstallDirs)

#add_definitions(-DUSE_MMAP)

# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

# Add other flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER})

#set some variable 
##dir where to install / find files
set(DATADIR ${CMAKE_INSTALL_FULL_DATADIR}/gjiten-kai)
set(DICDIR ${CMAKE_INSTALL_FULL_DATADIR}/dict)
set(LOCALDIR ${CMAKE_INSTALL_FULL_LOCALEDIR})

#generate config.h from config.h.in
configure_file(config.h.in config.h)

# files needed to compile gjitenkai
add_executable(
  bin/gjitenkai
  src/gjitenkai/main.c 
  src/gjitenkai/gjitenkai.c
  src/gjitenkai/callbacks.c
  src/worddic/conf.c 
  src/worddic/worddic.c 
  src/worddic/preferences.c
  src/worddic/inflection.c 
  src/worddic/callbacks.c
  src/worddic/dicentry.c
  src/worddic/worddic_dicfile.c
  src/worddic/dicresult.c 
  src/kanjidic/conf.c 
  src/kanjidic/kanjidic.c 
  src/kanjidic/kanjiutils.c 
  src/kanjidic/callbacks.c
  src/kanjidic/kanjifile.c
  src/kanjidic/radical_list.c
  src/kanjidic/preferences.c
  src/kanjidic/kanji_item.c
  src/common/conf.c
  src/common/dicfile.c 
  src/common/dicutil.c
  src/common/callbacks.c
  src/common/error.c   
)

# files needed to compile worddic
add_executable(
  bin/worddic 
  src/worddic/main.c 
  src/worddic/worddic.c 
  src/worddic/preferences.c
  src/worddic/inflection.c 
  src/worddic/callbacks.c
  src/worddic/conf.c
  src/worddic/dicentry.c
  src/worddic/dicresult.c 
  src/worddic/worddic_dicfile.c 
  src/common/conf.c
  src/common/dicfile.c
  src/common/dicutil.c 
  src/common/error.c 
  src/common/callbacks.c  
)

# files needed to compile kanjidic
add_executable(
  bin/kanjidic
  src/kanjidic/main.c
  src/kanjidic/kanjidic.c 
  src/kanjidic/kanjiutils.c
  src/kanjidic/kanjifile.c
  src/kanjidic/callbacks.c
  src/kanjidic/radical_list.c
  src/kanjidic/preferences.c
  src/kanjidic/kanji_item.c
  src/kanjidic/conf.c 
  src/common/dicfile.c 
  src/common/conf.c 
  src/common/dicutil.c
  src/common/error.c 
  src/common/callbacks.c
)

# Compiler flag needed to reach callback functions
set(GCC_COVERAGE_COMPILE_FLAGS "-rdynamic")

# gsettings
include(GSettings.cmake)
add_schemas(gjitenkai data)

# Link the target to the GTK+ libraries
target_link_libraries(bin/gjitenkai ${GTK3_LIBRARIES})
target_link_libraries(bin/worddic ${GTK3_LIBRARIES})
target_link_libraries(bin/kanjidic ${GTK3_LIBRARIES})

# Install radicals and inflection rules in the data dir
INSTALL(FILES data/radkfile.utf8 data/vconj.utf8 DESTINATION ${DATADIR})

# Install UI definitions files
INSTALL(FILES UI/worddic.glade UI/kanjidic.glade UI/gjitenkai.glade DESTINATION ${DATADIR})

# Install locales
#Français
INSTALL(FILES po/fr/worddic.mo DESTINATION ${CMAKE_INSTALL_LOCALEDIR}/fr/LC_MESSAGES/)

# Install programs
INSTALL(PROGRAMS bin/gjitenkai bin/worddic bin/kanjidic DESTINATION ${CMAKE_INSTALL_BINDIR})
