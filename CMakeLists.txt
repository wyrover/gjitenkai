# Set the name and the supported language of the project
project(gjiten-kai C)

# Set the minimum version of cmake required to build this project
cmake_minimum_required(VERSION 2.6)

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
set(DATADIR "/usr/share/gjiten-kai")
set(DICDIR "/usr/share/dict")

#generate config.h from config.h.in
configure_file(config.h.in config.h)

# files needed to compile gjitenkai
add_executable(
  bin/gjitenkai
  src/gjitenkai/main.c 
  src/gjitenkai/gjitenkai.c
  src/gjitenkai/callbacks.c
  src/worddic/worddic.c 
  src/worddic/preferences.c
  src/worddic/inflection.c 
  src/worddic/callbacks.c
  src/kanjidic/kanjidic.c 
  src/kanjidic/kanjiutils.c 
  src/kanjidic/callbacks.c
  src/kanjidic/kanjifile.c
  src/kanjidic/radical_list.c
  src/kanjidic/preferences.c
  src/common/conf.c 
  src/common/dicutil.c 
  src/common/dicfile.c 
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
  src/common/conf.c 
  src/common/dicutil.c 
  src/common/dicfile.c 
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

# Install dictionaries
INSTALL(FILES data/edict.utf8 data/kanjidic.utf8 DESTINATION ${DICDIR})

# Install programs
INSTALL(PROGRAMS bin/gjitenkai bin/worddic bin/kanjidic DESTINATION bin)