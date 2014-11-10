include(../AMFileLoaderPluginsCommon.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= SGM2010FastFileLoaderPlugin.h
SOURCES		+= SGM2010FastFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(SGM2010FastFileLoaderPlugin)
DESTDIR         = $$PATH_TO_AM/plugins/FileLoaders

# install
target.path = $$PATH_TO_AM/pluginProjects/FileLoaders/SGM2010FastFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS SGM2010FastFileLoader.pro
sources.path = $$PATH_TO_AM/pluginProjects/FileLoaders/SGM2010FastFileLoader
INSTALLS += target sources
