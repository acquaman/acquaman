include(../AMFileLoaderPluginsCommon.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= \
	AMRegion2013FileLoaderPlugin.h
SOURCES		+= \
	AMRegion2013FileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(AMRegion2013FileLoaderPlugin)
DESTDIR         = $$PATH_TO_AM/plugins/FileLoaders

# install
target.path = $$PATH_TO_AM/pluginProjects/FileLoaders/AMRegion2013FileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS AMRegion2013FileLoader.pro
sources.path = $$PATH_TO_AM/pluginProjects/FileLoaders/AMRegion2013FileLoader
INSTALLS += target sources




