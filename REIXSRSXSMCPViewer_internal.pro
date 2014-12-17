include ( $$PATH_TO_AM/compositeCommon/AMCommon.pri )

include ( $$PATH_TO_AM/compositeCommon/AMMPlot.pri )

include ( $$PATH_TO_AM/compositeCommon/AMPVControl.pri )
include ( $$PATH_TO_AM/compositeCommon/AMControlSet.pri )

TARGET = REIXSRSXSMCPViewer

FORMS +=

HEADERS += \ 
	source/dataman/datasource/REIXS/REIXSRSXSMCPDataSource.h \
	source/ui/REIXS/REIXSRSXSMCPDetectorView.h \
	source/dataman/datasource/AMDataSource.h \
	source/dataman/AMNumber.h \
	source/dataman/AMAxisInfo.h \
	source/dataman/datasource/AMDataSourceImageData.h \
	source/ui/dataman/AMImagePropertyEditor.h \
	source/ui/dataman/AMColorPickerButton.h \
    source/ui/REIXS/REIXSRSXSMCPDetectorFullView.h

SOURCES += \
	source/dataman/datasource/REIXS/REIXSRSXSMCPDataSource.cpp \
	source/ui/REIXS/REIXSRSXSMCPDetectorView.cpp \
	source/application/REIXSRSXSMCPViewer/REIXSRSXSMCPViewerMain.cpp \
	source/dataman/datasource/AMDataSource.cpp \
	source/dataman/AMNumber.cpp \
	source/dataman/AMAxisInfo.cpp \
	source/dataman/datasource/AMDataSourceImageData.cpp \
	source/ui/dataman/AMImagePropertyEditor.cpp \
	source/ui/dataman/AMColorPickerButton.cpp \
    source/ui/REIXS/REIXSRSXSMCPDetectorFullView.cpp




