#ifndef SGM2004FileLoader_H
#define SGM2004FileLoader_H

#include "dataman/AMAbstractFileLoader.h"
#include "AMBiHash.h"
#include "dataman/AMDetectorInfo.h"


class AMXASScan;

/// This class encapsulates loading and saving legacy SGM data files (XAS scans from 2004 to 2010) to/from an AMXASScan. It can be used for importing external files (by SGMImporter, where it is also used to extract meta-data and create channels), or used to load files that we already have in the database, which store their data in the legacy file format.
class SGM2004FileLoader : public AMAbstractFileLoader
{
public:
	SGM2004FileLoader(AMXASScan* scan = 0);

	QString formatTag() const { return "sgm2004"; }

	/// load from SGM legacy file format
	bool loadFromFile(const QString& filepath, bool extractMetaData = true, bool createChannels = true);

	/// save to SGM legacy file format (\todo UNIMPLEMENTED)
	bool saveToFile(const QString& filepath) {
		Q_UNUSED(filepath)
		return false;
	}

	/// set the target of this importer/exporter.
/*! There are two ways to attach an importer/exporter to a scan:
\code
AMXASScan myScan;
AMXASScan scan2;

SGM2004FileLoader myImporter(&myScan);
or...
myImporter = scan2;
\endcode
*/


	/// translate a PV string from the file header to a meaningful name, if we recognize it.  Unrecognized PV strings are untouched and false is returned.
	static bool pv2columnName(QString& pv) {
		QString newName = columns2pvNames_.valueR(pv);
		if(newName.isEmpty())
			return false;
		pv = newName;
		return true;
	}


protected:

	/// A forward- and reverse-mapping from meaningful data column names to process variable strings
	static AMBiHash<QString, QString> columns2pvNames_;
	QList<AMDetectorInfo*> sgmLegacyDetectors_;
	QStringList sgmLegacyDetectorNames_;
};



#endif // SGMLEGACYFILEINFO_H
