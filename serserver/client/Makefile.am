bin_PROGRAMS = serclient 
SUBDIRS = socket

CPPFLAGS = -pedantic -Wall -Woverloaded-virtual -Wwrite-strings 

serclass_SOURCES = parse.h parse.cpp client.h client.cpp serclient.cpp

# the library names to build (note we are building static libs only)
lib_LIBRARIES = libser.a libgetopt.a 

# where to install the headers on the system
libser_adir = $(includedir)/libser

# the list of header files that belong to the library (to be installed later)
libser_a_HEADERS =  client.h parse.h 

# the sources to add to the library and to add to the source distribution
libser_a_SOURCES = $(serclass_SOURCES)


############################################################################
#
# Library getopt
#
############################################################################

# where to install the headers on the system
libgetopt_adir = $(includedir)/libser

# the list of header files that belong to the library (to be installed later)
libgetopt_a_HEADERS = getopts.h

# the sources to add to the library and to add to the source distribution
libgetopt_a_SOURCES =  $(libsocket_a_HEADERS) \
                       getopts.cpp

############################################################################
#
#
############################################################################

serclient_SOURCES = serclient.cpp 
serclient_LDADD = libser.a socket/libsocket.a libgetopt.a 

