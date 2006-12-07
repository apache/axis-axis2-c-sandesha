prglibdir=$(prefix)/sandesha2
prglib_LTLIBRARIES = libsandesha2_client.la

libsandesha2_client_la_SOURCES = \
							sequence_report.c \
							report.c \
							client.c

INCLUDES = -I$(top_builddir)/include \
			-I$(top_builddir)/include/sandesha2 \
			@AXIS2INC@
