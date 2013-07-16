include ( acquamanCommon.pri )

TARGET = BareBonesAcquaman

FORMS +=

HEADERS += source/application/BareBones/BareBonesAppController.h \
    source/dataman/AMDbUpgrade1Pt3.h

SOURCES += source/application/BareBones/BareBonesAppController.cpp \
	source/application/BareBones/BareBonesMain.cpp \
    source/dataman/AMDbUpgrade1Pt3.cpp
