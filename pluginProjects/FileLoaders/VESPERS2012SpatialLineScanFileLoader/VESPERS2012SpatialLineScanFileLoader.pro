include(../AMFileLoaderPluginsCommon.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= VESPERS2012SpatialLineScanFileLoaderPlugin.h
SOURCES		+= VESPERS2012SpatialLineScanFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(VESPERS2012SpatialLineScanFileLoaderPlugin)
DESTDIR         = $$HOME_FOLDER/$$DEV_PATH/acquaman/plugins/FileLoaders

# install
target.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/VESPERS2012SpatialLineScanFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS VESPERS2012SpatialLineScanFileLoader.pro
sources.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/VESPERS2012SpatialLineScanFileLoader
INSTALLS += target sources
