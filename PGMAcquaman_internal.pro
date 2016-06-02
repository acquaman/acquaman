include ( clsCommon.pri )

TARGET = PGMAcquaman

FORMS +=

HEADERS +=	source/application/PGM/PGMAppController.h \
	source/beamline/PGM/PGMBeamline.h \
	source/ui/PGM/PGMPersistentView.h \
	source/ui/PGM/PGMBladeCurrentView.h \
	source/ui/PGM/PGMHVControlViewBranchA.h \
	source/ui/PGM/PGMHVControlViewBranchB.h

SOURCES +=	source/application/PGM/PGMAppController.cpp \
	source/application/PGM/PGMMain.cpp \
	source/beamline/PGM/PGMBeamline.cpp \
	source/ui/PGM/PGMPersistentView.cpp \
	source/ui/PGM/PGMBladeCurrentView.cpp \
	source/ui/PGM/PGMHVControlViewBranchA.cpp \
	source/ui/PGM/PGMHVControlViewBranchB.cpp
