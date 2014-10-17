include(../AMFileLoaderPluginsCommonOld.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= ALSBL8XASFileLoaderPlugin.h
SOURCES		+= ALSBL8XASFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(ALSBL8XASFileLoaderPlugin)
DESTDIR         = $$HOME_FOLDER/$$DEV_PATH/acquaman/plugins/FileLoaders

# install
target.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/ALSBL8XASFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS ALSBL8XASFileLoader.pro
sources.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/ALSBL8XASFileLoader
INSTALLS += target sources
