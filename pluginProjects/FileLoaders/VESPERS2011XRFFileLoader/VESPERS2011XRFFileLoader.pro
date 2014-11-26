include(../AMFileLoaderPluginsCommon.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= VESPERS2011XRFFileLoaderPlugin.h
SOURCES		+= VESPERS2011XRFFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(VESPERS2011XRFFileLoaderPlugin)
DESTDIR         = $$PATH_TO_AM/plugins/FileLoaders

# install
target.path = $$PATH_TO_AM/pluginProjects/FileLoaders/VESPERS2011XRFFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS VESPERS2011XRFFileLoader.pro
sources.path = $$PATH_TO_AM/pluginProjects/FileLoaders/VESPERS2011XRFFileLoader
INSTALLS += target sources
