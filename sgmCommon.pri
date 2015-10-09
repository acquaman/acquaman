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
    source/beamline/SGM/energy/SGMGratingSupport.h \
    source/beamline/SGM/energy/SGMUndulatorSupport.h \
    source/beamline/SGM/energy/SGMEnergyTrajectory.h \
	source/beamline/SGM/energy/SGMEnergyPosition.h \
	source/beamline/SGM/energy/SGMEnergyControl.h \
    source/beamline/SGM/energy/SGMGratingTranslationStepControl.h


SOURCES += \
    source/beamline/SGM/SGMMAXvMotor.cpp \
    source/beamline/SGM/SGMBeamline.cpp \
    source/beamline/SGM/SGMHexapod.cpp \
    source/beamline/SGM/SGMHexapodTransformedAxis.cpp \
    source/beamline/SGM/energy/SGMEnergyTrajectory.cpp \
	source/beamline/SGM/energy/SGMEnergyPosition.cpp \
	source/beamline/SGM/energy/SGMEnergyControl.cpp \
    source/beamline/SGM/energy/SGMGratingTranslationStepControl.cpp


































