include ( acquamanCommon.pri )

TARGET = VESPERSDatabaseDuplicateEntryPatch

FORMS +=

HEADERS +=	\
	source/dataman/VESPERS/VESPERSDatabaseDuplicateEntryPatch.h \
    source/ui/VESPERS/VESPERSDatabaseDuplicateEntryPatchView.h \
    source/dataman/VESPERS/VESPERSDatabaseDuplicateEntryObject.h

SOURCES += \
	source/application/VESPERS/VESPERSDatabaseDuplicateEntryPatchMain.cpp \
	source/dataman/VESPERS/VESPERSDatabaseDuplicateEntryPatch.cpp \
    source/ui/VESPERS/VESPERSDatabaseDuplicateEntryPatchView.cpp \
    source/dataman/VESPERS/VESPERSDatabaseDuplicateEntryObject.cpp








































