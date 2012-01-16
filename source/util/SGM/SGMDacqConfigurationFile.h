#ifndef SGMDACQCONFIGURATIONFILE_H
#define SGMDACQCONFIGURATIONFILE_H

#include "dataman/database/AMDbObject.h"

class SGMDacqConfigurationFile : public AMDbObject
{
Q_OBJECT
Q_PROPERTY(QString configurationFileName READ configurationFileName WRITE setConfigurationFileName)
Q_PROPERTY(QString configurationFilePath READ configurationFilePath WRITE setConfigurationFilePath)

public:
	explicit SGMDacqConfigurationFile(QObject *parent = 0);

	QString configurationFileName() const;
	QString configurationFilePath() const;

	QString configurationFileFullPath() const;

public slots:
	void setConfigurationFileName(const QString &configurationFileName);
	void setConfigurationFilePath(const QString &configurationFilePath);

signals:
	void configurationFileNameChanged(const QString &configurationFileName);
	void configurationFilePathChanged(const QString &configurationFilePath);

protected:
	QString configurationFileName_;
	QString configurationFilePath_;
};

#endif // SGMDACQCONFIGURATIONFILE_H
