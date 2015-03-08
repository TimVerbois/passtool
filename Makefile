#
# General Hash Function Algorithms Master MakeFile
# By Arash Partow - 2000
#
# URL: http://www.partow.net/programming/hashfunctions/index.html
#
# Copyright Notice:
# Free use of this library is permitted under the
# guidelines and in accordance with the most
# current version of the Common Public License.
# http://www.opensource.org/licenses/cpl.php
#

COMPILER      = -g++
OPTIONS       = -ansi -pedantic -Wall -o
OPTIONS_LIBS  = -ansi -pedantic -Wall -c 

all: key.o passtoolconf.o GeneralHashFunctions.o passtool

key.o: key.cpp key.hpp
	$(COMPILER) $(OPTIONS_LIBS) key.cpp

passtoolconf.o: passtoolconf.cpp passtoolconf.hpp
	$(COMPILER) $(OPTIONS_LIBS) passtoolconf.cpp

GeneralHashFunctions.o: GeneralHashFunctions.cpp GeneralHashFunctions.hpp
	$(COMPILER) $(OPTIONS_LIBS) GeneralHashFunctions.cpp

passtool: GeneralHashFunctions.cpp passtool.cpp passtoolconf.cpp key.cpp
	$(COMPILER) $(OPTIONS) passtool passtool.cpp GeneralHashFunctions.o  passtoolconf.o key.o


clean:
	rm -f core *.o *.bak *stackdump passtool *#
  
#
# The End !
#
