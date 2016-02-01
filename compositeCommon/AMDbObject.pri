include( AMCommon.pri )

QT *= sql

HEADERS *=\
	source/dataman/database/AMDbObject.h \
	source/dataman/database/AMDatabase.h \
	source/dataman/database/AMDbObjectSupport.h \
	source/dataman/database/AMConstDbObject.h \
	source/dataman/AMnDIndex.h \
	source/util/AMErrorMonitor.h \
	source/dataman/database/AMDbObjectDefinitions.h \
	source/acquaman.h \
	source/ui/util/AMMessageBoxWTimeout.h


SOURCES *= \
	source/dataman/database/AMDbObject.cpp \
	source/dataman/database/AMDatabase.cpp \
	source/dataman/database/AMDbObjectSupport.cpp \
	source/dataman/database/AMConstDbObject.cpp \
	source/dataman/AMnDIndex.cpp \
	source/util/AMErrorMonitor.cpp \
	source/acquaman.cpp \
	source/ui/util/AMMessageBoxWTimeout.cpp
