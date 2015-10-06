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
	source/beamline/SGM/monochromator/SGMGratingSupport.h \
    source/beamline/SGM/monochromator/SGMUndulatorSupport.h \
	source/beamline/SGM/monochromator/SGMEnergyTrajectory.h \
    $$PWD/source/beamline/SGM/monochromator/SGMEnergyPosition.h \
    $$PWD/source/beamline/SGM/monochromator/SGMEnergyControl.h


SOURCES += \
    source/beamline/SGM/SGMMAXvMotor.cpp \
    source/beamline/SGM/SGMBeamline.cpp \
	source/beamline/SGM/SGMHexapod.cpp \
	source/beamline/SGM/SGMHexapodTransformedAxis.cpp \
	source/beamline/SGM/monochromator/SGMEnergyTrajectory.cpp \
    $$PWD/source/beamline/SGM/monochromator/SGMEnergyPosition.cpp \
    $$PWD/source/beamline/SGM/monochromator/SGMEnergyControl.cpp





















