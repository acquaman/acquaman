include(../AMFileLoaderPluginsCommonOld.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= VESPERS20122DFileLoaderPlugin.h
SOURCES		+= VESPERS20122DFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(VESPERS20122DFileLoaderPlugin)
DESTDIR         = $$HOME_FOLDER/$$DEV_PATH/acquaman/plugins/FileLoaders

# install
target.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/VESPERS20122DFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS VESPERS20122DFileLoader.pro
sources.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/VESPERS20122DFileLoader
INSTALLS += target sources
