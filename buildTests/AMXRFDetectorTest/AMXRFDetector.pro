include ( $$PATH_TO_AM/compositeCommon/AMXRFDetector.pri )

TARGET = AMXRFDetectorTest

HEADERS *= \
	source/util/AMRange.h

SOURCES += \
	buildTests/AMXRFDetectorTest/AMXRFDetectorTestMain.cpp \
	source/util/AMRange.cpp
