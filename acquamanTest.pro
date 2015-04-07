include ( clsCommon.pri )
include ( sgmCommon.pri )

QT += testlib
TARGET = AcquamanTest

HEADERS += \
	source/tests/TestDataman.h \
	source/tests/TestUi.h \
	source/tests/TestBeamline.h \
	source/tests/TestAcquaman.h

SOURCES += \
	source/tests/testMain.cpp


# Additional files from SGM for testing SGM scan controllers and configurations
# FORMS += 	source/ui/SGMXASScanConfigurationViewer.ui

#HEADERS += 	source/beamline/SGM/SGMBeamline.h \
#	source/beamline/SGM/SGMBeamlineInfo.h
#SOURCES += source/beamline/SGM/SGMBeamline.cpp \
#	source/beamline/SGM/SGMBeamlineInfo.cpp
