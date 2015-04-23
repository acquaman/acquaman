include ( clsCommon.pri )
include ( sgmCommon.pri )

TARGET = SGMAcquaman	

RESOURCES += \
	source/beamline/SGM/TransferHelpImages/LoadLockOutImages.qrc \
	source/beamline/SGM/TransferHelpImages/LoadLockInImages.qrc \
	source/beamline/SGM/TransferHelpImages/ChamberOutImages.qrc \
	source/beamline/SGM/TransferHelpImages/ChamberInImages.qrc

SOURCES += \
	source/application/SGM/SGMMain.cpp
