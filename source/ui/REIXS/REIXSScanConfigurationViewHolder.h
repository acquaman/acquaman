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


#ifndef REIXSSCANCONFIGURATIONVIEWHOLDER_H
#define REIXSSCANCONFIGURATIONVIEWHOLDER_H

#include "ui/actions2/AMActionRunnerAddActionBar.h"

class AMScanConfigurationView;

/// This widget holds view widgets for configuring scans on REIXS, and implements AMActionRunnerAddActionBar::createAction() to create the REIXS-specific actions.
class REIXSScanConfigurationViewHolder : public AMActionRunnerAddActionBar
{
    Q_OBJECT
public:
	/// Constructor. We can handle any of the AMScanConfigurationViews used on REIXS.
	REIXSScanConfigurationViewHolder(AMScanConfigurationView* view, QWidget *parent = 0);

protected:
	/// Creates the appropriate action depending on the view type.
	virtual AMAction* createAction();

	AMScanConfigurationView* view_;
};

#endif // REIXSSCANCONFIGURATIONVIEWHOLDER_H
