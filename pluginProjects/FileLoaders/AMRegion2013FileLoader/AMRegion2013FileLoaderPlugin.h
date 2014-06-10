#ifndef AMREGION2013FILELOADERPLUGIN_H
#define AMREGION2013FILELOADERPLUGIN_H

#include <QObject>
#include <QStringList>

#include "dataman/AMFileLoaderInterface.h"

#define AMREGION2013FILELOADERPLUGIN_CANNOT_OPEN_FILE 627001
#define AMREGION2013FILELOADERPLUGIN_NO_SPECTRA_FILE 627002
#define AMREGION2013FILELOADERPLUGIN_CANNOT_OPEN_SPECTRA_FILE 627003
#define AMREGION2013FILELOADERPLUGIN_DATA_COLUMN_MISMATCH 627004

class AMRegion2013FileLoaderPlugin : public AMFileLoaderInterface
{

public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept amRegionAscii2013.
	virtual QStringList acceptedFileFormats() { return QStringList() << "amRegionAscii2013"; }

	virtual bool accepts(AMScan *scan);

	virtual bool load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor);
};

class AMRegion2013FileLoaderFactory : public QObject, public AMFileLoaderFactory
{
	Q_OBJECT
	Q_INTERFACES(AMFileLoaderFactory)

public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept amRegionAscii2013.
	virtual QStringList acceptedFileFormats() { return QStringList() << "amRegionAscii2013"; }
	/// This function allows you to do a more detailed check of an AMScan to see whether you can load data for it.
	virtual bool accepts(AMScan *scan);

	/// This should create an instance of an AMFileLoaderInterface object to do the actual loading.
	virtual AMFileLoaderInterface* createFileLoader() { return new AMRegion2013FileLoaderPlugin(); }
};


#endif
