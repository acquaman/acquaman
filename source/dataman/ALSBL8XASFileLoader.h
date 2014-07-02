/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef ALSBL8XASFileLoader_H
#define ALSBL8XASFileLoader_H

#include "dataman/AMAbstractFileLoader.h"
#include "util/AMBiHash.h"
#include "dataman/info/AMOldDetectorInfo.h"


class AMXASScan;

/// This class encapsulates loading and saving legacy ALS Beamline 8.0.1 absorption data files (scans from 200? to 2010) to/from an AMXASScan. It can be used for importing external files (by ALSBL8Importer, where it is also used to extract meta-data and create channels), or used to load files that we already have in the database, which happen to store their data in this file format.
class ALSBL8XASFileLoader : public AMAbstractFileLoader
{
public:
 	virtual ~ALSBL8XASFileLoader();
	ALSBL8XASFileLoader(AMXASScan* scan = 0);

	QString formatTag() const { return "alsBl8Xas"; }

	/// load from ALS BL 8.0.1 legacy file format
	bool loadFromFile(const QString& filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks);

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

	/// A list (in order) of the raw data columns we want to expose to the user by default
	QStringList defaultUserVisibleColumns_;

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
