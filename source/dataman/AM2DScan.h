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


#ifndef AM2DSCAN_H
#define AM2DSCAN_H

#include "dataman/AMScan.h"

/// This is a practical sublcass of AMScan which provides the details to represent any two-dimensional scan.
class AM2DScan : public AMScan
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "shareTableWithClass=AMScan;description=2D Scan")
public:
	/// Create a new 2D scan.
 	virtual ~AM2DScan();
	Q_INVOKABLE explicit AM2DScan(QObject *parent = 0);
};

#endif // AM2DSCAN_H
