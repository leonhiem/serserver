bin_PROGRAMS = solarclient 
SUBDIRS = socket

CPPFLAGS = -pedantic -Wall -Woverloaded-virtual -Wwrite-strings 

solarclass_SOURCES = parse.h parse.cpp client.h client.cpp solarclient.cpp

# the library names to build (note we are building static libs only)
lib_LIBRARIES = libsolar.a libgetopt.a 

# where to install the headers on the system
libsolar_adir = $(includedir)/libsolar

# the list of header files that belong to the library (to be installed later)
libsolar_a_HEADERS =  client.h parse.h 

# the sources to add to the library and to add to the source distribution
libsolar_a_SOURCES = $(solarclass_SOURCES)


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
#
############################################################################

solarclient_SOURCES = solarclient.cpp sun.h sun.cpp
solarclient_LDADD = libsolar.a socket/libsocket.a libgetopt.a 

