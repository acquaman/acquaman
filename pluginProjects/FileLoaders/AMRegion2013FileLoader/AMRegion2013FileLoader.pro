include(../AMFileLoaderPluginsCommon.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= \
	AMRegion2013FileLoaderPlugin.h
SOURCES		+= \
	AMRegion2013FileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(AMRegion2013FileLoaderPlugin)
DESTDIR         = $$HOME_FOLDER/$$DEV_PATH/acquaman/plugins/FileLoaders

# install
target.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/AMRegion2013FileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS AMRegion2013FileLoader.pro
sources.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/AMRegion2013FileLoader
INSTALLS += target sources




