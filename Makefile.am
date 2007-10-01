datadir=$(prefix)/modules/sandesha2
includedir=$(prefix)/include/sandesha2-0.91
docsdir=$(prefix)/docs/sandesha2c
sysconfdir=$(prefix)

SUBDIRS = src
include_HEADERS= include/*.h

data_DATA = LICENSE config/module.xml

docs_DATA=

sysconf_DATA=config/axis2.xml

install-data-hook: 
	cp -rf docs/* $(docsdir)


EXTRA_DIST=LICENSE config xdocs build samples ides


dist-hook:
	rm -rf `find $(distdir)/ -type d -name .svn`
	rm -rf `find $(distdir)/ -type f -name *.la`
	rm -rf `find $(distdir)/ -type f -name *.o`
	rm -rf `find $(distdir)/ -type f -name *.lo`
	rm -rf `find $(distdir)/ -type f -name *.loT`
	rm -rf `find $(distdir)/ -type d -name .deps`
	rm -rf `find $(distdir)/ -type d -name .libs`
	rm -rf `find $(distdir)/ -type f -name Makefile`
	cp -r target/docs $(distdir)
