prgbindir=$(prefix)/bin/samples
prgbin_PROGRAMS = rm_echo
samplesdir=$(prefix)/samples/client/rm_echo
samples_DATA=rm_echo.c Makefile.am Makefile.in
rm_echo_SOURCES = rm_echo.c echo_util.c

rm_echo_LDADD   =  \
                    -L$(AXIS2C_HOME)/lib \
					-laxis2_util \
                    -laxis2_axiom \
                    -laxis2_engine \
                    -laxis2_parser \
                    -lpthread \
                    -laxis2_http_sender \
                    -laxis2_http_receiver \
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
