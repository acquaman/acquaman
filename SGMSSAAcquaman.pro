include ( acquamanCommon.pri )
include ( sgmCommon.pri )

TARGET = SGMSSAAcquaman

RESOURCES += \
	source/beamline/SGM/TransferHelpImages/LoadLockOutImages.qrc \
	source/beamline/SGM/TransferHelpImages/LoadLockInImages.qrc \
	source/beamline/SGM/TransferHelpImages/ChamberOutImages.qrc \
	source/beamline/SGM/TransferHelpImages/ChamberInImages.qrc

SOURCES += \
	source/application/SGM/SGMSSAMain.cpp
