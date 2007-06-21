datadir=$(prefix)/modules/sandesha2
includedir=$(prefix)/include/sandesha2-0.90
docsdir=$(prefix)/docs/sandesha2c
samplesdir=$(prefix)/samples/sandesha2
SUBDIRS = src
samples_DATA=
include_HEADERS= include/*.h

data_DATA = LICENSE config/module.xml

docs_DATA=

install-data-hook: 
	cp -rf xdocs/* $(docsdir)
	cp -rf samples/* $(samplesdir)
	sh copy_samples.sh $(prefix)


EXTRA_DIST=LICENSE config xdocs build samples ides
