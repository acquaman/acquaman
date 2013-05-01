#ifndef REIXSXESRAWFILELOADERPLUGIN_H
#define REIXSXESRAWFILELOADERPLUGIN_H

#include <QObject>
#include <QStringList>

#define REIXSXESRAWFILELOADERPLUGIN_CANNOT_OPEN_FILE 627701
#define REIXSXESRAWFILELOADERPLUGIN_CANNOT_SET_IMAGE_VALUE 627702
#define REIXSXESRAWFILELOADERPLUGIN_DATA_COLUMN_MISMATCH 627703

#include "dataman/AMFileLoaderInterface.h"

/// THIS CLASS IS DEPRECATED: We now use AMCDFv1FileLoader. This class implements loading and saving of raw XES data in the "reixsXESRaw" file format.  The file format is binary, big endian format, and consists of the following data:
/*!
 - Magic string, in ASCII encoding: "REIXSXESRAW2011.1"
 - Number of x pixels (32-bit integer)
 - Number of y pixels (32-bit integer)
 - all the pixels (32-bit integer each) starting from x=0, y=0, with y varying the fastest (ie: outer loop on x, inner loop on y)
 - A closing magic string, in ASCII encoding: "REIXSXESRAW2011.1"
 */
class REIXSXESRawFileLoaderPlugin : public AMFileLoaderInterface
{
public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept reixsXESRaw.
	virtual QStringList acceptedFileFormats() { return QStringList() << "reixsXESRaw"; }
	/// A more detailed way of determining whether we work for a given scan...
	virtual bool accepts(AMScan *scan);
	/// Load the data under AMScan::filePath() into the \c scan
	virtual bool load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor);


protected:

};

class REIXSXESRawFileLoaderFactory : public QObject, public AMFileLoaderFactory
{
	Q_OBJECT
	Q_INTERFACES(AMFileLoaderFactory)

public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept reixsXESRaw.
	virtual QStringList acceptedFileFormats() { return QStringList() << "reixsXESRaw"; }
	/// This function allows you to do a more detailed check of an AMScan to see whether you can load data for it.
	virtual bool accepts(AMScan *scan);

	/// This should create an instance of an AMFileLoaderInterface object to do the actual loading.
	virtual AMFileLoaderInterface* createFileLoader() { return new REIXSXESRawFileLoaderPlugin(); }
};

#endif
