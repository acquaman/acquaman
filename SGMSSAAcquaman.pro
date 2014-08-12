include ( acquamanCommon.pri )
include ( sgmCommon.pri )

#TARGET = SGMSSAAcquaman

#RESOURCES += \
#	source/beamline/SGM/TransferHelpImages/LoadLockOutImages.qrc \
#	source/beamline/SGM/TransferHelpImages/LoadLockInImages.qrc \
#	source/beamline/SGM/TransferHelpImages/ChamberOutImages.qrc \
#	source/beamline/SGM/TransferHelpImages/ChamberInImages.qrc

#SOURCES += \
#	source/application/SGM/SGMSSAMain.cpp

TARGET = SGMSSAAcquaman

HEADERS += \
#	source/ui/SGM/SGMSampleManipulatorView.h \
#	source/ui/dataman/AMSampleManagementPre2013Widget.h \
	source/application/SGMSSAApplication/SGMSSAWidget.h

SOURCES += \
	source/application/SGM/SGMSSAMain.cpp \
#	source/ui/SGM/SGMSampleManipulatorView.cpp \
#	source/ui/dataman/AMSampleManagementPre2013Widget.cpp \
	source/application/SGMSSAApplication/SGMSSAWidget.cpp
