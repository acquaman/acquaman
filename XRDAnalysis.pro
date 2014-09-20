include ( acquamanCommon.pri )

TARGET = XRDAnalaysis

FORMS +=

HEADERS += \
	source/application/XRDAnalysis/XRDAnalysisAppController.h \
	source/ui/XRDAnalysis/XRDAnalysisMainWindow.h

SOURCES += source/application/XRDAnalysis/XRDAnalysisMain.cpp \
	source/application/XRDAnalysis/XRDAnalysisAppController.cpp \
	source/ui/XRDAnalysis/XRDAnalysisMainWindow.cpp

RESOURCES += \
