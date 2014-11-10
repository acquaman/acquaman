include ( $$PATH_TO_AM/compositeCommon/AMCommon.pri )

QT *= network

TARGET = AMCrashReporter

HEADERS +=\
	source/application/AMCrashReporter.h

SOURCES += \
	source/application/AMCrashReporter.cpp \
	source/application/AMCrashReporterMain.cpp


