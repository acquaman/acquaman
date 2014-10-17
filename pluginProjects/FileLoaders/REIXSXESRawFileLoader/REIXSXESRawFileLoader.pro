include(../AMFileLoaderPluginsCommonOld.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= REIXSXESRawFileLoaderPlugin.h
SOURCES		+= REIXSXESRawFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(REIXSXESRawFileLoaderPlugin)
DESTDIR         = $$HOME_FOLDER/$$DEV_PATH/acquaman/plugins/FileLoaders

# install
target.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/REIXSXESRawFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS REIXSXESRawFileLoader.pro
sources.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/REIXSXESRawFileLoader
INSTALLS += target sources
