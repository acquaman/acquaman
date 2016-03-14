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


#ifndef SGM2004FileLoader_H
#define SGM2004FileLoader_H

#include "dataman/AMAbstractFileLoader.h"
#include "util/AMBiHash.h"
#include "dataman/info/AMOldDetectorInfo.h"


class AMXASScan;

/// This class encapsulates loading and saving legacy SGM data files (XAS scans from 2004 to 2010) to/from an AMXASScan. It can be used for importing external files (by SGMImporter, where it is also used to extract meta-data and create channels), or used to load files that we already have in the database, which happen to store their data in this file format.
class SGM2004FileLoader : public AMAbstractFileLoader
{
public:
 	virtual ~SGM2004FileLoader();
	SGM2004FileLoader(AMXASScan* scan = 0);

	QString formatTag() const { return "sgm2004"; }

	/// load from SGM legacy file format
	bool loadFromFile(const QString& filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks);

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

	/// A list (in order) of the column names that our users might be interested in (ie: those which we should expose the raw data for, by default).  These names should match the column names in columns2pvNames_.
	QStringList defaultUserVisibleColumns_;

};



#endif // SGMLEGACYFILEINFO_H
