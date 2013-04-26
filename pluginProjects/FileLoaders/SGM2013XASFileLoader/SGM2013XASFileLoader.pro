include(../AMFileLoaderPluginsCommon.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= SGM2013XASFileLoaderPlugin.h
SOURCES		+= SGM2013XASFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(SGM2013XASFileLoaderPlugin)
DESTDIR         = $$HOME_FOLDER/$$DEV_PATH/acquaman/plugins/FileLoaders

# install
target.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/SGM2013XASFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS SGM2013XASFileLoader.pro
sources.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/SGM2013XASFileLoader
INSTALLS += target sources
