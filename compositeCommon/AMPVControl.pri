include( AMControl.pri )
include( AMCommonEPICS.pri )

QT *= core gui sql opengl network

DESTDIR = build

HEADERS *=\
	source/beamline/AMPVControl.h \
	source/beamline/AMProcessVariable.h \
	source/beamline/AMProcessVariablePrivate.h \
	source/util/AMDeferredFunctionCall.h

SOURCES *= \
	source/beamline/AMPVControl.cpp \
	source/beamline/AMProcessVariable.cpp \
	source/beamline/AMProcessVariablePrivate.cpp \
	source/util/AMDeferredFunctionCall.cpp
