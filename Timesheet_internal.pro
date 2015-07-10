CONFIG -= mobility
QT *= network webkit
include ( acquamanCommon.pri )

TARGET = Timesheet	

SOURCES += \
    source/application/Timesheet/AMGoogleAuthTokenRequest.cpp \
    source/application/Timesheet/TimesheetMain.cpp \
    source/application/Timesheet/TimesheetMainWindow.cpp

HEADERS += \
    source/application/Timesheet/AMGoogleAuthTokenRequest.h \
    source/application/Timesheet/TimesheetMainWindow.h











