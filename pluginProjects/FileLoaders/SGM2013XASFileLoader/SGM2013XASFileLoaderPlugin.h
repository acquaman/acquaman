#ifndef SGM2013XASFILELOADERPLUGIN_H
#define SGM2013XASFILELOADERPLUGIN_H

#include <QObject>
#include <QStringList>
#include "dataman/AMFileLoaderInterface.h"

class SGM2013XASFileLoaderPlugin : public AMFileLoaderInterface
{

public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept sgm2013XAS.
	virtual QStringList acceptedFileFormats() { return QStringList() << "sgm2013XAS"; }

	virtual bool accepts(AMScan *scan);

	virtual bool load(AMScan *scan, const QString &userDataFolder);

protected:
};

class SGM2013XASFileLoaderFactory : public QObject, public AMFileLoaderFactory
{
	Q_OBJECT
	Q_INTERFACES(AMFileLoaderFactory)

public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept sgm2013XAS.
	virtual QStringList acceptedFileFormats() { return QStringList() << "sgm2013XAS"; }
	/// This function allows you to do a more detailed check of an AMScan to see whether you can load data for it.
	virtual bool accepts(AMScan *scan);

	/// This should create an instance of an AMFileLoaderInterface object to do the actual loading.
	virtual AMFileLoaderInterface* createFileLoader() { return new SGM2013XASFileLoaderPlugin(); }
};


#endif
