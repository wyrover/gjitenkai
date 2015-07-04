# Set the name and the supported language of the project
project(gjitenkai C)

# Set the minimum version of cmake required to build this project
cmake_minimum_required(VERSION 2.6)

#install directories variables
include(GNUInstallDirs)

#dir where to install / find files
set(DATADIR_FULL ${CMAKE_INSTALL_FULL_DATADIR}/data)
set(DICDIR_FULL ${CMAKE_INSTALL_FULL_DATADIR}/dict)
set(LOCALDIR_FULL ${CMAKE_INSTALL_FULL_LOCALEDIR})

set(DATADIR ${CMAKE_INSTALL_DATADIR}/data)
set(DICDIR ${CMAKE_INSTALL_DATADIR}/dict)
set(LOCALDIR ${CMAKE_INSTALL_LOCALEDIR})
set(SCHEMADIR ${CMAKE_INSTALL_DATADIR}/glib-2.0/schemas)

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

#if we want to create a NSIS script
if(NSIS)
	include (NSIS.cmake)

	#install the gtk lib runtime in the binary directory
	set (GTK_BIN C:/gtk/bin)   #TODO set the GTK_BIN variable dynamically 
	
	#these are all the required dll listed in the GTK documentation
	#see http://www.gtk.org/download/win32_contentlist.php
    INSTALL(FILES ${GTK_BIN}/libatk-1.0-0.dll
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
				  ${GTK_BIN}/libpangocairo-1.0-0.dll
				  ${GTK_BIN}/libpangowin32-1.0-0.dll
				  ${GTK_BIN}/libpixman-1-0.dll
				  ${GTK_BIN}/libpng15-15.dll
				  ${GTK_BIN}/libxml2-2.dll
				  ${GTK_BIN}/pthreadGC2.dll
				  ${GTK_BIN}/zlib1.dll 
            DESTINATION ${CMAKE_INSTALL_BINDIR})
endif(NSIS)
	
# Custom target for packaging.
if(MINGW)
  add_definitions(-DMINGW)
endif(MINGW)

# Add other flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER})

#generate config.h from config.h.in
configure_file(config.h.in config.h)

# files needed to compile gjitenkai
add_executable(
  gjitenkai
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

# Link the target to the GTK+ libraries
target_link_libraries(gjitenkai ${GTK3_LIBRARIES})

# Compiler flag needed to reach callback functions
set(GCC_COVERAGE_COMPILE_FLAGS "-rdynamic")



include(GSettings.cmake)
set(LOCAL_SCHEMADIR data/gschemas)

#under windows point to a directory with specifique options and org files
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  set(LOCAL_SCHEMADIR ${LOCAL_SCHEMADIR}/windows)
endif()

## INSTALL
# Install kanjidic
INSTALL(FILES data/kanjidic.utf8 DESTINATION ${DICDIR})

# Install radicals and inflection rules in the data dir
INSTALL(FILES data/radkfile.utf8 data/vconj.utf8 DESTINATION ${DATADIR})

# Install UI definitions files
INSTALL(FILES UI/worddic.glade UI/kanjidic.glade UI/gjitenkai.glade DESTINATION ${DATADIR})

# Install locales
##Français
INSTALL(FILES po/fr/worddic.mo DESTINATION ${CMAKE_INSTALL_LOCALEDIR}/fr/LC_MESSAGES/)

# Install programs 
## Set the output binary full path name variable
set(GJITENKAI_OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/gjitenkai)

#if the current system is Windows, the generated binary will end with .exe
IF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	set(GJITENKAI_OUTPUT ${GJITENKAI_OUTPUT}.exe)
ENDIF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

INSTALL(PROGRAMS ${GJITENKAI_OUTPUT} DESTINATION ${CMAKE_INSTALL_BINDIR})