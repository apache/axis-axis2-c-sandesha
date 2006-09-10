SUBDIRS=resources
prgbindir=$(prefix)/bin/samples
prgbin_PROGRAMS = rm_mtom
samplesdir=$(prefix)/samples/client/rm_mtom
samples_DATA=mtom_client.c Makefile.am Makefile.in

rm_mtom_SOURCES = mtom_client.c

rm_mtom_LDADD   = $(LDFLAGS) \
                    -L$(AXIS2C_HOME)/lib \
					-laxis2_util \
                    -laxis2_axiom \
                    -laxis2_wsdl \
                    -laxis2_engine \
                    -laxis2_parser \
                    -lpthread \
                    -laxis2_http_sender \
                    -laxis2_http_receiver \
					-lwoden \
					-laxis2_xml_schema \
                    $(GUTHTHILA_LIBS) \
                    $(LIBXML2_LIBS)

INCLUDES = -I$(AXIS2C_HOME)/include \
			@UTILINC@ \
			@AXIOMINC@
