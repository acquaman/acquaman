#ifndef SGMPLUGINSLOCATION_H
#define SGMPLUGINSLOCATION_H

#include "dataman/database/AMDbObject.h"

class SGMPluginsLocation : public AMDbObject
{
Q_OBJECT
	Q_PROPERTY(QString pluginFolderPath READ pluginFolderPath WRITE setPluginFolderPath)

public:
	explicit SGMPluginsLocation(QObject *parent = 0);

	QString pluginFolderPath() const;

public slots:
	void setPluginFolderPath(const QString &pluginFolderPath);

signals:
	void pluginFolderPathChanged(const QString &pluginFolderPath);

protected:
	QString pluginFolderPath_;
};

#endif // SGMPLUGINSLOCATION_H
