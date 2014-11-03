include(../AMFileLoaderPluginsCommon.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= ALSBL8XASFileLoaderPlugin.h
SOURCES		+= ALSBL8XASFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(ALSBL8XASFileLoaderPlugin)
DESTDIR         = $$PATH_TO_AM/plugins/FileLoaders

# install
target.path = $$PATH_TO_AM/pluginProjects/FileLoaders/ALSBL8XASFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS ALSBL8XASFileLoader.pro
sources.path = $$PATH_TO_AM/pluginProjects/FileLoaders/ALSBL8XASFileLoader
INSTALLS += target sources
