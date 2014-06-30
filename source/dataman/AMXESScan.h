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


#ifndef AMXESSCAN_H
#define AMXESSCAN_H

#include "dataman/AMScan.h"

/// This is a practical subclass of AMScan which provides the details to represent an emission scan (ex: one 2D spectrometer detector, acquired over time, but not scanned in incident energy
class AMXESScan : public AMScan {
	Q_OBJECT
	Q_CLASSINFO("AMDbObject_Attributes", "shareTableWithClass=AMScan;description=XRay Emission Scan")

public:
 	virtual ~AMXESScan();
	Q_INVOKABLE explicit AMXESScan(QObject *parent = 0);

};

#endif // AMXESSCAN_H
