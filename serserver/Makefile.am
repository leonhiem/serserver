bin_PROGRAMS = serserver 
SUBDIRS = socket

CPPFLAGS = -pedantic -Wall -Woverloaded-virtual -Wwrite-strings -D_REENTRANT 

serclass_SOURCES = serial.h serial.cpp parse.h parse.cpp

# the library names to build (note we are building static libs only)
lib_LIBRARIES = libser.a libgetopt.a 

# where to install the headers on the system
libser_adir = $(includedir)/libser

# the list of header files that belong to the library (to be installed later)
libser_a_HEADERS =  serial.h parse.h 

# the sources to add to the library and to add to the source distribution
libser_a_SOURCES = $(serclass_SOURCES)


############################################################################
#
# Library getopt
#
############################################################################

# where to install the headers on the system
libgetopt_adir = $(includedir)/libsolar

# the list of header files that belong to the library (to be installed later)
libgetopt_a_HEADERS = getopts.h

# the sources to add to the library and to add to the source distribution
libgetopt_a_SOURCES =  $(libsocket_a_HEADERS) \
                       getopts.cpp

############################################################################
#
# Pointserver program
#
############################################################################

serserver_SOURCES = serserver.cpp 
serserver_LDADD = libser.a socket/libsocket.a libgetopt.a -lpthread 
#-lboost_regex

