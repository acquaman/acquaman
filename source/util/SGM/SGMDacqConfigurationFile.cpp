#include "SGMDacqConfigurationFile.h"

SGMDacqConfigurationFile::SGMDacqConfigurationFile(QObject *parent) :
	AMDbObject(parent)
{
}

QString SGMDacqConfigurationFile::configurationFileName() const{
	return configurationFileName_;
}

QString SGMDacqConfigurationFile::configurationFilePath() const{
	return configurationFilePath_;
}

QString SGMDacqConfigurationFile::configurationFileFullPath() const{
	return configurationFilePath()+"/"+configurationFileName();
}

void SGMDacqConfigurationFile::setConfigurationFileName(const QString &configurationFileName){
	if(configurationFileName_ != configurationFileName){
		configurationFileName_ = configurationFileName;
		emit configurationFileNameChanged(configurationFileName_);
	}
}

void SGMDacqConfigurationFile::setConfigurationFilePath(const QString &configurationFilePath){
	if(configurationFilePath_ != configurationFilePath){
		configurationFilePath_ = configurationFilePath;
		emit configurationFilePathChanged(configurationFilePath_);
	}
}
