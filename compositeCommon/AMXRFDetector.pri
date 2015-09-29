include( AMDetector.pri )
include( AMPVControl.pri )

DESTDIR = build

HEADERS *=\
	source/beamline/AMXRFDetector.h \
	source/dataman/AMAnalysisBlock.h \
	source/dataman/AMRegionOfInterest.h \
	source/dataman/datasource/AMProcessVariableDataSource.h \
	source/analysis/AM1DSummingAB.h \
	source/analysis/AMnDDeadTimeAB.h \
	source/analysis/AMRegionOfInterestAB.h \
	source/analysis/AMStandardAnalysisBlock.h \
	source/ui/dataman/AMSimpleDataSourceEditor.h

SOURCES *= \
	source/beamline/AMXRFDetector.cpp \
	source/dataman/AMAnalysisBlock.cpp \
	source/dataman/AMRegionOfInterest.cpp \
	source/dataman/datasource/AMProcessVariableDataSource.cpp \
	source/analysis/AM1DSummingAB.cpp \
	source/analysis/AMnDDeadTimeAB.cpp \
	source/analysis/AMRegionOfInterestAB.cpp \
	source/analysis/AMStandardAnalysisBlock.cpp \
	source/ui/dataman/AMSimpleDataSourceEditor.cpp
