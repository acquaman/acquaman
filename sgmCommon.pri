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
	source/beamline/SGM/energy/SGMGratingTranslationStepControl.h \
	source/beamline/SGM/energy/SGMGratingAngleControl.h \
	source/beamline/SGM/energy/SGMEnergyControlSet.h \
	source/beamline/SGM/energy/SGMUndulatorControl.h \
	source/beamline/SGM/energy/SGMEnergyCoordinatorControl.h \
	source/beamline/SGM/energy/SGMEnergyPVControl.h \
	source/beamline/SGM/SGMXPSLadder.h \
	source/beamline/SGM/SGMLadderControl.h \
	source/beamline/SGM/SGMBypassLadder.h \
	source/beamline/SGM/SGMXASLadder.h \
	source/ui/SGM/SGMLaddersView.h \
	source/beamline/SGM/energy/SGMExitSlitSupport.h \
	source/beamline/SGM/SGMBeamCoordinatorControl.h \
	source/beamline/SGM/SGMNewBeamOnControls.h \
	source/beamline/SGM/SGMVATValve.h \
	source/beamline/SGM/SGMSampleChamberVacuum.h \
	source/ui/SGM/SGMVATValveView.h \
	source/beamline/SGM/SGMTurboPump.h \
	source/beamline/SGM/SGMSampleChamber.h \
	source/ui/SGM/SGMSampleChamberView.h \
	source/beamline/SGM/SGMRoughingPump.h \
	source/beamline/SGM/SGMRoughingPumpRunningStatus.h \
	source/beamline/SGM/SGMVATValveState.h \
	source/ui/SGM/SGMChamberLightSlider.h

SOURCES += \
	source/beamline/SGM/SGMMAXvMotor.cpp \
	source/beamline/SGM/SGMBeamline.cpp \
	source/beamline/SGM/SGMHexapod.cpp \
	source/beamline/SGM/SGMHexapodTransformedAxis.cpp \
	source/beamline/SGM/energy/SGMEnergyTrajectory.cpp \
	source/beamline/SGM/energy/SGMEnergyPosition.cpp \
	source/beamline/SGM/energy/SGMGratingTranslationStepControl.cpp \
	source/beamline/SGM/energy/SGMGratingAngleControl.cpp \
	source/beamline/SGM/energy/SGMEnergyControlSet.cpp \
	source/beamline/SGM/energy/SGMUndulatorControl.cpp \
	source/beamline/SGM/energy/SGMEnergyCoordinatorControl.cpp \
	source/beamline/SGM/energy/SGMEnergyPVControl.cpp \
	source/beamline/SGM/SGMXPSLadder.cpp \
	source/beamline/SGM/SGMLadderControl.cpp \
	source/beamline/SGM/SGMBypassLadder.cpp \
	source/beamline/SGM/SGMXASLadder.cpp \
	source/ui/SGM/SGMLaddersView.cpp \
	source/beamline/SGM/SGMBeamCoordinatorControl.cpp \
	source/beamline/SGM/SGMNewBeamOnControls.cpp \
	source/beamline/SGM/SGMVATValve.cpp \
	source/beamline/SGM/SGMSampleChamberVacuum.cpp \
	source/ui/SGM/SGMVATValveView.cpp \
	source/beamline/SGM/SGMTurboPump.cpp \
	source/beamline/SGM/SGMSampleChamber.cpp \
	source/ui/SGM/SGMSampleChamberView.cpp \
	source/beamline/SGM/SGMRoughingPump.cpp \
	source/beamline/SGM/SGMRoughingPumpRunningStatus.cpp \
	source/beamline/SGM/SGMVATValveState.cpp \
	source/ui/SGM/SGMChamberLightSlider.cpp

