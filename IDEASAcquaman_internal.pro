include ( acquamanCommon.pri )

TARGET = IDEASAcquaman

FORMS +=

HEADERS +=	source/application/IDEAS/IDEASAppController.h \
	source/beamline/IDEAS/IDEASBeamline.h \
	source/ui/IDEAS/IDEASPersistentView.h \
	source/beamline/IDEAS/IDEASMonochromatorControl.h \
	source/acquaman/IDEAS/IDEASXASScanActionController.h \
	source/acquaman/IDEAS/IDEASXASScanConfiguration.h \
	source/ui/IDEAS/IDEASXASScanConfigurationView.h \
	source/beamline/IDEAS/IDEASKETEKDetector.h \
	source/acquaman/IDEAS/IDEASXRFScanConfiguration.h \
	source/acquaman/IDEAS/IDEASXRFScanController.h \
	source/ui/IDEAS/IDEASXRFDetailedDetectorViewWithSave.h \
    source/ui/IDEAS/IDEASScalerView.h \
    source/ui/IDEAS/IDEASScanConfigurationViewHolder3.h \
    source/ui/IDEAS/IDEASSampleCameraPanel.h \
    source/beamline/IDEAS/IDEAS13ElementGeDetector.h

SOURCES +=	source/application/IDEAS/IDEASAppController.cpp \
	source/application/IDEAS/IDEASMain.cpp \
	source/beamline/IDEAS/IDEASBeamline.cpp \
	source/ui/IDEAS/IDEASPersistentView.cpp \
	source/beamline/IDEAS/IDEASMonochromatorControl.cpp \
	source/acquaman/IDEAS/IDEASXASScanActionController.cpp \
	source/acquaman/IDEAS/IDEASXASScanConfiguration.cpp \
	source/ui/IDEAS/IDEASXASScanConfigurationView.cpp \
	source/beamline/IDEAS/IDEASKETEKDetector.cpp \
	source/acquaman/IDEAS/IDEASXRFScanConfiguration.cpp \
	source/acquaman/IDEAS/IDEASXRFScanController.cpp \
	source/ui/IDEAS/IDEASXRFDetailedDetectorViewWithSave.cpp \
    source/ui/IDEAS/IDEASScalerView.cpp \
    source/ui/IDEAS/IDEASScanConfigurationViewHolder3.cpp \
    source/ui/IDEAS/IDEASSampleCameraPanel.cpp \
    source/beamline/IDEAS/IDEAS13ElementGeDetector.cpp
















