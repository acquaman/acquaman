#ifndef ALSBL8XASFileLoaderPLUGIN_H
#define ALSBL8XASFileLoaderPLUGIN_H

#include <QObject>
#include <QStringList>

#define ALSBL8XASFileLoaderPLUGIN_CANNOT_OPEN_FILE 627901
#define ALSBL8XASFileLoaderPLUGIN_BAD_FORMAT_NO_EVENT1_HEADER 627902
#define ALSBL8XASFileLoaderPLUGIN_BAD_FORMAT_NO_ENERGY_COLUMN 627903
#define ALSBL8XASFileLoaderPLUGIN_DATA_COLUMN_MISMATCH 627904

#include "dataman/AMFileLoaderInterface.h"
#include "util/AMBiHash.h"

/// This is a file loader for legacy data coming from Beamline 8.0.1 at the Advanced Light Source.
class ALSBL8XASFileLoaderPlugin : public AMFileLoaderInterface
{
public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept alsBl8Xas.
	virtual QStringList acceptedFileFormats() { return QStringList() << "alsBl8Xas"; }

	virtual bool accepts(AMScan *scan);

	virtual bool load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor);

protected:

	/// A forward- and reverse-mapping from Acquaman-style data column names to the file format's column headers
	AMBiHash<QString, QString> columns2fileFormatHeaders_;

	/// A list (in order) of the raw data columns we want to expose to the user by default
	QStringList defaultUserVisibleColumns_;

	/// translate a string from the file's column header to a meaningful name, if we recognize it.  Unrecognized strings are untouched and false is returned.
	bool header2columnName(QString& source) {
		QString newName = columns2fileFormatHeaders_.valueR(source);
		if(newName.isEmpty())
			return false;
		source = newName;
		return true;
	}

};

/// This is a file loader factory for legacy data coming from Beamline 8.0.1 at the Advanced Light Source.
class ALSBL8XASFileLoaderFactory : public QObject, public AMFileLoaderFactory
{
	Q_OBJECT
	Q_INTERFACES(AMFileLoaderFactory)

public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept alsBl8Xas.
	virtual QStringList acceptedFileFormats() { return QStringList() << "alsBl8Xas"; }
	/// This function allows you to do a more detailed check of an AMScan to see whether you can load data for it.
	virtual bool accepts(AMScan *scan);

	/// This should create an instance of an AMFileLoaderInterface object to do the actual loading.
	virtual AMFileLoaderInterface* createFileLoader() { return new ALSBL8XASFileLoaderPlugin(); }
};

#endif
