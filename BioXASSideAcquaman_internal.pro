include ( acquamanCommon.pri )
include ( bioxasSideCommon.pri )

TARGET = BioXASSideAcquaman

#DEPENDPATH += source/StripTool2/
#INCLUDEPATH +=  ../testLib
#LIBS+=  -L../testLib/debug -ltestLib

FORMS +=

HEADERS += \
	source/application/BioXAS/BioXASSideAppController.h \
	source/ui/BioXAS/BioXASSidePersistentView.h \
	source/acquaman/BioXASSide/BioXASSideXASScanActionController.h \
	source/acquaman/BioXASSide/BioXASSideXASScanConfiguration.h \
	source/ui/BioXAS/BioXASSideXASScanConfigurationView.h \
    source/acquaman/BioXASSide/BioXASSideXRFScanConfiguration.h \
    source/acquaman/BioXASSide/BioXASSideXRFScanController.h

SOURCES += \
	source/application/BioXAS/BioXASSideMain.cpp \
	source/application/BioXAS/BioXASSideAppController.cpp \
	source/ui/BioXAS/BioXASSidePersistentView.cpp \
	source/acquaman/BioXASSide/BioXASSideXASScanActionController.cpp \
	source/acquaman/BioXASSide/BioXASSideXASScanConfiguration.cpp \
	source/ui/BioXAS/BioXASSideXASScanConfigurationView.cpp \
    source/acquaman/BioXASSide/BioXASSideXRFScanConfiguration.cpp \
    source/acquaman/BioXASSide/BioXASSideXRFScanController.cpp




