# Set the name and the supported language of the project
project(gjitenkai C)

# Set the minimum version of cmake required to build this project
cmake_minimum_required(VERSION 2.6)

#include directories variables
include(GNUInstallDirs)

#include GSettings macro
include(GSettings.cmake)

#Directories in the source tree
set (SOURCE_SHARE "share")

##directories where to install files.
#These paths must be relative
set(GLIB_SCHEMADIR "glib-2.0/schemas")
set(SCHEMADIR      ${CMAKE_INSTALL_DATADIR}/${GLIB_SCHEMADIR})

set(RES_FILES "")

#compiled binary output directory
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

## Set the output binary full path name variable
set(OUTPUT_BIN ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

#Make the binary directory
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

#These path are passed onto config.h.in to be used as preprocessor definitions
if(MINGW)
  include(MinGW.cmake)
else()
  set(SHARE_DIR ${CMAKE_INSTALL_FULL_DATADIR})
  set(LOCALE_DIR ${CMAKE_INSTALL_FULL_LOCALEDIR})

  # at install, copy the software gschema.xml to the glib gschemas directory
  install(
    FILES ${SOURCE_SHARE}/${GLIB_SCHEMADIR}/${PROJECT_NAME}.gschema.xml
    DESTINATION ${GLIB_PREFIX}/${SCHEMADIR}
    )

  #at install, compile glib gschemas directory
  install(
    CODE
    "message (STATUS \"Compiling GSettings schemas at ${GLIB_PREFIX}/${SCHEMADIR}\")"
    )

  compile_schemas_install(${GLIB_PREFIX}/${SCHEMADIR})
endif()

#generate config.h from config.h.in
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h)

# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(LIBSOUP REQUIRED libsoup-2.4)


# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(${GTK3_INCLUDE_DIRS} ${LIBSOUP_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS} ${LIBSOUP_LIBRARY_DIRS})
############################################################

# Add other flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER})
add_definitions(-Wall)

# files needed to compile the executable
add_executable(
  ${PROJECT_NAME}
  src/${PROJECT_NAME}/main.c
  src/${PROJECT_NAME}/${PROJECT_NAME}.c
  src/${PROJECT_NAME}/callbacks.c
  src/worddic/conf.c
  src/worddic/worddic.c
  src/worddic/preferences.c
  src/worddic/inflection.c
  src/worddic/callbacks.c
  src/worddic/dicentry.c
  src/worddic/worddic_dicfile.c
  src/worddic/dicresult.c
  src/worddic/gloss.c
  src/kanjidic/conf.c
  src/kanjidic/kanjidic.c
  src/kanjidic/kanjiutils.c
  src/kanjidic/callbacks.c
  src/kanjidic/kanjifile.c
  src/kanjidic/radical_window.c
  src/kanjidic/preferences.c
  src/kanjidic/kanji_item.c
  src/common/conf.c
  src/common/dicfile.c
  src/common/dicutil.c
  src/common/callbacks.c
  src/common/error.c
  ${RES_FILES}   #resouces file for icon
  )

# Link the target to the GTK+ libraries
target_link_libraries(${PROJECT_NAME} ${GTK3_LIBRARIES} ${LIBSOUP_LIBRARIES} -lz)

# Compiler flag needed to reach callback functions
set(GCC_COVERAGE_COMPILE_FLAGS "-rdynamic")

## Install UI definitions files
install(
  FILES
  ${SOURCE_SHARE}/${PROJECT_NAME}/ui/${PROJECT_NAME}.ui
  ${SOURCE_SHARE}/${PROJECT_NAME}/ui/worddic.ui
  ${SOURCE_SHARE}/${PROJECT_NAME}/ui/kanjidic.ui
  DESTINATION
  ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/ui/
  )

## Install locales
###Français
install(
  FILES
  po/fr/${PROJECT_NAME}.mo
  po/fr/worddic.mo
  po/fr/kanjidic.mo
  DESTINATION ${CMAKE_INSTALL_LOCALEDIR}/fr/LC_MESSAGES/)

###Japanese
install(
  FILES
  po/jp/${PROJECT_NAME}.mo
  po/jp/worddic.mo
  po/jp/kanjidic.mo
  DESTINATION ${CMAKE_INSTALL_LOCALEDIR}/jp/LC_MESSAGES/)


## Install kanjidic
install(
  FILES ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/kanjidic.utf8
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
  )

## Install radicals and inflection rules in the data dir
install(
  FILES
  ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/radkfile.utf8
  ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/vconj.utf8
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
  )

## Install icon
install(
  FILES ${SOURCE_SHARE}/${PROJECT_NAME}/kai.png
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})
install(
  FILES ${SOURCE_SHARE}/${PROJECT_NAME}/kai.png
  DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/48x48/apps/)
#install about menu banner
install(
  FILES ${SOURCE_SHARE}/${PROJECT_NAME}/${PROJECT_NAME}.png
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})

## Install desktop file
install(
  FILES ${PROJECT_NAME}.desktop
  DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

## Install programs
install(PROGRAMS ${OUTPUT_BIN} DESTINATION ${CMAKE_INSTALL_BINDIR})
