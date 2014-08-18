include( AMPVControl.pri )

QT *= core gui sql opengl network

DESTDIR = build

HEADERS *=\
	source/beamline/AMControlSet.h \
	source/dataman/info/AMControlInfoList.h \
	source/util/AMOrderedSet.h \
	source/util/AMOrderedSetSignalSource.h

SOURCES *= \
	source/beamline/AMControlSet.cpp \
	source/dataman/info/AMControlInfoList.cpp \
	source/util/AMOrderedSetSignalSource.cpp
