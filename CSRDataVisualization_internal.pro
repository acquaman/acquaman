include ( $$PATH_TO_AM/compositeCommon/AMCommon.pri )

INCLUDEPATH *= $$PATH_TO_AM/source

TARGET = CSRDataVisualization

FORMS +=

HEADERS +=	\
	source/CSRDataVisualization/CSRAppController.h \
	source/CSRDataVisualization/CSRDataModel.h \
	source/CSRDataVisualization/CSRMainWindow.h \
	source/CSRDataVisualization/CSRFileConfiguration.h \
	source/CSRDataVisualization/CSRFileConfigurationView.h

SOURCES +=	\
	source/CSRDataVisualization/CSRDataVisualizationMain.cpp \
	source/CSRDataVisualization/CSRAppController.cpp \
	source/CSRDataVisualization/CSRDataModel.cpp \
	source/CSRDataVisualization/CSRMainWindow.cpp \
	source/CSRDataVisualization/CSRFileConfiguration.cpp \
	source/CSRDataVisualization/CSRFileConfigurationView.cpp





























