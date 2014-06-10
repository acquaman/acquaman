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


#ifndef VESPERS2011XASFILELOADERPLUGIN_H
#define VESPERS2011XASFILELOADERPLUGIN_H

#include <QObject>

#define VESPERS2011XASFILELOADERPLUGIN_CANNOT_OPEN_FILE 627401
#define VESPERS2011XASFILELOADERPLUGIN_CANNOT_OPEN_SPECTRA_FILE 627402

#include "dataman/AMFileLoaderInterface.h"

/*!
  This class implements loading of the raw XAS data files that are outputted from the Dacq library.
  The file type is the standard CLS output file.
  */
class VESPERS2011XASFileLoaderPlugin : public AMFileLoaderInterface
{

public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept vespersXAS and vespers2011XAS and vespers2011EXAFS.
	virtual QStringList acceptedFileFormats() { return QStringList() << "vespersXAS" << "vespers2011XAS" << "vespers2011EXAFS"; }
	/// Format tag. a unique string identifying this format.
	virtual bool accepts(AMScan *scan);

	/// Loads data from \param filepath into the target scan.
	virtual bool load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor);
};

class VESPERS2011XASFileLoaderFactory : public QObject, AMFileLoaderFactory
{
	Q_OBJECT
	Q_INTERFACES(AMFileLoaderFactory)

public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept vespersXAS and vespers2011XAS
	virtual QStringList acceptedFileFormats() { return QStringList() << "vespersXAS" << "vespers2011XAS" << "vespers2011EXAFS"; }
	/// This function allows you to do a more detailed check of an AMScan to see whether you can load data for it.
	virtual bool accepts(AMScan *scan);

	/// This should create an instance of an AMFileLoaderInterface object to do the actual loading.
	virtual AMFileLoaderInterface* createFileLoader() { return new VESPERS2011XASFileLoaderPlugin(); }
};

#endif // VESPERS2011XASFILELOADERPLUGIN_H
