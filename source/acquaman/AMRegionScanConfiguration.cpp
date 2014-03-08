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

#include "AMRegionScanConfiguration.h"
#include "util/AMErrorMonitor.h"

AMRegionScanConfiguration::~AMRegionScanConfiguration(){}

AMRegionScanConfiguration::AMRegionScanConfiguration(QObject *parent, bool setup)
	: AMScanConfiguration(parent)
{
	if (setup){

		userScanName_ = "$sample - $technique";
		regions_ = new AMRegionsList(this);
		connect(regions_, SIGNAL(regionsChanged()), this, SLOT(onRegionsChanged()));
		connect(regions_, SIGNAL(regionsChanged()), this, SIGNAL(configurationChanged()));
	}
}

AMRegionScanConfiguration::AMRegionScanConfiguration(const AMRegionScanConfiguration &original, bool setup)
	: AMScanConfiguration()
{
	if (setup){
		setUserScanName(original.userScanName());
		regions_ = new AMRegionsList(this);
		connect(regions_, SIGNAL(regionsChanged()), this, SLOT(onRegionsChanged()));
		connect(regions_, SIGNAL(regionsChanged()), this, SIGNAL(configurationChanged()));
	}
}

const QMetaObject* AMRegionScanConfiguration::getMetaObject(){
	return metaObject();
}

double AMRegionScanConfiguration::startValue() const{
	if(regions_->count() > 0)
		return regionStart(0);
	return -1;
}

double AMRegionScanConfiguration::endValue() const{
	if(regions_->count() > 0)
		return regionEnd(regions_->count()-1);
	return -1;
}

bool AMRegionScanConfiguration::setStartValue(double startValue){
	if(regions_->count() > 0)
		return setRegionStart(0, startValue);
	return false;
}

bool AMRegionScanConfiguration::setEndValue(double endValue){
	if(regions_->count() > 0)
		return setRegionEnd(regions_->count()-1, endValue);
	return false;
}

void AMRegionScanConfiguration::onRegionsChanged(){
	setModified(true);
	emit regionsChanged();
}

QString AMRegionScanConfiguration::dbReadRegions() const{

	QStringList rv;

	// Stores the following information from the region in the following order:
	/*
	  1) Region starting point.
	  2) Region delta between points.
	  3) Region ending point.
	  4) Whether the region uses elastic starting points.
	  5) Whether the region uses elastic ending points.
	  6) The time spent on each point in the region.
	  7) The units for the scanned element of the region.
	  8) The units of time spent on each point.
	  */
	for(int x = 0; x < regions_->count(); x++){

		rv << QString("regionsVersion1.0,%1,%2,%3,%4,%5,%6,%7,%8")
			  .arg(regionStart(x))
			  .arg(regionDelta(x))
			  .arg(regionEnd(x))
			  .arg(regionElasticStart(x) == true ? 1 : 0)
			  .arg(regionElasticEnd(x) == true ? 1 : 0)
			  .arg(regionTime(x))
			  .arg(regionUnits(x))
			  .arg(regionTimeUnits(x));
	}
	if(rv.isEmpty())
		return QString("");
	return rv.join("\n");
}

void AMRegionScanConfiguration::dbLoadRegions(const QString &regionsString)
{
	// delete existing regions, if any.
	while(regions_->count())
		regions_->deleteRegion(regions_->count()-1);

	if(regionsString.isEmpty())
		return;
	QStringList allRegions = regionsString.split("\n", QString::SkipEmptyParts);
	QStringList oneRegion;
	bool addRegionSuccess = false;

	for(int x = 0; x < allRegions.count(); x++){
		oneRegion = allRegions.at(x).split(",", QString::KeepEmptyParts);

		// Legacy Acquaman XAS settings (version 1.0)
		if (oneRegion.count() == 3)
			addRegionSuccess = addRegion(x, oneRegion.at(0).toDouble(), oneRegion.at(1).toDouble(), oneRegion.at(2).toDouble());
		// Another legacy Acquaman XAS settings version 1.1
		else if (oneRegion.at(0) == "xasVersion1.1"){

			addRegionSuccess = addRegion(x, oneRegion.at(1).toDouble(), oneRegion.at(2).toDouble(), oneRegion.at(3).toDouble(), oneRegion.at(6).toDouble());
			addRegionSuccess &= setRegionElasticStart(x, oneRegion.at(4).toInt() == 1 ? true : false);
			addRegionSuccess &= setRegionElasticEnd(x, oneRegion.at(5).toInt() == 1 ? true : false);
		}
		// Beginning of a more succinct usage of AMRegions.  Version 1.0
		else if (oneRegion.at(0) == "regionsVersion1.0"){

			addRegionSuccess = addRegion(x, oneRegion.at(1).toDouble(), oneRegion.at(2).toDouble(), oneRegion.at(3).toDouble(), oneRegion.at(6).toDouble());
			addRegionSuccess &= setRegionElasticStart(x, oneRegion.at(4).toInt() == 1 ? true : false);
			addRegionSuccess &= setRegionElasticEnd(x, oneRegion.at(5).toInt() == 1 ? true : false);
			addRegionSuccess &= setRegionUnits(x, oneRegion.at(7));
			addRegionSuccess &= setRegionTimeUnits(x, oneRegion.at(8));
		}

		if (!addRegionSuccess)
			AMErrorMon::report(AMErrorReport(this,
							AMErrorReport::Alert,
							0,
							QString("There was an error reloading the scan region #%1, when reloading this scan from the database. Your database might be corrupted. Please report this bug to the Acquaman developers.").arg(x)));
	}
}
