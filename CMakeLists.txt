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
#These path must be relative
set(GLIB_SCHEMADIR "glib-2.0/schemas")
set(SCHEMADIR      ${CMAKE_INSTALL_DATADIR}/${GLIB_SCHEMADIR})

#These path are passed onto config.h.in to be used as preprocessor definitions
if(MINGW)
  set(SHARE_DIR "../share")
  set(LOCALE_DIR ${SHARE_DIR}/${PROJECT_NAME}/locale)
else(MINGW)
  set(SHARE_DIR ${CMAKE_INSTALL_FULL_DATADIR})
  set(LOCALE_DIR ${CMAKE_INSTALL_FULL_LOCALEDIR})
endif(MINGW)

#generate config.h from config.h.in
configure_file(config.h.in config.h)

############################################################
##Windows only options
option(NSIS "Use NSIS generator to produce installer" OFF)
option(STANDALONE "Install as standalone" OFF)

# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
############################################################

#compiled binary output directory
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

#if we want to create a NSIS script
if(NSIS OR STANDALONE)
  if(NSIS)
    set(CPACK_PACKAGE_EXECUTABLES ${PROJECT_NAME} ${PROJECT_NAME}.exe)
    set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)
    set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README.md)
    include (NSIS.cmake)
  endif()

  #install the Gtk DLLs from the Gtk's bin directory 
  set (GTK_BIN ${GLIB_PREFIX}/bin)

  #these are all the required dll listed in the GTK documentation
  #see http://www.gtk.org/download/win32_contentlist.php
  set (GTK_LIBS
    ${GTK_BIN}/libatk-1.0-0.dll
    ${GTK_BIN}/libfontconfig-1.dll
    ${GTK_BIN}/libgdk-3-0.dll
    ${GTK_BIN}/liblzma-5.dll
    ${GTK_BIN}/libcairo-2.dll
    ${GTK_BIN}/libcairo-gobject-2.dll
    ${GTK_BIN}/libffi-6.dll
    ${GTK_BIN}/libfreetype-6.dll
    ${GTK_BIN}/libgdk_pixbuf-2.0-0.dll
    ${GTK_BIN}/libgio-2.0-0.dll 
    ${GTK_BIN}/libglib-2.0-0.dll
    ${GTK_BIN}/libgmodule-2.0-0.dll 
    ${GTK_BIN}/libgobject-2.0-0.dll
    ${GTK_BIN}/libgtk-3-0.dll
    ${GTK_BIN}/libiconv-2.dll 
    ${GTK_BIN}/libintl-8.dll 
    ${GTK_BIN}/libpango-1.0-0.dll
    ${GTK_BIN}/libpango-1.0-0.dll 
    ${GTK_BIN}/libpangocairo-1.0-0.dll
    ${GTK_BIN}/libpangoft2-1.0-0.dll
    ${GTK_BIN}/libpangowin32-1.0-0.dll
    ${GTK_BIN}/libpixman-1-0.dll
    ${GTK_BIN}/libpng16-16.dll
    ${GTK_BIN}/libxml2-2.dll
    ${GTK_BIN}/zlib1.dll)

  install(
    PROGRAMS ${GTK_LIBS}
    DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT System
    )

  #install WINGW runtime libs
  get_filename_component( MINGW_BIN ${CMAKE_C_COMPILER} DIRECTORY )
  set(RUNTIME_LIBS
    ${MINGW_BIN}/libgcc_s_dw2-1.dll
    ${MINGW_BIN}/libwinpthread-1.dll
    ${MINGW_BIN}/libbz2-1.dll
    ${MINGW_BIN}/libepoxy-0.dll
    ${MINGW_BIN}/libexpat-1.dll
    ${MINGW_BIN}/libharfbuzz-0.dll
    ${MINGW_BIN}/libstdc++-6.dll)

  install( PROGRAMS ${RUNTIME_LIBS} DESTINATION bin COMPONENT System )

endif()

# Custom definition
if(MINGW)
  add_definitions(-DMINGW)
endif(MINGW)


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
  )

# Link the target to the GTK+ libraries
target_link_libraries(${PROJECT_NAME} ${GTK3_LIBRARIES} -lz)

# Compiler flag needed to reach callback functions
set(GCC_COVERAGE_COMPILE_FLAGS "-rdynamic")

#if using NSIS, compile the setting xml file in the source tree
#if not using NSIS install and compile in the glib directory
if(NSIS OR STANDALONE)
  #Settings. Compile this software gschema localy
  compile_schemas(${SCHEMADIR}/NSIS ${SCHEMADIR})    

  install(
    FILES ${SCHEMADIR}/gschemas.compiled
    DESTINATION ${SCHEMADIR}
    )

  #install files from the glib dir to the install dir
  ##icons
  install(
    DIRECTORY ${GLIB_PREFIX}/share/icons
    DESTINATION ${CMAKE_INSTALL_DATADIR}
    )

  install(
    FILES ${GLIB_PREFIX}/lib/gdk-pixbuf-2.0/2.10.0/loaders.cache
    DESTINATION ${CMAKE_INSTALL_DATADIR}/../lib/gdk-pixbuf-2.0/2.10.0/
    )

else()
  # install the gschema.xml schema file in glib system dir
  install(
    FILES ${SCHEMADIR}/${PROJECT_NAME}.gschema.xml
    DESTINATION ${GLIB_PREFIX}/${SCHEMADIR}
    )

  #compile the glib schemas directory
  install(
    CODE
    "message (STATUS \"Compiling GSettings schemas at ${GLIB_PREFIX}/${SCHEMADIR}\")"
    )

  compile_schemas_install(${GLIB_PREFIX}/${SCHEMADIR})
endif()

## Install UI definitions files
install(
  FILES
  ${SOURCE_SHARE}/${PROJECT_NAME}/${PROJECT_NAME}.glade
  ${SOURCE_SHARE}/${PROJECT_NAME}/worddic.glade
  ${SOURCE_SHARE}/${PROJECT_NAME}/kanjidic.glade
  DESTINATION 
  ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
  )

## Install locales
###Français
install(
  FILES
  po/fr/${PROJECT_NAME}.mo
  po/fr/worddic.mo
  po/fr/kanjidic.mo
  DESTINATION ${CMAKE_INSTALL_LOCALEDIR}/fr/LC_MESSAGES/)

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

## Install icons
install(
  FILES ${SOURCE_SHARE}/${PROJECT_NAME}/${PROJECT_NAME}.png
  DESTINATION ${CMAKE_INSTALL_DATADIR}/icons)
install(
  FILES ${SOURCE_SHARE}/${PROJECT_NAME}/${PROJECT_NAME}.png
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})

## Install desktop file
install(
  FILES ${PROJECT_NAME}.desktop
  DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

## Install programs 
## Set the output binary full path name variable
set(OUTPUT_BIN ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

#if the current system is Windows, the generated binary ends with .exe
if(MINGW)
  set(OUTPUT_BIN ${OUTPUT_BIN}.exe)
endif()

install(PROGRAMS ${OUTPUT_BIN} DESTINATION ${CMAKE_INSTALL_BINDIR})
