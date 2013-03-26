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


#ifndef VESPERS2011XRFFILELOADERPLUGIN_H
#define VESPERS2011XRFFILELOADERPLUGIN_H

#include <QObject>

#define VESPERS2011XRFFILELOADERPLUGIN_CANNOT_OPEN_FILE 627301
#define VESPERS2011XRFFILELOADERPLUGIN_UNRECOGNIZED_FILE_TYPE 627302

#include "dataman/AMFileLoaderInterface.h"

/*! This class implements loading and saving of XRF data in a custom way.  This is because this data is typically not saved currently.
	It consists of columns with the raw data from each element of the detector.
*/
class VESPERS2011XRFFileLoaderPlugin : public AMFileLoaderInterface
{

public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept vespersXRF and vespers2011XRF
	virtual QStringList acceptedFileFormats() { return QStringList() << "vespersXRF" << "vespers2011XRF"; }
	/// Format tag. a unique string identifying this format.
	virtual bool accepts(AMScan *scan);

	/// Loads data from \param filepath into the target scan.
	virtual bool load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor);
};

class VESPERS2011XRFFileLoaderFactory : public QObject, public AMFileLoaderFactory
{
	Q_OBJECT
	Q_INTERFACES(AMFileLoaderFactory)

public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept vespersXRF and vespers2011XRF
	virtual QStringList acceptedFileFormats() { return QStringList() << "vespersXRF" << "vespers2011XRF"; }
	/// This function allows you to do a more detailed check of an AMScan to see whether you can load data for it.
	virtual bool accepts(AMScan *scan);

	/// This should create an instance of an AMFileLoaderInterface object to do the actual loading.
	virtual AMFileLoaderInterface* createFileLoader() { return new VESPERS2011XRFFileLoaderPlugin(); }
};

#endif // VESPERS2011XRFFILELOADERPLUGIN_H
