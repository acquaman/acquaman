include(../AMFileLoaderPluginsCommonOld.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= VESPERS2011XASFileLoaderPlugin.h
SOURCES		+= VESPERS2011XASFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(VESPERS2011XASFileLoaderPlugin)
DESTDIR         = $$HOME_FOLDER/$$DEV_PATH/acquaman/plugins/FileLoaders

# install
target.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/VESPERS2011XASFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS VESPERS2011XASFileLoader.pro
sources.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/VESPERS2011XASFileLoader
INSTALLS += target sources
