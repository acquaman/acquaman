include ( $$PATH_TO_AM/compositeCommon/AMCommon.pri )
include ( $$PATH_TO_AM/compositeCommon/AMPVControl.pri )

TARGET = VESPERSBendingMirrors

FORMS +=

HEADERS += \
	source/ui/VESPERS/VESPERSBendingMirrorsView.h

SOURCES += \
	source/application/VESPERS/VESPERSBendingMirrorsMain.cpp \
	source/ui/VESPERS/VESPERSBendingMirrorsView.cpp





