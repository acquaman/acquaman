include(../AMFileLoaderPluginsCommon.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= SGM2004XASFileLoaderPlugin.h
SOURCES		+= SGM2004XASFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(SGM2004XASFileLoaderPlugin)
DESTDIR         = $$PATH_TO_AM/plugins/FileLoaders

# install
target.path = $$PATH_TO_AM/pluginProjects/FileLoaders/SGM2004XASFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS SGM2004XASFileLoader.pro
sources.path = $$PATH_TO_AM/pluginProjects/FileLoaders/SGM2004XASFileLoader
INSTALLS += target sources
