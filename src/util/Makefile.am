noinst_LTLIBRARIES = libsandesha_util.la

libsandesha_util_la_SOURCES = \
							error.c \
							utils.c \
							spec_specific_consts.c\
                            fault_data.c\
                            fault_mgr.c\
                            ack_mgr.c \
							seq_mgr.c \
                            terminate_mgr.c \
							msg_init.c \
							msg_creator.c\
                            msg_retrans_adjuster.c\
                            property_mgr.c

INCLUDES = -I$(top_builddir)/include \
			-I$(top_builddir)/include/sandesha2 \
			@AXIS2INC@
