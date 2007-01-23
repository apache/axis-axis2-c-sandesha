prgbindir=$(prefix)/bin/samples
prgbin_PROGRAMS = rm_ping_1_1
samplesdir=$(prefix)/samples/client/rm_ping_1_1
samples_DATA=rm_ping_1_1.c Makefile.am Makefile.in
rm_ping_1_1_SOURCES = rm_ping_1_1.c

rm_ping_1_1_LDADD   =  \
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
                    -L$(AXIS2C_HOME)/modules/sandesha2 \
					-lsandesha2_client \
					-lsandesha2 \
					-lsqlite3 \
                    $(GUTHTHILA_LIBS) \
                    $(LIBXML2_LIBS)

INCLUDES = -I$(AXIS2C_HOME)/include \
			-I$(AXIS2C_HOME)/modules/sandesha2 \
			@UTILINC@ \
			@AXIOMINC@
