include( AMDetectorInfo.pri )

DESTDIR = build

HEADERS *=\
	source/dataman/info/AMDetectorInfoSet.h \
	source/util/AMOrderedSet.h \
	source/util/AMOrderedSetSignalSource.h

SOURCES *= \
	source/dataman/info/AMDetectorInfoSet.cpp \
	source/util/AMOrderedSetSignalSource.cpp
