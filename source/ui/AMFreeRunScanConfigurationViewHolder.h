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


#ifndef AMFREERUNSCANCONFIGURATIONVIEWHOLDER_H
#define AMFREERUNSCANCONFIGURATIONVIEWHOLDER_H

#include "ui/AMScanConfigurationViewHolder.h"

class AMFreeRunScanConfigurationViewHolder : public AMScanConfigurationViewHolder
{
	Q_OBJECT

public:
	/// Default constructor that simply passes on all this information but hides the bottom bar.
	AMFreeRunScanConfigurationViewHolder(AMWorkflowManagerView* workflow, AMScanConfigurationView* view = 0, QWidget *parent = 0);

public slots:
	/// Start the scan and add it to the workflow.  It is public to allow your free run viewing classes the ability to connect to the workflow without having to see it.
	void onFreeRunStartRequested();
};

#endif // AMFREERUNSCANCONFIGURATIONVIEWHOLDER_H
