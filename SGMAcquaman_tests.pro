include ( clsCommon.pri )
include ( sgmCommon.pri )

TARGET = SGMAcquamanTests	

RESOURCES += \

HEADERS += \
    source/application/SGMTests/SGMTestsWindow.h \
    source/tests/SGM/TestSGMMonochromatorInfo.h \
    source/tests/SGM/SGMMonochromatorInfoTestView.h

SOURCES += \
    source/application/SGMTests/SGMTestsWindow.cpp \
    source/application/SGMTests/SGMTestsMain.cpp \
    source/tests/SGM/TestSGMMonochromatorInfo.cpp \
    source/tests/SGM/SGMMonochromatorInfoTestView.cpp










