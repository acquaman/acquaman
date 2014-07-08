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


#ifndef AMTIMEDSCANCONFIGURATIONCONVERTER_H
#define AMTIMEDSCANCONFIGURATIONCONVERTER_H

#include "acquaman/AMScanActionControllerScanConfigurationConverter.h"

class AMTimedRegionScanConfiguration;

#define AMTIMEDSCANCONFIGURATIONCONVERTER_INVALID_CONFIGURATION 683000
#define AMTIMEDSCANCONFIGURATIONCONVERTER_INVALID_SCAN_AXIS 683001
#define AMTIMEDSCANCONFIGURATIONCONVERTER_COULD_NOT_ADD_DETECTOR 683002

/// This class extends the functionality of the AMScanActionControllerScanConfigurationConverter to turn the scan configuration into a proper action tree.
class AMTimedScanConfigurationConverter : public AMScanActionControllerScanConfigurationConverter
{
	Q_OBJECT

public:
	/// Constructor.  Requires a valid scan assember and an AMTimedRegionScanConfiguration to build from.
	AMTimedScanConfigurationConverter(AMScanActionControllerScanAssembler *scanAssembler = 0, AMTimedRegionScanConfiguration *scanConfiguration = 0, QObject *parent = 0);
	/// Destructor.
	virtual ~AMTimedScanConfigurationConverter(){}

protected:
	/// Re-implemented to turn an AMRegionScanConfiguration into a runnable scan.
	virtual bool convertImplementation(AMScanActionControllerScanAssembler *scanAssembler, AMScanConfiguration *scanConfiguration);
};

#endif // AMTIMEDSCANCONFIGURATIONCONVERTER_H
