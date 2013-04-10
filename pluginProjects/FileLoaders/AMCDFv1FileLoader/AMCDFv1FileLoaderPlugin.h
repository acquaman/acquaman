#ifndef AMCDFV1FILELOADERPLUGIN_H
#define AMCDFV1FILELOADERPLUGIN_H

#include <QObject>
#include <QStringList>

#define AMCDFV1FILELOADERPLUGIN_CANNOT_OPEN_FILE 627801
#define AMCDFV1FILELOADERPLUGIN_CANNOT_USE_FILE 627802


#include "dataman/AMFileLoaderInterface.h"

/// This class implements loading of CDF data files in the "amCDFv1" file format, as created natively by AMCDFDataStore. For documentation of the CDF file structure/layout, see AMCDFDataStore.

class AMCDFv1FileLoaderPlugin : public AMFileLoaderInterface
{
public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept 'amCDFv1'.
	virtual QStringList acceptedFileFormats() { return QStringList() << "amCDFv1"; }
	/// A more detailed way of determining whether we work for a given scan...
	virtual bool accepts(AMScan *scan);
	/// Load the data under AMScan::filePath() into the \c scan
	virtual bool load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor);


protected:

};

class AMCDFv1FileLoaderFactory : public QObject, public AMFileLoaderFactory
{
	Q_OBJECT
	Q_INTERFACES(AMFileLoaderFactory)

public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept 'amCDFv1'.
	virtual QStringList acceptedFileFormats() { return QStringList() << "amCDFv1"; }
	/// This function allows you to do a more detailed check of an AMScan to see whether you can load data for it.
	virtual bool accepts(AMScan *scan);

	/// This should create an instance of an AMFileLoaderInterface object to do the actual loading.
	virtual AMFileLoaderInterface* createFileLoader() { return new AMCDFv1FileLoaderPlugin(); }
};

#endif
