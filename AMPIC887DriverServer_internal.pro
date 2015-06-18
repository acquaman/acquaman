include ( acquamanCommon.pri )

TARGET = AMPIC887DriverServer	

SOURCES *= \
	source/application/AMPIC887DriverServer/main.cpp \
	source/application/AMPIC887DriverServer/AMPIC887ControllerCollection.cpp \
	source/application/AMPIC887DriverServer/AMPIC887Controller.cpp \
	source/application/AMPIC887DriverServer/AMPIC887ConsoleCommandParser.cpp \
	source/application/AMPIC887DriverServer/AMPIC887ConsoleApplication.cpp \
	source/application/AMPIC887DriverServer/AMGCS2Support.cpp \
	source/application/AMPIC887DriverServer/AMGCS2MoveCommand.cpp \
	source/application/AMPIC887DriverServer/AMGCS2CommandFactory.cpp \
	source/application/AMPIC887DriverServer/AMGCS2Command.cpp \
	source/application/AMPIC887DriverServer/AMConsoleInputHandler.cpp \	
	source/application/AMPIC887DriverServer/AMAbstractConsoleCommandParser.cpp \
    source/application/AMPIC887DriverServer/AMGCS2StopCommand.cpp \
    source/application/AMPIC887DriverServer/AMGCS2HaltSmoothlyCommand.cpp \
    source/application/AMPIC887DriverServer/AMGCS2CompositeCommand.cpp \
    source/application/AMPIC887DriverServer/AMGCS2SetCommandLevelCommand.cpp \
    source/application/AMPIC887DriverServer/AMGCS2SetSyntaxVersionCommand.cpp \
    source/application/AMPIC887DriverServer/AMPIC887DataRecorderConfiguration.cpp \
    source/application/AMPIC887DriverServer/AMGCS2SetDataRecorderConfigurationCommand.cpp \
    source/application/AMPIC887DriverServer/AMGCS2GetCommandLevelCommand.cpp \
    source/application/AMPIC887DriverServer/AMGCS2GetMovingStatusCommand.cpp \
    source/application/AMPIC887DriverServer/AMGCS2GetNumberOfRecordedPointsCommand.cpp \
    source/application/AMPIC887DriverServer/AMGCS2GetDeviceIdentificationCommand.cpp \
    source/application/AMPIC887DriverServer/AMGCS2GetDataRecorderConfigurationCommand.cpp \
    source/application/AMPIC887DriverServer/AMGCS2GetCurrentPositionCommand.cpp \
    source/application/AMPIC887DriverServer/AMGCS2GetControllerReadyStatusCommand.cpp \
    source/application/AMPIC887DriverServer/AMGCS2GetRecordedDataValuesCommand.cpp \
    source/application/AMPIC887DriverServer/AMGCS2SetRecordTriggerSourceCommand.cpp \
    source/application/AMPIC887DriverServer/AMGCS2GetRecordTriggerSourceCommand.cpp \
    source/application/AMPIC887DriverServer/AMGCS2ReferenceMoveCommand.cpp \
    source/application/AMPIC887DriverServer/AMGCS2GetReferenceResultCommand.cpp \
    source/application/AMPIC887DriverServer/AMGCS2GetRecorderOptionsCommand.cpp \
    source/application/AMPIC887DriverServer/AMGCS2GetAvailableParametersCommand.cpp \
    source/application/AMPIC887DriverServer/AMGCS2GetLimitSwitchStatusCommand.cpp

HEADERS *= \
	source/application/AMPIC887DriverServer/AMPIC887ControllerCollection.h \	
	source/application/AMPIC887DriverServer/AMPIC887Controller.h \
	source/application/AMPIC887DriverServer/AMPIC887ConsoleCommandParser.h \
	source/application/AMPIC887DriverServer/AMPIC887ConsoleApplication.h \
	source/application/AMPIC887DriverServer/AMGCS2Support.h \
	source/application/AMPIC887DriverServer/AMGCS2MoveCommand.h \
	source/application/AMPIC887DriverServer/AMGCS2CommandFactory.h \
	source/application/AMPIC887DriverServer/AMGCS2Command.h \
	source/application/AMPIC887DriverServer/AMGCS2.h \
	source/application/AMPIC887DriverServer/AMConsoleInputHandler.h \
	source/application/AMPIC887DriverServer/AMAbstractConsoleCommandParser.h \
	source/util/AMCArrayHandler.h \
    source/application/AMPIC887DriverServer/AMGCS2StopCommand.h \
    source/application/AMPIC887DriverServer/AMGCS2HaltSmoothlyCommand.h \
    source/application/AMPIC887DriverServer/AMGCS2CompositeCommand.h \
    source/application/AMPIC887DriverServer/AMGCS2SetCommandLevelCommand.h \
    source/application/AMPIC887DriverServer/AMGCS2SetSyntaxVersionCommand.h \
    source/application/AMPIC887DriverServer/AMPIC887DataRecorderConfiguration.h \
    source/application/AMPIC887DriverServer/AMGCS2SetDataRecorderConfigurationCommand.h \
    source/application/AMPIC887DriverServer/AMGCS2GetCommandLevelCommand.h \
    source/application/AMPIC887DriverServer/AMGCS2GetMovingStatusCommand.h \
    source/application/AMPIC887DriverServer/AMGCS2GetNumberOfRecordedPointsCommand.h \
    source/application/AMPIC887DriverServer/AMGCS2GetDeviceIdentificationCommand.h \
    source/application/AMPIC887DriverServer/AMGCS2GetDataRecorderConfigurationCommand.h \
    source/application/AMPIC887DriverServer/AMGCS2GetCurrentPositionCommand.h \
    source/application/AMPIC887DriverServer/AMGCS2GetControllerReadyStatusCommand.h \
    source/application/AMPIC887DriverServer/AMGCS2GetRecordedDataValuesCommand.h \
    source/application/AMPIC887DriverServer/AMGCS2SetRecordTriggerSourceCommand.h \
    source/application/AMPIC887DriverServer/AMGCS2GetRecordTriggerSourceCommand.h \
    source/application/AMPIC887DriverServer/AMGCS2ReferenceMoveCommand.h \
    source/application/AMPIC887DriverServer/AMGCS2GetReferenceResultCommand.h \
    source/application/AMPIC887DriverServer/AMGCS2GetRecorderOptionsCommand.h \
    source/application/AMPIC887DriverServer/AMGCS2GetAvailableParametersCommand.h \
    source/application/AMPIC887DriverServer/AMGCS2GetLimitSwitchStatusCommand.h

# PI Controller Driver Dependencies

# TODO: These will need to be altered for deploy to be under the shared srvunix directory.
PIC887_INCLUDE_DIR = $$PATH_TO_AM/contrib/pi_gcs2/include
PIC887_LIB = -L$$PATH_TO_AM/contrib/pi_gcs2/lib -lpi_pi_gcs2

INCLUDEPATH *= $$PIC887_INCLUDE_DIR

LIBS *= $$PIC887_LIB

CONFIG -= mobility
























































































