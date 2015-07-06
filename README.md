Gjiten kai - グジテン改

A new implementation of Gjiten, a Gnome japanese dictionary.

# Goal
The goal of this rewrite of Gjiten is to have a Gjiten with new features and a
new interface, while keeping the original overall design. 

#Prerequistes

## Edict 
To use the worddic (word dictionary) you will need an edict file encoded in UTF-8
The official edict dictionary is encoded in EUC-JP.

We provide a converted version at http://odrevet.github.io/gjitenkai/downloads/edict.utf8

Or you can convert the edict file yourself with the following command:
<pre>
 iconv -f EUC-JP -t UTF-8 dictfile -o dictfile.utf8
</pre>

Then set the worrdic file in Edit/preferences/ Worddic - Dictionary - New - Browse ...  

## Other

You also need Japanese font and a Japanse input system. See your OS documentation
for more details. 

# Differances with the legacy Gjiten

* New implementation 
* Regex search, colored output, EDICT2 support
* a lot more ! 

# Build

The Gjiten Kai project uses CMake to generate the Makefile

<pre>
--generate Makefile and config.h
$cmake . 
--generate binaries worrdic, kanidic and gjitenkai
$make
--install dictionnaries, settings and binaries
#make install
</pre>

## Build Under Windows with MSYS/MinGW

Install MSYS/MinGW

Install CMake for Windows

Install GTK for Windows http://www.gtk.org/download/win32.php

<pre>
$cmake -G "MSYS Makefiles" . && make && make install
</pre>

#Install

The Windows version is still a work in progress.

Linux user must install it from source.

Arch Linux users can install it from AUR (https://aur4.archlinux.org/packages/gjitenkai-git)

# Credits
* Gjiten kai developer (2015 - ...) Olivier Drevet
* Special Thanks to the Original Gjiten developer (1999 - 2005) Botond Botyanszki 
* EDICT Dictionary Released under Creative Commons Attribution-ShareAlike Licence (V3.0)