include ( acquamanCommon.pri )

include ( $$PATH_TO_AM/compositeCommon/AMCommon.pri )

include ( $$PATH_TO_AM/compositeCommon/AMMPlot.pri )

include ( $$PATH_TO_AM/compositeCommon/AMPVControl.pri )
include ( $$PATH_TO_AM/compositeCommon/AMControlSet.pri )

include ( $$PATH_TO_AM/compositeCommon/AMGSL.pri )

CONFIG += mobility
MOBILITY += multimedia

TARGET = REIXSXESLoadLockControl

FORMS +=

HEADERS += \ 
        source/ui/REIXS/REIXSXESLoadlockControl.h

SOURCES += \
	source/ui/REIXS/REIXSXESLoadlockControl.cpp \
        source/application/REIXSXESLoadlockControl/REIXSXESLoadlockControlMain.cpp



# Required for camera widget

#HEADERS += source/ui/dataman/AMColorPickerButton.h \
#        source/ui/AMCrosshairOverlayVideoWidget.h \
#       source/ui/AMOverlayVideoWidget.h \
#        source/ui/AMBeamlineCameraWidget.h \
#        source/ui/AMBeamlineCameraBrowser.h

#SOURCES +=	source/application/AcquaCam/AcquaCamMain.cpp \
#        source/ui/dataman/AMColorPickerButton.cpp \
#        source/ui/AMCrosshairOverlayVideoWidget.cpp \
#        source/ui/AMOverlayVideoWidget.cpp \
#        source/ui/AMBeamlineCameraWidget.cpp \
#        source/ui/AMBeamlineCameraBrowser.cpp
