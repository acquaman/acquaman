include ( $$PATH_TO_AM/compositeCommon/AMCommon.pri )

include ( $$PATH_TO_AM/compositeCommon/AMPVControl.pri )
include ( $$PATH_TO_AM/compositeCommon/AMControlSet.pri )

TARGET = VESPERSAddOnsCoordinator

FORMS +=

HEADERS += \
	source/beamline/VESPERS/VESPERSAddOnsCoordinator.h

SOURCES += \
	source/application/VESPERS/VESPERSAddOnsCoordinatorMain.cpp \
	source/beamline/VESPERS/VESPERSAddOnsCoordinator.cpp




