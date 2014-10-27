include ( acquamanCommon.pri )

TARGET = SXRMBAcquaman

FORMS +=

HEADERS += \
    source/application/SXRMB/SXRMBAppController.h \
    source/beamline/SXRMB/SXRMBBeamline.h \
    source/beamline/SXRMB/SXRMBBrukerDetector.h \
    source/acquaman/SXRMB/SXRMB2DMapScanConfiguration.h \
    source/acquaman/SXRMB/SXRMB2DScanActionController.h \
    source/ui/SXRMB/SXRMB2DMapScanConfigurationView.h

SOURCES += \
    source/application/SXRMB/SXRMBMain.cpp \
    source/application/SXRMB/SXRMBAppController.cpp \
    source/beamline/SXRMB/SXRMBBeamline.cpp \
    source/beamline/SXRMB/SXRMBBrukerDetector.cpp \
    source/acquaman/SXRMB/SXRMB2DMapScanConfiguration.cpp \
    source/acquaman/SXRMB/SXRMB2DScanActionController.cpp \
    source/ui/SXRMB/SXRMB2DMapScanConfigurationView.cpp








