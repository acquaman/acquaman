include ( clsCommon.pri )

TARGET = IDEASAcquaman

FORMS +=

HEADERS +=	source/application/IDEAS/IDEASAppController.h \
	source/application/IDEAS/IDEAS.h \
	source/beamline/IDEAS/IDEASBeamline.h \
	source/ui/IDEAS/IDEASPersistentView.h \
	source/beamline/IDEAS/IDEASMonochromatorControl.h \
	source/acquaman/IDEAS/IDEASXASScanActionController.h \
	source/acquaman/IDEAS/IDEASXASScanConfiguration.h \
	source/ui/IDEAS/IDEASXASScanConfigurationView.h \
	source/beamline/IDEAS/IDEASKETEKDetector.h \
	source/ui/IDEAS/IDEASXRFDetailedDetectorView.h \
	source/ui/IDEAS/IDEASScalerView.h \
	source/ui/IDEAS/IDEASSampleCameraPanel.h \
        source/beamline/IDEAS/IDEASKETEKDetailedDetectorView.h \
        source/beamline/IDEAS/IDEAS13ElementGeDetailedDetectorView.h \
	source/beamline/IDEAS/IDEAS13ElementGeDetector.h \
	source/StripTool2/IDEAS/IDEASScalerStripTool.h \
    source/acquaman/IDEAS/IDEASScanConfiguration.h \
    source/acquaman/IDEAS/IDEASScanConfigurationDbObject.h \
    source/dataman/IDEAS/IDEASDbUpgrade1Pt1.h \
    source/acquaman/IDEAS/IDEAS2DScanConfiguration.h \
    source/acquaman/IDEAS/IDEAS2DScanActionController.h \
    source/ui/IDEAS/IDEAS2DScanConfigurationView.h \
    source/dataman/IDEAS/IDEASUserConfiguration.h

SOURCES +=	source/application/IDEAS/IDEASAppController.cpp \
	source/application/IDEAS/IDEASMain.cpp \
	source/beamline/IDEAS/IDEASBeamline.cpp \
	source/ui/IDEAS/IDEASPersistentView.cpp \
	source/beamline/IDEAS/IDEASMonochromatorControl.cpp \
	source/acquaman/IDEAS/IDEASXASScanActionController.cpp \
	source/acquaman/IDEAS/IDEASXASScanConfiguration.cpp \
	source/ui/IDEAS/IDEASXASScanConfigurationView.cpp \
	source/beamline/IDEAS/IDEASKETEKDetector.cpp \
	source/ui/IDEAS/IDEASXRFDetailedDetectorView.cpp \
	source/ui/IDEAS/IDEASScalerView.cpp \
	source/ui/IDEAS/IDEASSampleCameraPanel.cpp \
        source/beamline/IDEAS/IDEASKETEKDetailedDetectorView.cpp \
        source/beamline/IDEAS/IDEAS13ElementGeDetailedDetectorView.cpp \
        source/beamline/IDEAS/IDEAS13ElementGeDetector.cpp \
	source/StripTool2/IDEAS/IDEASScalerStripTool.cpp \
    source/acquaman/IDEAS/IDEASScanConfiguration.cpp \
    source/acquaman/IDEAS/IDEASScanConfigurationDbObject.cpp \
    source/dataman/IDEAS/IDEASDbUpgrade1Pt1.cpp \
    source/acquaman/IDEAS/IDEAS2DScanConfiguration.cpp \
    source/acquaman/IDEAS/IDEAS2DScanActionController.cpp \
    source/ui/IDEAS/IDEAS2DScanConfigurationView.cpp \
    source/dataman/IDEAS/IDEASUserConfiguration.cpp

#Striptool:
SOURCES +=\
	source/StripTool2/STWidget.cpp \
#		source/StripTool2/STMain.cpp \
	source/StripTool2/STPlotEditor.cpp \
	source/StripTool2/STPlotWidget.cpp \
	source/StripTool2/STVariable.cpp \
	source/StripTool2/STVariableEditor.cpp \
	source/StripTool2/STVariableCollection.cpp \
	source/StripTool2/STVariableCollectionEditor.cpp \
	source/StripTool2/STEditorDialog.cpp \
	source/StripTool2/STEditor.cpp \
	source/StripTool2/STTime.cpp \
	source/StripTool2/STListView.cpp \
	source/StripTool2/STColorButton.cpp \
	source/StripTool2/STPlotSelectorTool.cpp \
	source/StripTool2/ST0DTimestampAB.cpp

HEADERS  += \
	source/StripTool2/STWidget.h \
	source/StripTool2/STPlotEditor.h \
	source/StripTool2/STPlotWidget.h \
	source/StripTool2/STVariable.h \
	source/StripTool2/STVariableEditor.h \
	source/StripTool2/STVariableCollection.h \
	source/StripTool2/STVariableCollectionEditor.h \
	source/StripTool2/STEditorDialog.h \
	source/StripTool2/STEditor.h \
	source/StripTool2/STTime.h \
	source/StripTool2/STListView.h \
	source/StripTool2/STColorButton.h \
	source/StripTool2/STPlotSelectorTool.h \
	source/StripTool2/ST0DTimestampAB.h














