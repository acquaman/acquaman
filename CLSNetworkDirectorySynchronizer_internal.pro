include ( $$PATH_TO_AM/compositeCommon/AMCommon.pri )

TARGET = CLSNetworkDirectorySynchronizer

HEADERS +=\
	source/application/CLSNetworkDirectorySynchronizer/CLSNetworkDirectorySynchronizer.h \
	source/util/AMDirectorySynchronizer.h \
	source/util/AMRecursiveDirectoryCompare.h \
	source/ui/util/AMDirectorySynchronizerDialog.h \
	source/ui/AMVerticalStackWidget.h \
	source/ui/AMHeaderButton.h \
	source/ui/util/AMMessageBoxWTimeout.h \
	source/util/AMThread.h

SOURCES += \
	source/application/CLSNetworkDirectorySynchronizer/CLSNetworkDirectorySynchronizerMain.cpp \
	source/application/CLSNetworkDirectorySynchronizer/CLSNetworkDirectorySynchronizer.cpp \
	source/util/AMDirectorySynchronizer.cpp \
	source/util/AMRecursiveDirectoryCompare.cpp \
	source/ui/util/AMDirectorySynchronizerDialog.cpp \
	source/ui/AMVerticalStackWidget.cpp \
	source/ui/AMHeaderButton.cpp \
	source/ui/util/AMMessageBoxWTimeout.cpp \
	source/util/AMThread.cpp
