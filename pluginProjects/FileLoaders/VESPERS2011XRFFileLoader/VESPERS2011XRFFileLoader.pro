include(../AMFileLoaderPluginsCommonOld.pri)
TEMPLATE        = lib
CONFIG		+= plugin
HEADERS		+= VESPERS2011XRFFileLoaderPlugin.h
SOURCES		+= VESPERS2011XRFFileLoaderPlugin.cpp
TARGET          = $$qtLibraryTarget(VESPERS2011XRFFileLoaderPlugin)
DESTDIR         = $$HOME_FOLDER/$$DEV_PATH/acquaman/plugins/FileLoaders

# install
target.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/VESPERS2011XRFFileLoader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS VESPERS2011XRFFileLoader.pro
sources.path = $$HOME_FOLDER/$$DEV_PATH/acquaman/pluginProjects/FileLoaders/VESPERS2011XRFFileLoader
INSTALLS += target sources
