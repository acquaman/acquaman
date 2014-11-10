include(../AMFileLoaderPluginsCommon.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= VESPERS20122DFileLoaderPlugin.h
SOURCES		+= VESPERS20122DFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(VESPERS20122DFileLoaderPlugin)
DESTDIR         = $$PATH_TO_AM/plugins/FileLoaders

# install
target.path = $$PATH_TO_AM/pluginProjects/FileLoaders/VESPERS20122DFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS VESPERS20122DFileLoader.pro
sources.path = $$PATH_TO_AM/pluginProjects/FileLoaders/VESPERS20122DFileLoader
INSTALLS += target sources
