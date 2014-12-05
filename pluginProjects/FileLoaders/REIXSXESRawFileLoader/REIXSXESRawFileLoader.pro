include(../AMFileLoaderPluginsCommon.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= REIXSXESRawFileLoaderPlugin.h
SOURCES		+= REIXSXESRawFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(REIXSXESRawFileLoaderPlugin)
DESTDIR         = $$PATH_TO_AM/plugins/FileLoaders

# install
target.path = $$PATH_TO_AM/pluginProjects/FileLoaders/REIXSXESRawFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS REIXSXESRawFileLoader.pro
sources.path = $$PATH_TO_AM/pluginProjects/FileLoaders/REIXSXESRawFileLoader
INSTALLS += target sources
