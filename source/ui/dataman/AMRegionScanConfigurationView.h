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


#ifndef AMREGIONSCANCONFIGURATIONVIEW_H
#define AMREGIONSCANCONFIGURATIONVIEW_H

#include "acquaman/AMRegionScanConfiguration.h"
#include "ui/acquaman/AMScanConfigurationView.h"

class AMRegionsView;
class AMRegionsLineView;

/// Provides a simple view of an AMRegionScanConfiguration with a table for editing the regions. Can be used within a more detailed scan configuration view, or on its own.
/*! Extend by incorporating inside a more detailed AMScanConfigurationView, or by accessing its layout() and adding widgets within.*/
class AMRegionScanConfigurationView : public AMScanConfigurationView
{
    Q_OBJECT
public:
	/// Constructor. A valid \c configuration must be provided.
	explicit AMRegionScanConfigurationView(AMRegionScanConfiguration* configuration, QWidget *parent = 0);
	virtual ~AMRegionScanConfigurationView();

	/// Returns a pointer to the configuration being edited.
	virtual const AMScanConfiguration* configuration() const { return configuration_; }

signals:

public slots:

protected:
	AMRegionScanConfiguration* configuration_;
	AMRegionsView *regionsView_;
	AMRegionsLineView *regionsLineView_;
};

#endif // AMREGIONSCANCONFIGURATIONVIEW_H
