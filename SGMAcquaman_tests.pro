include ( clsCommon.pri )
include ( sgmCommon.pri )

include ( $$PATH_TO_AM/compositeCommon/AMAcquamanDataServer.pri )

TARGET = SGMAcquamanTests

RESOURCES += \

HEADERS += \
    source/application/SGMTests/SGMTestsWindow.h \
    source/tests/SGM/SGMEnergyTrajectoryTestView.h \
    source/tests/SGM/SGMEnergyPositionTestView.h \
    source/tests/SGM/SGMEnergyPositionTest.h \
    source/tests/SGM/SGMEnergyControlTestView.h \
    source/tests/SGM/SGMGratingVelocityTests.h \
    source/tests/SGM/SGMUndulatorVelocityTests.h \
    source/tests/SGM/AMScalerAnalyserTestView.h

SOURCES += \
	source/application/SGMTests/SGMTestsMain.cpp \
    source/application/SGMTests/SGMTestsWindow.cpp \
    source/tests/SGM/SGMEnergyTrajectoryTestView.cpp \
    source/tests/SGM/SGMEnergyPositionTestView.cpp \
    source/tests/SGM/SGMEnergyPositionTest.cpp \
    source/tests/SGM/SGMEnergyControlTestView.cpp \
    source/tests/SGM/SGMGratingVelocityTests.cpp \
    source/tests/SGM/SGMUndulatorVelocityTests.cpp \
    source/tests/SGM/AMScalerAnalyserTestView.cpp










