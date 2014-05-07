/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef VESPERSXRFDATALOADER_H
#define VESPERSXRFDATALOADER_H

#include "dataman/AMAbstractFileLoader.h"
#include "dataman/VESPERS/AMXRFScan.h"

/*! This class implements loading and saving of XRF data in a custom way.  This is because this data is typically not saved currently.
	It consists of columns with the raw data from each element of the detector.
*/
class VESPERSXRFDataLoader : public AMAbstractFileLoader
{
public:
	/// Constructs a file loader to data into \c scan.
	VESPERSXRFDataLoader(AMXRFScan *scan);
	/// Destructor.
	virtual ~VESPERSXRFDataLoader();

	/// Format tag: a unique string identifying this format.
	virtual QString formatTag() const { return QString("vespersXRF"); }

	/// Loads data from \c filepath into the target scan.
	virtual bool loadFromFile(const QString& filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks);

	/// Save the target scan to the file at \c filepath.
	virtual bool saveToFile(const QString& filepath);
};

#endif // VESPERSXRFDATALOADER_H
