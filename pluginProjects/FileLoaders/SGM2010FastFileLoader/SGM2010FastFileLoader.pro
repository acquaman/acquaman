include(../AMFileLoaderPluginsCommonOld.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= SGM2010FastFileLoaderPlugin.h
SOURCES		+= SGM2010FastFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(SGM2010FastFileLoaderPlugin)
DESTDIR         = $$HOME_FOLDER/$$DEV_PATH/acquaman/plugins/FileLoaders

# install
target.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/SGM2010FastFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS SGM2010FastFileLoader.pro
sources.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/SGM2010FastFileLoader
INSTALLS += target sources
