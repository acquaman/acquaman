include( AMDbObject.pri )

#QT *= core gui sql opengl network

DESTDIR = build

HEADERS *=\
	source/dataman/info/AMControlInfo.h \
	source/beamline/AMControl.h

SOURCES *= \
	source/dataman/info/AMControlInfo.cpp \
	source/beamline/AMControl.cpp
