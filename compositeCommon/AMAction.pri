include( AMDbObject.pri )

DESTDIR = build

HEADERS *=\
	source/actions3/AMActionInfo3.h \
	source/actions3/AMAction3.h \
	source/util/AMErrorMonitor.h

SOURCES *= \
	source/actions3/AMActionInfo3.cpp \
	source/actions3/AMAction3.cpp \
	source/util/AMErrorMonitor.cpp
