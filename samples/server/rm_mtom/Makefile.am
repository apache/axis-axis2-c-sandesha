prglibdir=$(prefix)/services/rm_mtom
samplesdir=$(prefix)/samples/server/rm_mtom
prglib_LTLIBRARIES = librm_mtom.la
samples_DATA=mtom.c mtom_skeleton.c mtom.h services.xml Makefile.am Makefile.in
prglib_DATA= services.xml
EXTRA_DIST = services.xml
noinst_HEADERS = mtom.h
SUBDIRS =
librm_mtom_la_SOURCES = mtom.c mtom_skeleton.c
librm_mtom_la_LIBADD  =
INCLUDES = -I$(AXIS2C_HOME)/include \
			@UTILINC@ \
			@AXIOMINC@
