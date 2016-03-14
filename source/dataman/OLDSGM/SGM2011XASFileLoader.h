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


#ifndef SGM2011XASFILELOADER_H
#define SGM2011XASFILELOADER_H

#include "dataman/AMAbstractFileLoader.h"
#include "util/AMBiHash.h"
#include "dataman/info/AMOldDetectorInfo.h"

class AMXASScan;

/// This class encapsulates loading from SGM DacqLibrary data files (XAS scans made with Acquaman from 2011 to ?) to an AMXASScan. It can only be used for loading files that we already have in the database which happen to store their data in this file format. This loader is far more restrictive than the 2004FileLoader as it needs to be able to convert all the PV name it encounters to column names. However, it can also load data with more than one spectral component.
class SGM2011XASFileLoader : public AMAbstractFileLoader
{
public:
 	virtual ~SGM2011XASFileLoader();
	SGM2011XASFileLoader(AMXASScan *scan = 0);

	QString formatTag() const { return "sgm2011XAS"; }

	/// load from SGM dacqLibrary file format (the setMetaData flag is not used)
	bool loadFromFile(const QString& filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks);

	/// save to SGM legacy file format (\todo UNIMPLEMENTED)
	bool saveToFile(const QString& filepath) {
		Q_UNUSED(filepath)
		return false;
	}

	/// translate a PV string from the file header to a meaningful name, if we recognize it.  Unrecognized PV strings are untouched and false is returned.
	static bool pv2columnName(QString& pv) {
		QString newName = columns2pvNames_.valueR(pv);
		if(newName.isEmpty())
			return false;
		pv = newName.section('_', 0, 0);//Want to remove anything after the underscore
		return true;
	}

protected:

	/// A forward- and reverse-mapping from meaningful data column names to process variable strings
	static AMBiHash<QString, QString> columns2pvNames_;

	/* Want/need to use this ... get some help
	/// A forward- and reverse-mapping from offset data (for the spectra.dat files) to their measurement infos
	static AMBiHash<QString, AMMeasurementInfo> offsets2MeasurementInfos_;
	*/
	/**/
	QStringList offsets2MeasurementInfos_;
	/**/

	/// A list (in order) of the column names that our users might be interested in (ie: those which we should expose the raw data for, by default).  These names should match the column names in columns2pvNames_.
	QStringList defaultUserVisibleColumns_;
};

#endif // SGM2011XASFILELOADER_H
