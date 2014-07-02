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


#ifndef VESPERSXASDATALOADER_H
#define VESPERSXASDATALOADER_H

#include "dataman/AMAbstractFileLoader.h"
#include "dataman/AMXASScan.h"

/*!
  This class implements loading of the raw XAS data files that are outputted from the Dacq library.
  The file type is the standard CLS output file.
  */
class VESPERSXASDataLoader : public AMAbstractFileLoader
{

public:
	/// Constructor.  Creates a file loader to load the scan from \param scan.
 	virtual ~VESPERSXASDataLoader();
	explicit VESPERSXASDataLoader(AMXASScan *scan);

	/// Format tag. a unique string identifying this format.
	virtual QString formatTag() const { return QString("vespersXAS"); }

	/// Loads data from \param filepath into the target scan.
	virtual bool loadFromFile(const QString &filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks);

	/// This is unimplemented because the Dacq library and corresponding scan controller handles saving the file.
	virtual bool saveToFile(const QString &filepath) { Q_UNUSED(filepath) return false; }
};

#endif // VESPERSXASDATALOADER_H
