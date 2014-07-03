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


#ifndef VESPERSSCANCONFIGURATIONVIEWHOLDER3_H
#define VESPERSSCANCONFIGURATIONVIEWHOLDER3_H

#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/VESPERS/VESPERSEXAFSScanConfigurationView.h"

/// This class is a simple extension to the normal configuration view holder specifically for XAS scans.
class VESPERSScanConfigurationViewHolder3 : public AMScanConfigurationViewHolder3
{
	Q_OBJECT

public:
	/// Constructor.  Requires the VESPERSEXAFSScanConfigurationView since it is specifically that scan that needs the ability to be put inside a loop action.
	VESPERSScanConfigurationViewHolder3(VESPERSEXAFSScanConfigurationView *view = 0, QWidget *parent = 0);
	/// Destructor.
	virtual ~VESPERSScanConfigurationViewHolder3();

protected:
	/// This function is used to create the action that will be added to the queue.  The difference between this method and the normal one is the creation of a loop and putting the scan inside.
	virtual AMAction3 *createAction();
};

#endif // VESPERSSCANCONFIGURATIONVIEWHOLDER3_H
