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


#ifndef AMXESSCAN_H
#define AMXESSCAN_H

#include "dataman/AMScan.h"
#include "acquaman/AMScanConfiguration.h"	/// \todo Move this to dataman; make sure it's just the information to do the scan, not the controls.


/// This is a practical subclass of AMScan which provides the details to represent an emission scan (ex: one 2D spectrometer detector, acquired over time, but not scanned in incident energy
/*! 	\todo detailed doc.
  */
class AMXESScan : public AMScan {
	Q_OBJECT
	Q_CLASSINFO("AMDbObject_Attributes", "shareTableWithClass=AMScan;description=XRay Emission Scan")

public:
	/// create a new XAS scan with the following named \c detectors. Each "detector" is a source of a datapoint, that will be stored/logged, available as a column of raw data, and accessible through channel(s).
	Q_INVOKABLE explicit AMXESScan(QObject *parent = 0);

	/// Re-implemented from AMScan. Currently only the 'reixsXESRaw' format is supported.
	virtual bool loadDataImplementation();


signals:


public slots:

protected slots:

protected:

	/// \todo Move this to AMScan eventually?
	AMScanConfiguration* configuration_;

	// friend class REIXSXESRawFileLoader;

};

#endif // AMXESSCAN_H
