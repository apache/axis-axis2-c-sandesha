SUBDIRS = wsrm storage util transport workers client handlers msgprocessors
noinst_LTLIBRARIES = libsandesha2.la

libsandesha2_la_SOURCES = msg_ctx.c\
                          mod_sandesha2.c\
                          msg_validator.c

INCLUDES = -I$(top_builddir)/include \
            @AXIS2INC@

