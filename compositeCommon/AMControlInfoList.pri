include( AMControlInfo.pri )

DESTDIR = build

HEADERS *=\
	source/dataman/info/AMControlInfoList.h \
	source/util/AMOrderedList.h \
	source/util/AMOrderedSetSignalSource.h

SOURCES *= \
	source/dataman/info/AMControlInfoList.cpp \
	source/util/AMOrderedSetSignalSource.cpp
