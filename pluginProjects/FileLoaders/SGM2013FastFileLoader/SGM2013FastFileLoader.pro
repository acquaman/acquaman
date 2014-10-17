include(../AMFileLoaderPluginsCommonOld.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= SGM2013FastFileLoaderPlugin.h
SOURCES		+= SGM2013FastFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(SGM2013FastFileLoaderPlugin)
DESTDIR         = $$HOME_FOLDER/$$DEV_PATH/acquaman/plugins/FileLoaders

# install
target.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/SGM2013FastFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS SGM2013FastFileLoader.pro
sources.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/SGM2013FastFileLoader
INSTALLS += target sources
