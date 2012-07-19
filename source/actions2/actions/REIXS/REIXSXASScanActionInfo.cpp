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


#include "REIXSXASScanActionInfo.h"

REIXSXASScanActionInfo::REIXSXASScanActionInfo(REIXSXASScanConfiguration *scanConfig, QObject *parent) : AMScanControllerActionInfo(scanConfig, parent)
{
	connect(scanConfig, SIGNAL(configurationChanged()), this, SLOT(onConfigurationChanged()));
}

REIXSXASScanActionInfo::REIXSXASScanActionInfo(const REIXSXASScanActionInfo &other) : AMScanControllerActionInfo(other)
{
	connect(XASConfig(), SIGNAL(configurationChanged()), this, SLOT(onConfigurationChanged()));
}

// This will never be called, because we will always have a valid REIXSXASScanConfiguration returned by dbReadScanConfig().
void REIXSXASScanActionInfo::dbLoadScanConfig(AMDbObject */*newObject*/) {
//	REIXSXASScanConfiguration* newScanConfig = qobject_cast<REIXSXASScanConfiguration*>(newObject);
//	if(!newScanConfig) {
//		delete newObject;
//		return;
//	}

//	delete scanConfig_;
//	scanConfig_ = newScanConfig;
//	connect(newScanConfig, SIGNAL(configurationChanged()), this, SLOT(onConfigurationChanged()));
}

AMDbObject * REIXSXASScanActionInfo::dbReadScanConfig()
{
	return scanConfig_;
}


void REIXSXASScanActionInfo::onConfigurationChanged()
{
	QString description = XASConfig()->description();
	setShortDescription(description);
	setLongDescription(description);
}
