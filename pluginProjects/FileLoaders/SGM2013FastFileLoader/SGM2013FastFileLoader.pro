include(../AMFileLoaderPluginsCommon.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= SGM2013FastFileLoaderPlugin.h
SOURCES		+= SGM2013FastFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(SGM2013FastFileLoaderPlugin)
DESTDIR         = $$PATH_TO_AM/plugins/FileLoaders

# install
target.path = $$PATH_TO_AM/pluginProjects/FileLoaders/SGM2013FastFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS SGM2013FastFileLoader.pro
sources.path = $$PATH_TO_AM/pluginProjects/FileLoaders/SGM2013FastFileLoader
INSTALLS += target sources
