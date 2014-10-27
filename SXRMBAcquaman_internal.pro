include ( acquamanCommon.pri )

TARGET = SXRMBAcquaman

FORMS +=

HEADERS += \
    source/application/SXRMB/SXRMBAppController.h \
    source/beamline/SXRMB/SXRMBBeamline.h \
    source/beamline/SXRMB/SXRMBBrukerDetector.h \
    source/ui/SXRMB/SXRMBPersistentView.h \
    source/acquaman/SXRMB/SXRMBEXAFSScanConfiguration.h \
    source/ui/SXRMB/SXRMBEXAFSScanConfigurationView.h \
    source/acquaman/SXRMB/SXRMBEXAFSScanActionController.h

SOURCES += \
    source/application/SXRMB/SXRMBMain.cpp \
    source/application/SXRMB/SXRMBAppController.cpp \
    source/beamline/SXRMB/SXRMBBeamline.cpp \
    source/beamline/SXRMB/SXRMBBrukerDetector.cpp \
    source/ui/SXRMB/SXRMBPersistentView.cpp \
    source/acquaman/SXRMB/SXRMBEXAFSScanConfiguration.cpp \
    source/ui/SXRMB/SXRMBEXAFSScanConfigurationView.cpp \
    source/acquaman/SXRMB/SXRMBEXAFSScanActionController.cpp










