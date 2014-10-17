include(../AMFileLoaderPluginsCommonOld.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= AMCDFv1FileLoaderPlugin.h
SOURCES		+= AMCDFv1FileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(AMCDFv1FileLoaderPlugin)
DESTDIR         = $$HOME_FOLDER/$$DEV_PATH/acquaman/plugins/FileLoaders

# install
target.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/AMCDFv1FileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS AMCDFv1FileLoader.pro
sources.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/AMCDFv1FileLoader
INSTALLS += target sources
