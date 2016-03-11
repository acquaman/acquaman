# PI Controller Driver Dependencies

# TODO: These will need to be altered for deploy to be under the shared srvunix directory.
PIC887_INCLUDE_DIR = $$PATH_TO_AM/contrib/pi_gcs2/include
PIC887_LIB = -L$$PATH_TO_AM/contrib/pi_gcs2/lib -lpi_pi_gcs2

INCLUDEPATH *= $$PIC887_INCLUDE_DIR

LIBS *= $$PIC887_LIB

CONFIG -= mobility

include ( compositeCommon/AMPVControl.pri )
include ( compositeCommon/AMControlSet.pri )

TARGET = AMPIC887DriverServer	

SOURCES *= \
	source/application/AMPIC887DriverServer/main.cpp \
	source/application/AMPIC887DriverServer/AMPIC887ControllerCollection.cpp \
	source/application/AMPIC887DriverServer/AMPIC887Controller.cpp \
	source/application/AMPIC887DriverServer/AMPIC887ConsoleCommandParser.cpp \
	source/application/AMPIC887DriverServer/AMPIC887ConsoleApplication.cpp \
	source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2MoveCommand.cpp \
	source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2Command.cpp \
	source/application/AMPIC887DriverServer/AMConsoleInputHandler.cpp \	
	source/application/AMPIC887DriverServer/AMAbstractConsoleCommandParser.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2StopCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2HaltSmoothlyCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetCommandLevelCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetSyntaxVersionCommand.cpp \
    source/application/AMPIC887DriverServer/AMPIC887DataRecorderConfiguration.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetDataRecorderConfigurationCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetCommandLevelCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetMovingStatusCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetNumberOfRecordedPointsCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetDeviceIdentificationCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetCurrentPositionCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetControllerReadyStatusCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetRecordedDataValuesCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetRecordTriggerSourceCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetRecordTriggerSourceCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2ReferenceMoveCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetReferenceResultCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetRecorderOptionsCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetAvailableParametersCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetLimitSwitchStatusCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetTargetPositionCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2MoveRelativeCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetLowSoftLimitsCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetLowSoftLimitsCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetOnTargetStateCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetHighSoftLimitsCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetHighSoftLimitsCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetPositionUnitsCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetCycleTimeCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetCycleTimeCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetPivotPointCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetPivotPointCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetSoftLimitsStatusCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetSoftLimitsStatusCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetServoModeCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetServoModeCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetStepSizeCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetStepSizeCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetMinCommandablePositionCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetMaxCommandablePositionCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetSystemVelocityCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetSystemVelocityCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2VirtualMoveCommand.cpp \
    source/application/AMPIC887DriverServer/AMPIC887ErrorMessageClearer.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2AsyncCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2AsyncReferenceMoveCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2AsyncMoveCommand.cpp \
    source/application/AMPIC887DriverServer/AMPIC887ControllerState.cpp \
    source/application/AMPIC887DriverServer/AMPIC887HexapodState.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2InitializeControllerStateCommand.cpp \
    source/application/AMPIC887DriverServer/AMPIC887DataRecorderState.cpp \
    source/application/AMPIC887DriverServer/AMPIC887DataRecorderTableState.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2AsyncMoveRelativeCommand.cpp \
    source/application/AMPIC887DriverServer/AMPIC887AxisCollection.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetRecordConfigurationCommand.cpp \
    source/application/AMPIC887DriverServer/AMPIC887EpicsCoordinator.cpp \
    source/application/AMPIC887DriverServer/AMPIC887TrajectoryCoordinator.cpp \
    source/application/AMPIC887DriverServer/AMPIC887HexapodPosition.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2AsyncGetRecordedDataValuesCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetRecordRateCommand.cpp \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetRecordRateCommand.cpp

HEADERS *= \
	source/application/AMPIC887DriverServer/AMPIC887ControllerCollection.h \	
	source/application/AMPIC887DriverServer/AMPIC887Controller.h \
	source/application/AMPIC887DriverServer/AMPIC887ConsoleCommandParser.h \
	source/application/AMPIC887DriverServer/AMPIC887ConsoleApplication.h \
	source/application/AMPIC887DriverServer/AMGCS2Support.h \
	source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2MoveCommand.h \
	source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2Command.h \
	source/application/AMPIC887DriverServer/AMGCS2.h \
	source/application/AMPIC887DriverServer/AMConsoleInputHandler.h \
	source/application/AMPIC887DriverServer/AMAbstractConsoleCommandParser.h \
	source/util/AMCArrayHandler.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2StopCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2HaltSmoothlyCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetCommandLevelCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetSyntaxVersionCommand.h \
    source/application/AMPIC887DriverServer/AMPIC887DataRecorderConfiguration.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetDataRecorderConfigurationCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetCommandLevelCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetMovingStatusCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetNumberOfRecordedPointsCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetDeviceIdentificationCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetCurrentPositionCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetControllerReadyStatusCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetRecordedDataValuesCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetRecordTriggerSourceCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetRecordTriggerSourceCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2ReferenceMoveCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetReferenceResultCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetRecorderOptionsCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetAvailableParametersCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetLimitSwitchStatusCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetTargetPositionCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2MoveRelativeCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetLowSoftLimitsCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetLowSoftLimitsCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetOnTargetStateCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetHighSoftLimitsCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetHighSoftLimitsCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetPositionUnitsCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetCycleTimeCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetCycleTimeCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetPivotPointCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetPivotPointCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetSoftLimitsStatusCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetSoftLimitsStatusCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetServoModeCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetServoModeCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetStepSizeCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetStepSizeCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetMinCommandablePositionCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetMaxCommandablePositionCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetSystemVelocityCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetSystemVelocityCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2VirtualMoveCommand.h \
    source/application/AMPIC887DriverServer/AMPIC887ErrorMessageClearer.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2AsyncCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2AsyncReferenceMoveCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2AsyncMoveCommand.h \
    source/application/AMPIC887DriverServer/AMPIC887ControllerState.h \
    source/application/AMPIC887DriverServer/AMPIC887HexapodState.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2InitializeControllerStateCommand.h \
    source/application/AMPIC887DriverServer/AMPIC887DataRecorderState.h \
    source/application/AMPIC887DriverServer/AMPIC887DataRecorderTableState.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2AsyncMoveRelativeCommand.h \
    source/application/AMPIC887DriverServer/AMPIC887AxisCollection.h \
    source/application/AMPIC887DriverServer/AMPIC887AxisMap.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetRecordConfigurationCommand.h \
    source/application/AMPIC887DriverServer/AMPIC887EpicsCoordinator.h \
    source/application/AMPIC887DriverServer/AMPIC887TrajectoryCoordinator.h \
    source/application/AMPIC887DriverServer/AMPIC887HexapodPosition.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2AsyncGetRecordedDataValuesCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2GetRecordRateCommand.h \
    source/application/AMPIC887DriverServer/GCS2Commands/AMGCS2SetRecordRateCommand.h

OTHER_FILES *= \
	db/BL1611-ID-1-HexapodAddon.db



























































































































































































































































