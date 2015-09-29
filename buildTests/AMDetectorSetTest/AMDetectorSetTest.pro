include ( $$PATH_TO_AM/compositeCommon/AMDetectorSet.pri )

TARGET = AMDetectorSetTest

HEADERS *= \
	source/util/AMRange.h \
	source/util/AMUtility.h

SOURCES += \
	buildTests/AMDetectorSetTest/AMDetectorSetTestMain.cpp \
	source/util/AMRange.cpp \
	source/util/AMUtility.cpp
