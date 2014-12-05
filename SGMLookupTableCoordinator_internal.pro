include ( $$PATH_TO_AM/compositeCommon/AMCommon.pri )

include ( $$PATH_TO_AM/compositeCommon/AMPVControl.pri )
include ( $$PATH_TO_AM/compositeCommon/AMControlSet.pri )

TARGET = SGMLookupTableCoordinator

FORMS +=

HEADERS += \ 
    source/beamline/SGM/SGMLookupTableCoordinator.h

SOURCES += \
	source/application/SGMLookupTableCoordinator/SGMLookupTableCoordinatorMain.cpp \
	source/beamline/SGM/SGMLookupTableCoordinator.cpp


