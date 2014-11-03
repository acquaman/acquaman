include(../AMFileLoaderPluginsCommon.pri)

TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= AMCDFv1FileLoaderPlugin.h
SOURCES		+= AMCDFv1FileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(AMCDFv1FileLoaderPlugin)
DESTDIR         = $$PATH_TO_AM/plugins/FileLoaders

# install
target.path = $$PATH_TO_AM/pluginProjects/FileLoaders/AMCDFv1FileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS AMCDFv1FileLoader.pro
sources.path = $$PATH_TO_AM/pluginProjects/FileLoaders/AMCDFv1FileLoader
INSTALLS += target sources
