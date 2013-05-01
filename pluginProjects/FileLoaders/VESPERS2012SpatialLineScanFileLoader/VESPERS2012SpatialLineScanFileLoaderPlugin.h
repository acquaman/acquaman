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


#ifndef VESPERS201SPATIALLINESCANFILELOADERPLUGIN_H
#define VESPERS201SPATIALLINESCANFILELOADERPLUGIN_H

#include <QObject>

#define VESPERS201SPATIALLINESCANFILELOADERPLUGIN_CANNOT_OPEN_FILE 627101
#define VESPERS201SPATIALLINESCANFILELOADERPLUGIN_CANNOT_OPEN_SPECTRA_FILE 627102

#include "dataman/AMFileLoaderInterface.h"

/*!
  This class implements loading of the raw line scan data files that are outputted from the Dacq library.
  The file type is the standard CLS output file.
  */
class VESPERS2012SpatialLineScanFileLoaderPlugin : public AMFileLoaderInterface
{

public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept vespers2012LineScanXRF1El, vespers2012LineScanXRF1ElXRD, vespers2012LineScanXRF4El, and vespers2012LineScanXRF4ElXRD.
	virtual QStringList acceptedFileFormats() { return QStringList() << "vespers2012LineScanXRF1El" << "vespers2012LineScanXRF1ElXRD" << "vespers2012LineScanXRF4El" << "vespers2012LineScanXRF4ElXRD" << "vespers2012LineScan1Eln4El" << "vespers2012LineScan1Eln4ElXRD" << "vespers2012Energy"; }
	/// Format tag. a unique string identifying this format.
	virtual bool accepts(AMScan *scan);

	/// Loads data from \param filepath into the target scan.
	virtual bool load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor);
};

class VESPERS2012SpatialLineScanFileLoaderFactory : public QObject, AMFileLoaderFactory
{
	Q_OBJECT
	Q_INTERFACES(AMFileLoaderFactory)

public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept vespersXAS and vespers2011XAS
	virtual QStringList acceptedFileFormats() { return QStringList() << "vespers2012LineScanXRF1El" << "vespers2012LineScanXRF1ElXRD" << "vespers2012LineScanXRF4El" << "vespers2012LineScanXRF4ElXRD" << "vespers2012LineScan1Eln4El" << "vespers2012LineScan1Eln4ElXRD" << "vespers2012Energy"; }
	/// This function allows you to do a more detailed check of an AMScan to see whether you can load data for it.
	virtual bool accepts(AMScan *scan);

	/// This should create an instance of an AMFileLoaderInterface object to do the actual loading.
	virtual AMFileLoaderInterface* createFileLoader() { return new VESPERS2012SpatialLineScanFileLoaderPlugin(); }
};

#endif // VESPERS2012SPATIALLINESCANFILELOADERPLUGIN_H
