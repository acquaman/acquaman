/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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
#include "util/AMErrorMonitor.h"


/// Constructor simply passes arguments up to AMScanConfiguration constructor.
AMXASScanConfiguration::AMXASScanConfiguration(QObject *parent) : AMScanConfiguration(parent)
{
	regions_ = new AMXASRegionsList(this);
	connect(regions_, SIGNAL(regionsChanged()), this, SLOT(onRegionsChanged()));
	connect(regions_, SIGNAL(regionsChanged()), this, SIGNAL(configurationChanged()));
}

double AMXASScanConfiguration::startEnergy() const{
	if(regions_->count() > 0)
		return regionStart(0);
	return -1;
}

double AMXASScanConfiguration::endEnergy() const{
	if(regions_->count() > 0)
		return regionEnd(regions_->count()-1);
	return -1;
}

bool AMXASScanConfiguration::setStartEnergy(double startEnergy){
	if(regions_->count() > 0)
		return setRegionStart(0, startEnergy);
	return false;
}

bool AMXASScanConfiguration::setEndEnergy(double endEnergy){
	if(regions_->count() > 0)
		return setRegionEnd(regions_->count()-1, endEnergy);
	return false;
}

void AMXASScanConfiguration::onRegionsChanged(){
	setModified(true);
	emit regionsChanged();
}

QString AMXASScanConfiguration::dbReadXASRegions() const{
	QStringList rv;

	for(int x = 0; x < regions_->count(); x++)
		rv << QString("%1,%2,%3").arg(regionStart(x)).arg(regionDelta(x)).arg(regionEnd(x));
	if(rv.isEmpty())
		return QString("");
	return rv.join("\n");
}

void AMXASScanConfiguration::dbLoadXASRegions(const QString &XASRegionsString){
	if(XASRegionsString.isEmpty())
		return;
	QStringList allRegions = XASRegionsString.split("\n", QString::SkipEmptyParts);
	QStringList oneRegion;

	for(int x = 0; x < allRegions.count(); x++){
		oneRegion = allRegions.at(x).split(",", QString::SkipEmptyParts);
		if(!addRegion(x, oneRegion.at(0).toDouble(), oneRegion.at(1).toDouble(), oneRegion.at(2).toDouble()))
			AMErrorMon::report(AMErrorReport(this,
							AMErrorReport::Alert,
							0,
							QString("There was an error reloading the scan region #%1, when reloading this scan from the database. Your database might be corrupted. Please report this bug to the Acquaman developers.").arg(x)));
	}
}
