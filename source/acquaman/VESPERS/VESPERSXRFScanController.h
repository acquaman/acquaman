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


#ifndef VESPERSXRFSCANCONTROLLER_H
#define VESPERSXRFSCANCONTROLLER_H

#include "acquaman/VESPERS/VESPERSXRFScanConfiguration.h"
#include "acquaman/AMScanController.h"
#include "dataman/VESPERS/AMXRFScan.h"

class VESPERSXRFScanController : public AMScanController
{
	Q_OBJECT

public:
	/// Default constructor.
	VESPERSXRFScanController(VESPERSXRFScanConfiguration *scanConfig, QObject *parent = 0);
	/// Destructor.
	virtual ~VESPERSXRFScanController();

public slots:


protected slots:
	/// Helper slot to finish up a scan.
	void onDetectorAcquisitionFinished();
	/// Helper slot that emits the current progress of the scan.
	void onProgressUpdate();
	/// Checks to see if the scan has finished.  If it is, then the finish scan sequence is started.
	void onStatusChanged();

protected:
	/// Initializes the scan
	virtual bool initializeImplementation() { setInitialized(); return true; }
	/// Starts current scan.
	virtual bool startImplementation();
	/// Cancels current scan.  Treated as finishing early.
	virtual void cancelImplementation() { setCancelled(); }
	/// Cancels current scan. Treated as finishing early.
	virtual void stopImplementation(const QString &command) { Q_UNUSED(command); setFinished(); }
	/// Saves the data after a scan is stopped.
	void saveData();
};

#endif // VESPERSXRFSCANCONTROLLER_H
