include(../AMFileLoaderPluginsCommon.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= VESPERS2012SpatialLineScanFileLoaderPlugin.h
SOURCES		+= VESPERS2012SpatialLineScanFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(VESPERS2012SpatialLineScanFileLoaderPlugin)
DESTDIR         = $$PATH_TO_AM/plugins/FileLoaders

# install
target.path = $$PATH_TO_AM/pluginProjects/FileLoaders/VESPERS2012SpatialLineScanFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS VESPERS2012SpatialLineScanFileLoader.pro
sources.path = $$PATH_TO_AM/pluginProjects/FileLoaders/VESPERS2012SpatialLineScanFileLoader
INSTALLS += target sources
