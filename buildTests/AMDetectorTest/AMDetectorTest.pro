include ( $$PATH_TO_AM/compositeCommon/AMDetector.pri )

TARGET = AMDetectorTest

HEADERS *= \
	source/util/AMRange.h

SOURCES *= \
	buildTests/AMDetectorTest/AMDetectorTestMain.cpp \
	source/util/AMRange.cpp
