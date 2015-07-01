Gjiten kai - グジテン改

A modern, partial rewrite of Gjiten, a japanese dictionary for Gnome.

# Goal
The goal of this rewrite of Gjiten is to have a Gjiten on which new features 
can be more easly added in the future. 

# Differances with the legacy Gjiten

## Core

* entirely rewrited user interface
* Use GTK3 with GladeBuilder
* code factorization, comments, cleanup...
* complete separation between core functions and user interface functions

## Misc

* Renamed 'English' by 'Latin' in the interface and the source code
  (English uses Latin characters, and gjiten can be use to search in 
  several language versions of edict dictionaries)

* Uses CMake to generate the Makefile (see Build section)

# Build

Gjiten Kai uses CMake to generate the Makefile

<pre>
--generate Makefile and config.h
$cmake . 
--generate binaries worrdic, kanidic and gjitenkai
$make
--install dictionnaries, settings and binaries
#make install
</pre>

## Build Under Windows with MinGW/MSYS

Install MSYS/MinGW and CMake for Windows
Install GTK for Windows http://www.gtk.org/download/win32.php

<pre>
--generate Makefile and config.h
$cmake -G "MSYS Makefiles" . 
--generate binaries worrdic, kanidic and gjitenkai
$make
--install dictionnaries, settings and binaries
$make install
</pre>

... will produce the commands equivalent to:
<pre>
$ gcc `pkg-config gtk+-3.0 --cflags` ./src/kanjidic/* ./src/common/* -o bin/kanjidic `pkg-config gtk+-3.0 --libs`
$ gcc `pkg-config gtk+-3.0 --cflags` ./src/worddic/* ./src/common/* -o bin/worddic `pkg-config gtk+-3.0 --libs`
$ gcc `pkg-config gtk+-3.0 --cflags` ./src/kanjidic/* ./src/worddic/* ./src/common/* -o bin/gjitenkai `pkg-config gtk+-3.0 --libs`
</pre>

To use the exe you'll also need Japanese font and a Japanse input system. 

# Credits
* Original Gjiten developer (1999 - 2005) Botond Botyanszki
* Gjiten kai developer (2015 - ...) Olivier Drevet
