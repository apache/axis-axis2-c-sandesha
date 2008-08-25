prglibdir=$(prefix)/lib
prglib_LTLIBRARIES = libsandesha2_client.la

libsandesha2_client_la_SOURCES = \
							sequence_report.c \
							report.c \
							client.c

libsandesha2_client_la_LIBADD = ../handlers/libsandesha_handlers.la\
                        ../msgprocessors/libsandesha2_msgprocessors.la\
                        ../storage/beans/libsandesha2_storage_beans.la\
						../storage/common/libsandesha2_storage.la\
                        ../storage/sqlite/libsandesha2_permanent.la\
                        ../transport/libsandesha2_transport.la\
                        ../util/libsandesha_util.la\
                        ../workers/libsandesha2_workers.la\
                        ../wsrm/libsandesha2_wsrm.la \
                        ../polling/libsandesha2_polling.la

INCLUDES = -I$(top_builddir)/include \
		   	@AXIS2INC@ \
		   	@AXIOMINC@ \
		   	@UTILINC@ \
			@NEETHIINC@
