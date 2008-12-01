prgbindir = $(prefix)/bin/samples/sandesha2/amqp

prgbin_PROGRAMS = rm_ping_1_0

rm_ping_1_0_SOURCES = rm_ping_1_0.c

rm_ping_1_0_LDADD = ../../../src/core/libsandesha2.la \
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
