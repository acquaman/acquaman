include ( $$PATH_TO_AM/clsCompositeCommon/CLSAcquamanCommon.pri )
include ( sgmCommon.pri )

TARGET = SGMSSAAcquaman

HEADERS += \
	source/application/SGMSSAApplication/SGMSSAWidget.h

SOURCES += \
	source/application/SGM/SGMSSAMain.cpp \
	source/application/SGMSSAApplication/SGMSSAWidget.cpp
