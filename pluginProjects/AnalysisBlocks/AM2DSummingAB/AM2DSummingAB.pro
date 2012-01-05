include(../AMAnalysisBlockPluginsCommon.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= AM2DSummingABPlugin.h \
		AM2DSummingABEditor.h
SOURCES		+= AM2DSummingABPlugin.cpp \
		AM2DSummingABEditor.cpp
TARGET          = $$qtLibraryTarget(AM2DSummingABPlugin)
DESTDIR         = $$HOME_FOLDER/$$DEV_PATH/acquaman/plugins/AnalysisBlocks

# install
target.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/AnalysisBlocks/AM2DSummingAB
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS AM2DSummingAB.pro
sources.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/AnalysisBlocks/AM2DSummingAB
INSTALLS += target sources
