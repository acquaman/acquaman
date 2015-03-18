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


#ifndef SXRMBXRFSCANCONFIGURATION_H
#define SXRMBXRFSCANCONFIGURATION_H

#include "acquaman/AMScanConfiguration.h"

class SXRMBXRFScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=SXRMB XRF Scan Configuration")

public:
	/// Default constructor.
	Q_INVOKABLE explicit SXRMBXRFScanConfiguration(QObject *parent = 0);
	/// Convenience constructor.
	SXRMBXRFScanConfiguration(const SXRMBXRFScanConfiguration &other);
	/// Destructor.
	virtual ~SXRMBXRFScanConfiguration();


	/// Returns a new instance of the scan configuration.
	virtual AMScanConfiguration *createCopy() const;

	/// Returns a new instance of the scan controller.
	virtual AMScanController *createController();

	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details.
	virtual QString description() const {
		return QString("XRF Free Run Scan");
	}

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;

public slots:

protected:
};

#endif // SXRMBXRFSCANCONFIGURATION_H
