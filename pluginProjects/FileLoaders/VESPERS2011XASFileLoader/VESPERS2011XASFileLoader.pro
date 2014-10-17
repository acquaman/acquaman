include(../AMFileLoaderPluginsCommon.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= VESPERS2011XASFileLoaderPlugin.h
SOURCES		+= VESPERS2011XASFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(VESPERS2011XASFileLoaderPlugin)
DESTDIR         = $$PATH_TO_AM/plugins/FileLoaders

# install
target.path = $$PATH_TO_AM/pluginProjects/FileLoaders/VESPERS2011XASFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS VESPERS2011XASFileLoader.pro
sources.path = $$PATH_TO_AM/pluginProjects/FileLoaders/VESPERS2011XASFileLoader
INSTALLS += target sources
