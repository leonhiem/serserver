
lib_LIBRARIES = libsocket.a
AM_CPPFLAGS = -pedantic -Wall -Woverloaded-virtual -Wwrite-strings -I$(top_srcdir)/include/

############################################################################
#
#
############################################################################

socket_SOURCES = socket.cpp socket.h

# where to install the headers on the system
libsocket_adir = $(includedir)/libsolar

# the list of header files that belong to the library (to be installed later)
libsocket_a_HEADERS = $(top_srcdir)/include/socket.h

# the sources to add to the library and to add to the source distribution
libsocket_a_SOURCES =  $(socket_SOURCES)

