include( AMDbObject.pri )
include( AMCommonEPICS.pri )

QT *= core gui sql opengl network

DESTDIR = build

HEADERS +=\
	source/dataman/info/AMControlInfo.h \
	source/beamline/AMControl.h \
	source/beamline/AMPVControl.h \
	source/beamline/AMProcessVariable.h \
	source/beamline/AMProcessVariablePrivate.h \
	source/util/AMDeferredFunctionCall.h

SOURCES += \
	source/dataman/info/AMControlInfo.cpp \
	source/beamline/AMControl.cpp \
	source/beamline/AMPVControl.cpp \
	source/beamline/AMProcessVariable.cpp \
	source/beamline/AMProcessVariablePrivate.cpp \
	source/util/AMDeferredFunctionCall.cpp
