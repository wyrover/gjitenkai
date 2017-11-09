# Gjiten kai - グジテン改

<i>A modern re-implementation of Gjiten, a japanese dictionary</i>

# Goal
The goal of this rewrite of Gjiten is to have a Gjiten with new features and a
new interface, while keeping the original overall design. 

# Prerequistes

## Edict 
To use the worddic (word dictionary) you will need an EDICT dictionary file.
Get one from the Official EDICT website at 
http://ftp.monash.edu.au/pub/nihongo/00INDEX.html#dic_fil

Then set the worrdic file in Edit/preferences/ Worddic - Dictionary - New - Browse ...  

## Other

You also need Japanese font and a Japanse input system. See your OS documentation
for more details. 

# Documentation

A documentation in available in the project Wiki 

https://github.com/odrevet/gjitenkai/wiki/

## Learn how to search expressions efficiently

By reading the use case located at https://github.com/odrevet/gjitenkai/wiki/Search-expressions

# Differences with the Original Gjiten 2.6

* Almost entierly rewritten from scratch
* Very fast (even on older computer)
* Regex search
* Configurable output color, font and character separator
* EDICT2 support
* Use GTK3 with Glade
* Autoexpend kanji to it's radical list
* Threaded dictionary loading 

# Web site  

http://odrevet.github.io/gjitenkai/


# Credits
* Gjiten kai developer (2015 - ...) Olivier Drevet
* Special Thanks to the Original Gjiten developer (1999 - 2005) Botond Botyanszki 
* EDICT Dictionary Released under Creative Commons Attribution-ShareAlike Licence (V3.0)
