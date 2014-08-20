include( AMControl.pri )
include( AMControlInfoList.pri )

DESTDIR = build

HEADERS *=\
	source/beamline/AMControlSet.h \
	source/util/AMOrderedSet.h

SOURCES *= \
	source/beamline/AMControlSet.cpp
