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


#ifndef AMREGIONSCANCONFIGURATIONCONVERTER_H
#define AMREGIONSCANCONFIGURATIONCONVERTER_H

#include "acquaman/AMScanActionControllerScanConfigurationConverter.h"

class AMRegionScanConfiguration;

#define AMREGIONSCANCONFIGURATIONCONVERTER_INVALID_CONFIGURATION 688000
#define AMREGIONSCANCONFIGURATIONCONVERTER_INVALID_REGION 688001
#define AMREGIONSCANCONFIGURATIONCONVERTER_INVALID_SCAN_AXIS 688002
#define AMREGIONSCANCONFIGURATIONCONVERTER_COULD_NOT_ADD_DETECTOR 688003

/// This class extends the functionality of the AMScanActionControllerScanConfigurationConverter to turn any AMRegionScanConfiguration into a viable and runnable scan.
class AMRegionScanConfigurationConverter : public AMScanActionControllerScanConfigurationConverter
{
	Q_OBJECT

public:
	/// Constructor.  Requires a valid scan assembler and an AMRegionScanConfiguration to build from.
	AMRegionScanConfigurationConverter(AMScanActionControllerScanAssembler *scanAssembler = 0, AMRegionScanConfiguration *scanConfiguration = 0, QObject *parent = 0);
	/// Destructor.
	virtual ~AMRegionScanConfigurationConverter();

protected:
	/// Re-implemented to turn an AMRegionScanConfiguration into a runnable scan.
	virtual bool convertImplementation(AMScanActionControllerScanAssembler *scanAssembler, AMScanConfiguration *scanConfiguration);
};

#endif // AMXASSCANCONFIGURATIONCONVERTER_H
