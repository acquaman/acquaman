#include "SGMPluginsLocation.h"

SGMPluginsLocation::SGMPluginsLocation(QObject *parent) :
	AMDbObject(parent)
{
}

QString SGMPluginsLocation::pluginFolderPath() const{
	return pluginFolderPath_;
}

void SGMPluginsLocation::setPluginFolderPath(const QString &pluginFolderPath){
	if(pluginFolderPath_ != pluginFolderPath){
		pluginFolderPath_ = pluginFolderPath;
		emit pluginFolderPathChanged(pluginFolderPath_);
	}
}
