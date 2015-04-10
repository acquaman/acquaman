include ( clsCommon.pri )
include ( bioxasCommon.pri )

TARGET = BioXASImagingAcquaman

FORMS +=

HEADERS += \
    source/application/BioXAS/BioXASImagingAppController.h \
    source/beamline/BioXAS/BioXASImagingBeamline.h

SOURCES += \
    source/application/BioXAS/BioXASImagingMain.cpp \
    source/application/BioXAS/BioXASImagingAppController.cpp \
    source/beamline/BioXAS/BioXASImagingBeamline.cpp
