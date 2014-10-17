include(../AMFileLoaderPluginsCommon.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= SGM2011XASFileLoaderPlugin.h
SOURCES		+= SGM2011XASFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(SGM2011XASFileLoaderPlugin)
DESTDIR         = $$PATH_TO_AM/plugins/FileLoaders

# install
target.path = $$PATH_TO_AM/pluginProjects/FileLoaders/SGM2011XASFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS SGM2011XASFileLoader.pro
sources.path = $$PATH_TO_AM/pluginProjects/FileLoaders/SGM2011XASFileLoader
INSTALLS += target sources
