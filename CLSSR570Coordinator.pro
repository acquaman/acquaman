include ( acquamanCommon.pri )

TARGET = CLSSR570CoordinatorAPP

HEADERS += \
	source/beamline/CLS/CLSSR570Coordinator.h

SOURCES += \
	source/application/CLSSR570Coordinator/CLSSR570CoordinatorMain.cpp \
	source/beamline/CLS/CLSSR570Coordinator.cpp

OTHER_FILES += \
	source/application/CLSSR570Coordinator/runCLSSR570CoordinatorStartup \
	source/application/CLSSR570Coordinator/clsSR570Coordinator \
    source/application/CLSSR570Coordinator/cpCLSSR570Coordinator
