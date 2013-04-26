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


#ifndef VESPERS2012DFILELOADERPLUGIN_H
#define VESPERS2012DFILELOADERPLUGIN_H

#include <QObject>

#define VESPERS2012DFILELOADERPLUGIN_CANNOT_OPEN_FILE 627201
#define VESPERS2012DFILELOADERPLUGIN_CANNOT_OPEN_SPECTRA_FILE 627202

#include "dataman/AMFileLoaderInterface.h"

/*!
  This class implements loading of the raw 2D data files that are outputted from the Dacq library.
  The file type is the standard CLS output file.
  */
class VESPERS20122DFileLoaderPlugin : public AMFileLoaderInterface
{

public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept vespersXAS and vespers2011XAS and vespers2011EXAFS.
	virtual QStringList acceptedFileFormats() { return QStringList() << "vespers2012XRF1El" << "vespers2012XRF1ElXRD" << "vespers2012XRF4El" << "vespers2012XRF4ElXRD" << "vespers2012XRF1Eln4El" << "vespers2012XRF1Eln4ElXRD"; }
	/// Format tag. a unique string identifying this format.
	virtual bool accepts(AMScan *scan);

	/// Loads data from \param filepath into the target scan.
	virtual bool load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor);
};

class VESPERS20122DFileLoaderFactory : public QObject, AMFileLoaderFactory
{
	Q_OBJECT
	Q_INTERFACES(AMFileLoaderFactory)

public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept vespersXAS and vespers2011XAS
	virtual QStringList acceptedFileFormats() { return QStringList() << "vespers2012XRF1El" << "vespers2012XRF1ElXRD" << "vespers2012XRF4El" << "vespers2012XRF4ElXRD" << "vespers2012XRF1Eln4El" << "vespers2012XRF1Eln4ElXRD"; }
	/// This function allows you to do a more detailed check of an AMScan to see whether you can load data for it.
	virtual bool accepts(AMScan *scan);

	/// This should create an instance of an AMFileLoaderInterface object to do the actual loading.
	virtual AMFileLoaderInterface* createFileLoader() { return new VESPERS20122DFileLoaderPlugin(); }
};

#endif // VESPERS20122DFILELOADERPLUGIN_H
