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


#include "VESPERSTimeScanConfiguration.h"

#include "acquaman/VESPERS/VESPERSTimeScanActionController.h"
#include "ui/VESPERS/VESPERSTimeScanConfigurationView.h"

VESPERSTimeScanConfiguration::VESPERSTimeScanConfiguration(QObject *parent)
	: AMTimedRegionScanConfiguration(parent)
{
	setTime(1);
	setTotalTime(10);
}

VESPERSTimeScanConfiguration::VESPERSTimeScanConfiguration(const VESPERSTimeScanConfiguration &original)
	: AMTimedRegionScanConfiguration(original)
{

}

AMScanConfiguration *VESPERSTimeScanConfiguration::createCopy() const
{
	return new VESPERSTimeScanConfiguration(*this);
}

AMScanController *VESPERSTimeScanConfiguration::createController()
{
	AMScanActionController *controller = new VESPERSTimeScanActionController(this);
	controller->buildScanController();

	return controller;
}

AMScanConfigurationView *VESPERSTimeScanConfiguration::createView()
{
	return new VESPERSTimeScanConfigurationView(this);
}
