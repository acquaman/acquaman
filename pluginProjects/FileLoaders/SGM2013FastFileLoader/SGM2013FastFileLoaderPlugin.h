#ifndef SGM2013FASTFILELOADERPLUGIN_H
#define SGM2013FASTFILELOADERPLUGIN_H

#include <QObject>
#include <QStringList>
#include "dataman/AMFileLoaderInterface.h"

#define SGM2013FASTFILELOADERPLUGIN_CANNOT_OPEN_FILE 628001
#define SGM2013FASTFILELOADERPLUGIN_NO_SPECTRA_FILE 628002
#define SGM2013FASTFILELOADERPLUGIN_CANNOT_OPEN_SPECTRA_FILE 628003
#define SGM2013FASTFILELOADERPLUGIN_DATA_COLUMN_MISMATCH 628004

class SGM2013FastFileLoaderPlugin : public AMFileLoaderInterface
{


public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept sgm2013Fast.
	virtual QStringList acceptedFileFormats() { return QStringList() << "sgm2013Fast"; }

	virtual bool accepts(AMScan *scan);

	virtual bool load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor);

protected:
};

class SGM2013FastFileLoaderFactory : public QObject, public AMFileLoaderFactory
{
	Q_OBJECT
	Q_INTERFACES(AMFileLoaderFactory)

public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept sgm2013Fast.
	virtual QStringList acceptedFileFormats() { return QStringList() << "sgm2013Fast"; }
	/// This function allows you to do a more detailed check of an AMScan to see whether you can load data for it.
	virtual bool accepts(AMScan *scan);

	/// This should create an instance of an AMFileLoaderInterface object to do the actual loading.
	virtual AMFileLoaderInterface* createFileLoader() { return new SGM2013FastFileLoaderPlugin(); }
};


#endif
