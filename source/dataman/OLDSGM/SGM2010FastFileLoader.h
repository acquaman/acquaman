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


#ifndef SGM2010FASTFILELOADER_H
#define SGM2010FASTFILELOADER_H

#include "dataman/AMAbstractFileLoader.h"
#include "util/AMBiHash.h"

class AMFastScan;

class SGM2010FastFileLoader : public AMAbstractFileLoader
{
public:
 	virtual ~SGM2010FastFileLoader();
    SGM2010FastFileLoader(AMFastScan *scan = 0);

    QString formatTag() const { return "sgm2010Fast"; }

    /// load from SGM _spectra.dat file format
    bool loadFromFile(const QString &filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks);

    /// save to SGM _spectra.dat file format
    bool saveToFile(const QString &filepath);

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

#endif // SGM2010FASTFILELOADER_H
