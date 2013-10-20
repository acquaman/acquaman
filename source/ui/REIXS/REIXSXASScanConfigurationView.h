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


#ifndef REIXSXASSCANCONFIGURATIONVIEW_H
#define REIXSXASSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/REIXS/REIXSXASScanConfiguration.h"

class AMSampleSelector;
class AMTopFrame2;

namespace Ui {
class REIXSXASScanConfigurationView;
}

/// Editor widget for a REIXSXASScanConfiguration, used to configure XAS scans to run now or later.
class REIXSXASScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.  If \c config is 0, we create our own (and delete it, when destroyed).  If a valid \c config is provided, we will operate on that one, but not take ownership of it. It must remain valid as long as this widget exists.
	explicit REIXSXASScanConfigurationView(REIXSXASScanConfiguration* config = 0, QWidget *parent = 0);
	virtual ~REIXSXASScanConfigurationView();

	virtual const AMScanConfiguration* configuration() const { return config_; }

protected:
	REIXSXASScanConfiguration* config_;

protected slots:
	/// Determines whether to enable the polarization angle (only if the polarization is enabled, and set to Linear Inclined).
	void reviewPolarizationAngleBoxEnabled();
	/// Computes the expected acquisition time when the regions change.
	void onRegionsChanged();

private:
	Ui::REIXSXASScanConfigurationView *ui;
	AMSampleSelector* sampleSelector_;
	AMTopFrame2* topFrame_;
};

#endif // REIXSXASSCANCONFIGURATIONVIEW_H
