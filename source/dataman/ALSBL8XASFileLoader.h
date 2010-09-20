#ifndef ALSBL8XASFileLoader_H
#define ALSBL8XASFileLoader_H

#include "dataman/AMAbstractFileLoader.h"
#include "AMBiHash.h"
#include "dataman/AMDetectorInfo.h"


class AMXASScan;

/// This class encapsulates loading and saving legacy ALS Beamline 8.0.1 absorption data files (scans from 200? to 2010) to/from an AMXASScan. It can be used for importing external files (by ALSBL8Importer, where it is also used to extract meta-data and create channels), or used to load files that we already have in the database, which happen to store their data in this file format.
class ALSBL8XASFileLoader : public AMAbstractFileLoader
{
public:
	ALSBL8XASFileLoader(AMXASScan* scan = 0);

	QString formatTag() const { return "alsBl8Xas"; }

	/// load from ALS BL 8.0.1 legacy file format
	bool loadFromFile(const QString& filepath, bool extractMetaData = true, bool createChannels = true);

	/// save to ALS BL 8.0.1 legacy file format (\todo UNIMPLEMENTED)
	bool saveToFile(const QString& filepath) {
		Q_UNUSED(filepath)
		return false;
	}

	/// set the target of this importer/exporter.
/*! There are two ways to attach an importer/exporter to a scan:
\code
AMXASScan myScan;
AMXASScan scan2;

ALSBL8XASFileLoader myImporter(&myScan);
or...
myImporter = scan2;
\endcode
*/



protected:

	/// A forward- and reverse-mapping from Acquaman-style data column names to the file format's column headers
	static AMBiHash<QString, QString> columns2fileFormatHeaders_;

	/// translate a string from the file's column header to a meaningful name, if we recognize it.  Unrecognized strings are untouched and false is returned.
	static bool header2columnName(QString& source) {
		QString newName = columns2fileFormatHeaders_.valueR(source);
		if(newName.isEmpty())
			return false;
		source = newName;
		return true;
	}
};



#endif // ALSBL8XASFileLoader_H
