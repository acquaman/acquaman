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


#ifndef VESPERSXRFSCANCONFIGURATIONVIEW_H
#define VESPERSXRFSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERSXRFScanConfiguration.h"

#include <QDoubleSpinBox>
#include <QLabel>

class VESPERSXRFScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT
public:
	/// Default constructor.
	VESPERSXRFScanConfigurationView(VESPERSXRFScanConfiguration *scanConfig, QWidget *parent = 0);
	/// Destructor.
	virtual ~VESPERSXRFScanConfigurationView();

	/// Returns a pointer to the current configuration.
	const AMScanConfiguration *configuration() const { return configuration_; }

protected slots:

protected:

	/// The current configuration.
	VESPERSXRFScanConfiguration *configuration_;
};

#endif // VESPERSXRFSCANCONFIGURATIONVIEW_H
