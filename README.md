# graviton
Graviton project by Ethinza, Inc

# Clone repository first

$ git clone https://github.com/ethinza/graviton

# To Build
This folder contains all embedded projects and server drivers. "graviton" is 
container of all projects that run on various platforms.


== Top level folders ==

Folder  Description
------  -----------
apps    Applications for release
docs    Doxygen generated help files from source files
libs    All library files - generic and global files
make    All make files required to build project(s)
mods    Kernel level modules
tests   Test programs based on library files
tools   Stand alone application used in the project source files
xparty  External 3rd party tools/applications

Whenever a new top level folder is added, add description above and modify
"Makefile" in the product (scb) folder to add newly added folder to the 
"SUBDIRS" tag.

"setenv" file contains setting build environment. You can copy the content in
this file to your ".bashrc" file (~/.bashrc), so that you do not have to execute
 "setenv" always.
 
 If this does not work, execute following on your shell.
 
 shell > export ETHINZA_DIR=`pwd`
 
 == Sync with latest git repository changes ==
 
shell > git pull

== Build procedure ==

To clean up all object files, execute "make clobber".
To build all the folder objects and executables if any, execute "make".

By default, "make" or "make clobber" do not generate verbose output. If you want
 to see complete output of build, open "Makefile" in product folder and 
uncomment the line or add the line as "export VERBOSE := 1".


== git repository ==

Once local changes compile without error/warning (make) and are tested, to 
commit follow the next example.

shell > git add
shell > git push

///////////////////////////////////////////////////////////////////////////////////////

== Project Details ==

Library Folders: 
----------------
Include ethinza.h in all code files (well, mostly).
#include <ethinza.h>

"base" contains base class for all special data types.
"utility" contains OS, memory etc utilities.
"service" contains system level services such as thread monitoring.
"log" contains logging subsystem.
"protocol" contains implementation of storageZ protocol.
"user" contains user interfaces.

Test Folders:
-------------

Tools:
------
