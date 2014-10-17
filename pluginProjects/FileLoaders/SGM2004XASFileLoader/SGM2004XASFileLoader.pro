include(../AMFileLoaderPluginsCommonOld.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= SGM2004XASFileLoaderPlugin.h
SOURCES		+= SGM2004XASFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(SGM2004XASFileLoaderPlugin)
DESTDIR         = $$HOME_FOLDER/$$DEV_PATH/acquaman/plugins/FileLoaders

# install
target.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/SGM2004XASFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS SGM2004XASFileLoader.pro
sources.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/SGM2004XASFileLoader
INSTALLS += target sources
