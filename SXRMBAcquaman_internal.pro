include ( acquamanCommon.pri )

TARGET = SXRMBAcquaman

FORMS +=

HEADERS += \
    source/application/SXRMB/SXRMBAppController.h \
    source/beamline/SXRMB/SXRMBBeamline.h \
    source/beamline/SXRMB/SXRMBBrukerDetector.h

SOURCES += \
    source/application/SXRMB/SXRMBMain.cpp \
    source/application/SXRMB/SXRMBAppController.cpp \
    source/beamline/SXRMB/SXRMBBeamline.cpp \
    source/beamline/SXRMB/SXRMBBrukerDetector.cpp


