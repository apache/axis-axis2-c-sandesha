noinst_LTLIBRARIES = libsandesha2_mysql.la

libsandesha2_permanent_la_SOURCES = \
								permanent_create_seq_mgr.c \
								permanent_invoker_mgr.c \
								permanent_sender_mgr.c \
								permanent_next_msg_mgr.c \
								permanent_seq_property_mgr.c \
								permanent_transaction.c \
								permanent_storage_mgr.c \
								permanent_bean_mgr.c \
								msg_store_bean.c

INCLUDES = -I$(top_builddir)/include \
		   	@AXIS2INC@ \
			-I/usr/include/mysql

EXTRA_DIST=sandesha2_sqls.h
