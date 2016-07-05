include ( clsCommon.pri )

TARGET = MockAcquaman

FORMS +=

HEADERS += \
    source/tests/mocks/beamline/AMMockBeamline.h \
    source/tests/mocks/application/AMMockAppController.h \
    source/tests/mocks/beamline/AMMockControl.h \
    source/tests/mocks/ui/AMMockPersistentView.h \
    source/tests/mocks/actions/AMMockMoveAction.h \
    source/tests/mocks/beamline/AMMockDetector.h


SOURCES += \
    source/tests/mocks/beamline/AMMockBeamline.cpp \
    source/tests/mocks/application/AMMockAppController.cpp \
    source/tests/mocks/application/AMMockMain.cpp \
    source/tests/mocks/beamline/AMMockControl.cpp \
    source/tests/mocks/ui/AMMockPersistentView.cpp \
    source/tests/mocks/actions/AMMockMoveAction.cpp \
    source/tests/mocks/beamline/AMMockDetector.cpp























