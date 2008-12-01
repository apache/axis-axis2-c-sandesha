prgbindir = $(prefix)/bin/samples/sandesha2/amqp

prgbin_PROGRAMS = rm_mtom_1_0

rm_mtom_1_0_SOURCES = rm_mtom_1_0.c mtom_util.c

rm_mtom_1_0_LDADD = ../../../src/core/libsandesha2.la \
					-laxutil \
                    -laxis2_axiom \
                    -laxis2_engine \
                    -laxis2_parser \
                    -lpthread \
                    -lsandesha2_client \
                    $(GUTHTHILA_LIBS) \
                    $(LIBXML2_LIBS)

INCLUDES = @AXIS2INC@ \
		   -I../../../include 
