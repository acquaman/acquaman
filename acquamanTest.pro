include ( acquamanCommon.pri )

QT += testlib
TARGET = AcquamanTest

HEADERS += \
	source/tests/TestDataman.h \
	source/tests/TestUi.h \
	source/tests/TestBeamline.h \
	source/tests/TestAcquaman.h

SOURCES += \
	source/tests/testMain.cpp
