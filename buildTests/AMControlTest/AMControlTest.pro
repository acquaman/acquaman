#QMAKEFEATURES *= /Users/chevrid/beamline/programming/acquaman_4_8/compositeCommon
QMAKEFEATURES *= ./

message($$QMAKEFEATURES)
message($$PWD)

include ( ../../compositeCommon/AMControl.pri )
load( AMAllCommon )

TARGET = AMControlTest

SOURCES += \
	buildTests/AMControlTest/AMControlTestMain.cpp
