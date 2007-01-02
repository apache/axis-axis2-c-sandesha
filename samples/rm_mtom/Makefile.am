prgbindir=$(prefix)/bin/samples
prgbin_PROGRAMS = rm_mtom
samplesdir=$(prefix)/samples/client/rm_mtom
samples_DATA=rm_mtom_client.c Makefile.am Makefile.in
rm_mtom_SOURCES = rm_mtom_client.c mtom_util.c

rm_mtom_LDADD   =  \
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
					-lsqlite3 \
					-laxis2_xml_schema \
                    $(GUTHTHILA_LIBS) \
                    $(LIBXML2_LIBS)

INCLUDES = -I$(AXIS2C_HOME)/include \
			-I$(AXIS2C_HOME)/modules/sandesha2 \
			@UTILINC@ \
			@AXIOMINC@
