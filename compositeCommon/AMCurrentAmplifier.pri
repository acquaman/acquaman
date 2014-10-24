include ( $$PATH_TO_AM/compositeCommon/AMPVControl.pri )

DESTDIR = build

HEADERS *=\
	source/beamline/CLS/CLSSR570.h \
	source/beamline/AMCurrentAmplifier.h \
	source/ui/beamline/AMCurrentAmplifierView.h \
	source/ui/beamline/AMCurrentAmplifierSingleView.h

SOURCES *= \
	source/beamline/CLS/CLSSR570.cpp \
	source/beamline/AMCurrentAmplifier.cpp \
	source/ui/beamline/AMCurrentAmplifierView.cpp \
	source/ui/beamline/AMCurrentAmplifierSingleView.cpp

