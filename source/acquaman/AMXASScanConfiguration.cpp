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


#include "AMXASScanConfiguration.h"

// Constructor simply passes arguments up to AMScanConfiguration constructor.
 AMXASScanConfiguration::~AMXASScanConfiguration(){}
AMXASScanConfiguration::AMXASScanConfiguration(QObject *parent, bool setup)
	: AMRegionScanConfiguration(parent, false)
{
	if (setup){

		//userScanName_ = "$sample - $technique";
		regions_ = new AMXASRegionsList(this);
		connect(regions_, SIGNAL(regionsChanged()), this, SLOT(onRegionsChanged()));
		connect(regions_, SIGNAL(regionsChanged()), this, SIGNAL(configurationChanged()));
	}
}

AMXASScanConfiguration::AMXASScanConfiguration(const AMXASScanConfiguration &original, bool setup)
	: AMRegionScanConfiguration(original, false)
{
	if (setup){
		setUserScanName(original.userScanName());
		regions_ = new AMXASRegionsList(this);
		connect(regions_, SIGNAL(regionsChanged()), this, SLOT(onRegionsChanged()));
		connect(regions_, SIGNAL(regionsChanged()), this, SIGNAL(configurationChanged()));
	}
}
