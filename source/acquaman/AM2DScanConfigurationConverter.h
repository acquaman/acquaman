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


#ifndef AM2DSCANCONFIGURATIONCONVERTER_H
#define AM2DSCANCONFIGURATIONCONVERTER_H

#include "acquaman/AMScanActionControllerScanConfigurationConverter.h"

class AM2DScanConfiguration;

#define AM2DSCANCONFIGURATIONCONVERTER_INVALID_CONFIGURATION 689000
#define AM2DSCANCONFIGURATIONCONVERTER_INVALID_XAXIS 689001
#define AM2DSCANCONFIGURATIONCONVERTER_INVALID_YAXIS 689002
#define AM2DSCANCONFIGURATIONCONVERTER_COULD_NOT_ADD_X_AXIS 689003
#define AM2DSCANCONFIGURATIONCONVERTER_COULD_NOT_ADD_Y_AXIS 689004
#define AM2DSCANCONFIGURATIONCONVERTER_COULD_NOT_ADD_DETECTOR 689005

/// This class extends the functionality of the AMScanActionControllerScanConfigurationConverter to build the appropriate axes for a 2D scan.
class AM2DScanConfigurationConverter : public AMScanActionControllerScanConfigurationConverter
{
	Q_OBJECT

public:
	/// Constructor.  Requires a valid scan assembler and an AM2DScanConfiguration.
	AM2DScanConfigurationConverter(AMScanActionControllerScanAssembler *scanAssembler = 0, AM2DScanConfiguration *scanConfiguration = 0, QObject *parent = 0);
	/// Destructor.
	virtual ~AM2DScanConfigurationConverter(){}

protected:
	/// Re-implemented to turn an AM2DScanConfiguration into a runnable scan.
	virtual bool convertImplementation(AMScanActionControllerScanAssembler *scanAssembler, AMScanConfiguration *scanConfiguration);
};

#endif // AM2DSCANCONFIGURATIONCONVERTER_H
