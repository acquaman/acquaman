include ( clsCommon.pri )

QT += testlib
TARGET = REIXSTest

HEADERS += source/dataman/REIXS/REIXSXESCalibration.h

SOURCES += source/tests/REIXS/REIXSTest.cpp \
                        source/dataman/REIXS/REIXSXESCalibration.cpp

