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


#include "AMEXAFSScanConfiguration.h"
#include "util/AMErrorMonitor.h"
#include <QDebug>

 AMEXAFSScanConfiguration::~AMEXAFSScanConfiguration(){}
AMEXAFSScanConfiguration::AMEXAFSScanConfiguration(QObject *parent, bool setup)
	: AMXASScanConfiguration(parent, false)
{
	if (setup){

		userScanName_ = "$sample - $technique";
		regions_ = new AMEXAFSRegionsList(this);
		connect(regions_, SIGNAL(regionsChanged()), this, SLOT(onRegionsChanged()));
		connect(regions_, SIGNAL(regionsChanged()), this, SIGNAL(configurationChanged()));
	}
}

AMEXAFSScanConfiguration::AMEXAFSScanConfiguration(const AMEXAFSScanConfiguration &original, bool setup)
	: AMXASScanConfiguration(original, false)
{
	if (setup){
		setUserScanName(original.userScanName());
		regions_ = new AMEXAFSRegionsList(this);
		connect(regions_, SIGNAL(regionsChanged()), this, SLOT(onRegionsChanged()));
		connect(regions_, SIGNAL(regionsChanged()), this, SIGNAL(configurationChanged()));
	}
}

double AMEXAFSScanConfiguration::startValue() const
{
	if(regions_->count() > 0)
		return regionStartByType(0, AMEXAFSRegion::Energy);
	return -1;
}

double AMEXAFSScanConfiguration::endValue() const
{
	if(regions_->count() > 0)
		return regionEndByType(regions_->count()-1, AMEXAFSRegion::Energy);
	return -1;
}

bool AMEXAFSScanConfiguration::setStartValue(double startValue)
{
	if(regions_->count() > 0)
		return setRegionStartByType(0, startValue, AMEXAFSRegion::Energy);
	return false;
}

bool AMEXAFSScanConfiguration::setEndValue(double endValue)
{
	if(regions_->count() > 0)
		return setRegionEndByType(regions_->count()-1, endValue, AMEXAFSRegion::Energy);
	return false;
}

QString AMEXAFSScanConfiguration::dbReadRegions() const{

	QStringList rv;

	// Stores the following information from the region in the following order:
	/*
	 1) Type (either Energy space or kSpace)
	 2) Edge energy (in the units given by region units)
	 3) Whether the energy is relative or absolute (true = relative, false = absolute).
	 4) Region starting point in energy space.
	 5) Region delta between points.
	 6) Region ending point in energy space.
	 7) Whether the region uses elastic starting points.
	 8) Whether the region uses elastic ending points.
	 9) The time spent on each point in the region.
	 10) The units for the scanned element of the region.  This is the units of energy space even if the region is itself in kSpace.
	 11) The units of time spent on each point.
	  */
	for(int x = 0; x < regions_->count(); x++){

		rv << QString("exafsVersion1.0,%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11")
			  .arg(regionType(x) == AMEXAFSRegion::Energy ? "Energy" : "kSpace")
			  .arg(exafsRegions()->defaultEdgeEnergy())
			  .arg(exafsRegions()->defaultIsRelative() == true ? 1 : 0)
			  .arg(regionStartByType(x, AMEXAFSRegion::Energy))
			  .arg(regionDelta(x))
			  .arg(regionEndByType(x, AMEXAFSRegion::Energy))
			  .arg(regionElasticStart(x) == true ? 1 : 0)
			  .arg(regionElasticEnd(x) == true ? 1 : 0)
			  .arg(regionTime(x))
			  .arg(regionEnergyUnits(x))
			  .arg(regionTimeUnits(x));
	}
	if(rv.isEmpty())
		return QString("");
	return rv.join("\n");
}

void AMEXAFSScanConfiguration::dbLoadRegions(const QString &exafsRegionsString){
	if(exafsRegionsString.isEmpty())
		return;
	QStringList allRegions = exafsRegionsString.split("\n", QString::SkipEmptyParts);
	QStringList oneRegion;
	bool addRegionSuccess = false;

	for(int x = 0; x < allRegions.count(); x++){
		oneRegion = allRegions.at(x).split(",", QString::SkipEmptyParts);

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

		else if (oneRegion.at(0) == "exafsVersion1.0"){

			addRegionSuccess = addRegion(x, oneRegion.at(4).toDouble(), oneRegion.at(5).toDouble(), oneRegion.at(6).toDouble(), oneRegion.at(9).toDouble());
			addRegionSuccess &= setRegionType(x, oneRegion.at(1) == "Energy" ? AMEXAFSRegion::Energy : AMEXAFSRegion::kSpace);
			addRegionSuccess &= setRegionIsRelative(x, oneRegion.at(3).toInt() == 1 ? true : false);
			addRegionSuccess &= setRegionEdgeEnergy(x, oneRegion.at(2).toDouble());
			addRegionSuccess &= setRegionElasticStart(x, oneRegion.at(7).toInt() == 1 ? true : false);
			addRegionSuccess &= setRegionElasticEnd(x, oneRegion.at(8).toInt() == 1 ? true : false);
			addRegionSuccess &= setRegionUnits(x, oneRegion.at(10));
			addRegionSuccess &= setRegionTimeUnits(x, oneRegion.at(11));

			if (x == 0){

				exafsRegions()->setDefaultEdgeEnergy(regionEdgeEnergy(x));
				exafsRegions()->setDefaultIsRelative(regionIsRelative(x));
			}
		}

		if (!addRegionSuccess)
			AMErrorMon::report(AMErrorReport(this,
							AMErrorReport::Alert,
							0,
							QString("There was an error reloading the scan region #%1, when reloading this scan from the database. Your database might be corrupted. Please report this bug to the Acquaman developers.").arg(x)));
	}
}
