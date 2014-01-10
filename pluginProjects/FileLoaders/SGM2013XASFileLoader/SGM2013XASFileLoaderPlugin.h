#ifndef SGM2013XASFILELOADERPLUGIN_H
#define SGM2013XASFILELOADERPLUGIN_H

#include <QObject>
#include <QStringList>
#include "dataman/AMFileLoaderInterface.h"

#define SGM2013XASFILELOADERPLUGIN_CANNOT_OPEN_FILE 627001
#define SGM2013XASFILELOADERPLUGIN_NO_SPECTRA_FILE 627002
#define SGM2013XASFILELOADERPLUGIN_CANNOT_OPEN_SPECTRA_FILE 627003
#define SGM2013XASFILELOADERPLUGIN_DATA_COLUMN_MISMATCH 627004

class SGM2013XASFileLoaderPlugin : public AMFileLoaderInterface
{

public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept sgm2013XAS.
	virtual QStringList acceptedFileFormats() { return QStringList() << "sgm2013XAS" << "amRegionAscii2013"; }

	virtual bool accepts(AMScan *scan);

	virtual bool load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor);

protected:
};

class SGM2013XASFileLoaderFactory : public QObject, public AMFileLoaderFactory
{
	Q_OBJECT
	Q_INTERFACES(AMFileLoaderFactory)

public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept sgm2013XAS.
	virtual QStringList acceptedFileFormats() { return QStringList() << "sgm2013XAS" << "amRegionAscii2013"; }
	/// This function allows you to do a more detailed check of an AMScan to see whether you can load data for it.
	virtual bool accepts(AMScan *scan);

	/// This should create an instance of an AMFileLoaderInterface object to do the actual loading.
	virtual AMFileLoaderInterface* createFileLoader() { return new SGM2013XASFileLoaderPlugin(); }
};


#endif
