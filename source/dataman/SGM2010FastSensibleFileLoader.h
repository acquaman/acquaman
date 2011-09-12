/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef SGM2010FASTSENSIBLEFILELOADER_H
#define SGM2010FASTSENSIBLEFILELOADER_H

#include "dataman/AMAbstractFileLoader.h"
#include "util/AMBiHash.h"

class AMFastScan;

class SGM2010FastSensibleFileLoader : public AMAbstractFileLoader
{
public:
    SGM2010FastSensibleFileLoader(AMFastScan *scan = 0);

    QString formatTag() const { return "sgm2010FastSensible"; }

    /// load from SGM sensible fast scan file format
    bool loadFromFile(const QString &filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks);

    /// save to SGM sensible fast scan file format
    bool saveToFile(const QString &filepath);


protected:
    /// A list (in order) of the column names that our users might be interested in (ie: those which we should expose the raw data for, by default).  These names should match the column names in columns2pvNames_.
    QStringList defaultUserVisibleColumns_;
};

#endif // SGM2010FASTSENSIBLEFILELOADER_H
