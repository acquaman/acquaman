include ( $$PATH_TO_AM/compositeCommon/AMCommon.pri )

include ( $$PATH_TO_AM/compositeCommon/AMPVControl.pri )
include ( $$PATH_TO_AM/compositeCommon/AMControlSet.pri )

TARGET = SGMAddOnsCoordinator

FORMS +=

HEADERS += \ 
	source/beamline/SGM/SGMAddOnsCoordinator.h

SOURCES += \
	source/application/SGMAddOnsCoordinator/SGMAddOnsCoordinatorMain.cpp \
	source/beamline/SGM/SGMAddOnsCoordinator.cpp




