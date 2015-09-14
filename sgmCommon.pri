# #####################################################################
# QMake project file for SGM common files  	January 2012. david.k.chevrier@gmail.com
# ####################################################################

# Source Files (SGM Common)
#######################

FORMS += 

HEADERS += \
    source/beamline/SGM/SGMMAXvMotor.h \
    source/beamline/SGM/SGMBeamline.h \
    source/beamline/SGM/SGMHexapod.h \
    source/beamline/SGM/SGMHexapodTransformedAxis.h \
    $$PWD/source/beamline/SGM/SGMGratingTranslation.h \
	source/beamline/SGM/monochromator/SGMMonochromatorSupport.h \
	source/beamline/SGM/monochromator/SGMGratingSupport.h \
    source/beamline/SGM/monochromator/SGMUndulatorSupport.h


SOURCES += \
    source/beamline/SGM/SGMMAXvMotor.cpp \
    source/beamline/SGM/SGMBeamline.cpp \
	source/beamline/SGM/SGMHexapod.cpp \
    source/beamline/SGM/SGMHexapodTransformedAxis.cpp \
    $$PWD/source/beamline/SGM/SGMGratingTranslation.cpp


















