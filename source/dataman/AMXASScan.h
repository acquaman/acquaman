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


#ifndef AMXASSCAN_H
#define AMXASSCAN_H


#include "dataman/AMScan.h"


/// This is a practical subclass of AMScan which provides the details to represent an absorption scan (ex: eV setpoints and readings from 0-dimensional and 1-dimensional detectors such as the sample current (TEY), fluorescence yield (TFY), and energy-resolved fluorescence (SDD).
/*! 	\todo detailed doc.
  */
class AMXASScan : public AMScan {
	Q_OBJECT
	Q_CLASSINFO("AMDbObject_Attributes", "shareTableWithClass=AMScan;description=XRay Absorption Scan")

public:
	/// create a new XAS scan
 	virtual ~AMXASScan();
	Q_INVOKABLE explicit AMXASScan(QObject *parent = 0);

protected:

	QString legacyGrating_;
	QString legacyIntegrationTime_;
};

#endif // AMXASSCAN_H
