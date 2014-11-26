include ( $$PATH_TO_AM/compositeCommon/AMCommon.pri )

include ( $$PATH_TO_AM/compositeCommon/AMPVControl.pri )
include ( $$PATH_TO_AM/compositeCommon/AMControlSet.pri )

TARGET = SGMAmptekCoordinator

FORMS +=

HEADERS += \
	source/beamline/SGM/SGMAmptekCoordinator.h

SOURCES += \
	source/application/SGMAmptekCoordinator/SGMAmptekCoordinatorMain.cpp \
	source/beamline/SGM/SGMAmptekCoordinator.cpp

