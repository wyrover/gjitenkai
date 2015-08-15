# Set the name and the supported language of the project
project(gjitenkai C)

# Set the minimum version of cmake required to build this project
cmake_minimum_required(VERSION 2.6)

#install directories variables
include(GNUInstallDirs)

##directories where to find/install files.
#These path are used in with the install instruction which only supports
#relative path
set(SHAREDIR       ${CMAKE_INSTALL_DATADIR})
set(LOCALDIR       ${CMAKE_INSTALL_LOCALEDIR})
set(GLIB_SCHEMADIR "glib-2.0/schemas")
set(SCHEMADIR      ${CMAKE_INSTALL_DATADIR}/${GLIB_SCHEMADIR})

#These path are passed onto config.h.in to be used as preprocessor definitions
if(MINGW)
	set(SHARE_DIR "../share")
    set(LOCALE_DIR ${SHARE_DIR}/${PROJECT_NAME}/locale)
	set(DICT_DIR ${SHARE_DIR}/${PROJECT_NAME}/dict)
else(MINGW)
	set(SHARE_DIR ${CMAKE_INSTALL_FULL_DATADIR})
	set(LOCALE_DIR ${CMAKE_INSTALL_FULL_LOCALEDIR})
	set(DICT_DIR ${CMAKE_INSTALL_FULL_DATADIR}/dict)
endif(MINGW)


#generate config.h from config.h.in
configure_file(config.h.in config.h)

############################################################
option(NSIS "Use NSIS generator to produce installer" OFF)

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

#set the pkg-config variable
set(PKG_CONFIG_EXECUTABLE pkg-config)

#find the glib prefix directory with pkg-config 
execute_process (
COMMAND ${PKG_CONFIG_EXECUTABLE} gio-2.0 --variable prefix
OUTPUT_VARIABLE GLIB_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE
)
	
#if we want to create a NSIS script
if(NSIS)
  set(CPACK_PACKAGE_EXECUTABLES ${PROJECT_NAME} ${PROJECT_NAME}.exe)
  #set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)
  #set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README.md)
  include (NSIS.cmake)

  #install the Gtk DLLs from the Gtk's bin directory 
  set (GTK_BIN ${GLIB_PREFIX}/bin)
  
  #these are all the required dll listed in the GTK documentation
  #see http://www.gtk.org/download/win32_contentlist.php
  INSTALL(FILES
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
    ${GTK_BIN}/zlib1.dll 
    DESTINATION ${CMAKE_INSTALL_BINDIR})
endif(NSIS)

# Add other flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER})
add_definitions(-Wall)

# files needed to compile gjitenkai
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

include(GSettings.cmake)

if(MINGW)
  #under windows point to a directory with specific options and org files
  compile_schemas(${SCHEMADIR}/windows ${SCHEMADIR})
  
  #add a compiler definition
  add_definitions(-DMINGW)
else()
  compile_schemas(${SCHEMADIR} ${SCHEMADIR})
endif()

# INSTALL
##Install settings
###Compiled file
install(
  FILES ${SCHEMADIR}/gschemas.compiled
  DESTINATION ${SCHEMADIR}
  )

#The xml gschema is installed exepct when creating an NSIS package because:
#In a standalone application using an installer, we only need the app schema
#Absolute install path are forbidden when creating a package
if(NOT NSIS)
message ("COMPLETE SHEMADIR IS ${GLIB_PREFIX}/${SCHEMADIR}")
###gschema.xml
install(
  FILES ${SCHEMADIR}/${PROJECT_NAME}.gschema.xml
  DESTINATION ${GLIB_PREFIX}/${SCHEMADIR}
  )
endif()

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

## Install UI definitions files
install(
  FILES 
  ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/worddic.glade 
  ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/kanjidic.glade 
  ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/${PROJECT_NAME}.glade
  DESTINATION 
  ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
  )

## Install locales
###Français
install(FILES
  po/fr/${PROJECT_NAME}.mo
  po/fr/worddic.mo
  po/fr/kanjidic.mo
  DESTINATION ${CMAKE_INSTALL_LOCALEDIR}/fr/LC_MESSAGES/)

## Install desktop file
install(
  FILES ${PROJECT_NAME}.desktop
  DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

## Install icon
# install(
  # FILES pixmaps/${PROJECT_NAME}.png
  # DESTINATION ${CMAKE_INSTALL_DATADIR}/icons)

## Install pixmaps
install(
  FILES pixmaps/${PROJECT_NAME}.png pixmaps/kai.png
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})

## Install program
### Set the output binary full path name variable
set(OUTPUT_NAME ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})

#if the current system is Windows, the generated binary will end with .exe
if(MINGW)
  set(OUTPUT_NAME ${OUTPUT_NAME}.exe)
endif()

install(PROGRAMS ${OUTPUT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
