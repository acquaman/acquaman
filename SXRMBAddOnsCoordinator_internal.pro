include ( $$PATH_TO_AM/compositeCommon/AMCommon.pri )

include ( $$PATH_TO_AM/compositeCommon/AMPVControl.pri )
include ( $$PATH_TO_AM/compositeCommon/AMControlSet.pri )

TARGET = SXRMBAddOnsCoordinator

FORMS +=

HEADERS += \ 
	source/beamline/SXRMB/SXRMBAddOnsCoordinator.h

SOURCES += \
	source/application/SXRMBAddOnsCoordinator/SXRMBAddOnsCoordinatorMain.cpp \
	source/beamline/SXRMB/SXRMBAddOnsCoordinator.cpp




