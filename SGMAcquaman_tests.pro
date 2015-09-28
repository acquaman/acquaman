include ( clsCommon.pri )
include ( sgmCommon.pri )

TARGET = SGMAcquamanTests

RESOURCES += \

HEADERS += \
    source/application/SGMTests/SGMTestsWindow.h \
    source/tests/SGM/SGMMonochromatorInfoTestView.h \
    source/tests/SGM/SGMEnergyTrajectoryTestView.h \
    source/tests/SGM/SGMMonochromatorInfoTest.h \
    source/tests/AMVelocityProfileTest.h

SOURCES += \
    source/application/SGMTests/SGMTestsWindow.cpp \
    source/application/SGMTests/SGMTestsMain.cpp \
    source/tests/SGM/SGMMonochromatorInfoTestView.cpp \
    source/tests/SGM/SGMEnergyTrajectoryTestView.cpp \
    source/tests/SGM/SGMMonochromatorInfoTest.cpp \
    source/tests/AMVelocityProfileTest.cpp










