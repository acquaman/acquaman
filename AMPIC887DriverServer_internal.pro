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
	source/application/AMPIC887DriverServer/AMGCS2CurrentPositionCommand.cpp \
	source/application/AMPIC887DriverServer/AMGCS2CommandFactory.cpp \
	source/application/AMPIC887DriverServer/AMGCS2Command.cpp \
	source/application/AMPIC887DriverServer/AMConsoleInputHandler.cpp \	
	source/application/AMPIC887DriverServer/AMAbstractConsoleCommandParser.cpp \

HEADERS *= \
	source/application/AMPIC887DriverServer/AMPIC887ControllerCollection.h \	
	source/application/AMPIC887DriverServer/AMPIC887Controller.h \
	source/application/AMPIC887DriverServer/AMPIC887ConsoleCommandParser.h \
	source/application/AMPIC887DriverServer/AMPIC887ConsoleApplication.h \
	source/application/AMPIC887DriverServer/AMGCS2Support.h \
	source/application/AMPIC887DriverServer/AMGCS2MoveCommand.h \
	source/application/AMPIC887DriverServer/AMGCS2CurrentPositionCommand.h \
	source/application/AMPIC887DriverServer/AMGCS2CommandFactory.h \
	source/application/AMPIC887DriverServer/AMGCS2Command.h \
	source/application/AMPIC887DriverServer/AMGCS2.h \
	source/application/AMPIC887DriverServer/AMConsoleInputHandler.h \
	source/application/AMPIC887DriverServer/AMAbstractConsoleCommandParser.h \
	source/util/AMCArrayHandler.h

# PI Controller Driver Dependencies

# TODO: These will need to be altered for deploy to be under the shared srvunix directory.
PIC887_INCLUDE_DIR = $$PATH_TO_AM/contrib/pi_gcs2/include
PIC887_LIB = -L$$PATH_TO_AM/contrib/pi_gcs2/lib -lpi_pi_gcs2

INCLUDEPATH *= $$PIC887_INCLUDE_DIR

LIBS *= $$PIC887_LIB

CONFIG -= mobility
