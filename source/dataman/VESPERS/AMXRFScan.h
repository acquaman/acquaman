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


#ifndef AMXRFSCAN_H
#define AMXRFSCAN_H

#include "dataman/AMScan.h"

/*!
	 This is a practical subclass of AMScan which provides the details to represent an fluorescence scan.
It creates a zero dimensional scan axis because there aren't any independent variables that are varied in the scan
space.  All parameters are set and then data collects while you wait for it to be done.  This is why the contructor
is left empty.
*/

class AMXRFScan : public AMScan
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "shareTableWithClass=AMScan;description=XRay Fluorescence Scan")

public:
	/// Constructor.  Creates a new XRF scan.
 	virtual ~AMXRFScan();
	Q_INVOKABLE explicit AMXRFScan(QObject *parent = 0);

};

#endif // AMXRFSCAN_H
