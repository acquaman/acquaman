include(../AMFileLoaderPluginsCommonOld.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= SGM2011XASFileLoaderPlugin.h
SOURCES		+= SGM2011XASFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(SGM2011XASFileLoaderPlugin)
DESTDIR         = $$HOME_FOLDER/$$DEV_PATH/acquaman/plugins/FileLoaders

# install
target.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/SGM2011XASFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS SGM2011XASFileLoader.pro
sources.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/SGM2011XASFileLoader
INSTALLS += target sources
