prglibdir=$(prefix)/sandesha2
prglib_LTLIBRARIES = libsandesha2.la

libsandesha2_la_SOURCES = msg_ctx.c\
                          mod_sandesha2.c\
                          msg_validator.c

INCLUDES = -I$(top_builddir)/include \
            @AXIS2INC@
libsandesha2_la_LIBADD = ../client/libsandesha_client.la\
                        ../handlers/libsandesha_handlers.la\
                        ../msgprocessors/libsandesha2_msgprocessors.la\
                        ../storage/beans/libsandesha2_storage_beans.la\
                        ../storage/inmemory/libsandesha2_inmemory.la\
                        ../transport/libsandesha2_transport.la\
                        ../util/libsandesha_util.la\
                        ../workers/libsandesha2_workers.la\
                        ../wsrm/libsandesha2_wsrm.la

