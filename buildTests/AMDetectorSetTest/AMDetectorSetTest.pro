include ( $$PATH_TO_AM/compositeCommon/AMDetectorSet.pri )

TARGET = AMDetectorSetTest

HEADERS *= \
	source/util/AMRange.h

SOURCES += \
	buildTests/AMDetectorSetTest/AMDetectorSetTestMain.cpp \
	source/util/AMRange.cpp
