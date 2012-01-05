#include "AMEXAFSScanConfiguration.h"
#include "util/AMErrorMonitor.h"

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
	: AMXASScanConfiguration(original.parent(), false)
{
	if (setup){

		qDebug() << "Using AMEXAFSScanConfiguration copy constructor";
		setUserScanName(original.userScanName());
		regions_ = new AMEXAFSRegionsList(this);
		connect(regions_, SIGNAL(regionsChanged()), this, SLOT(onRegionsChanged()));
		connect(regions_, SIGNAL(regionsChanged()), this, SIGNAL(configurationChanged()));
	}
}

QString AMEXAFSScanConfiguration::dbReadRegions() const{

	QStringList rv;

	// Stores the following information from the region in the following order:
	/*
	  1) Type (either Energy space or kSpace)
	  2) Edge energy (in the units given by region units)
	  3) Region starting point in energy space.
	  4) Region delta between points.
	  5) Region ending point in energy space.
	  6) Whether the region uses elastic starting points.
	  7) Whether the region uses elastic ending points.
	  8) The time spent on each point in the region.
	  9) The units for the scanned element of the region.  This is the units of energy space even if the region is itself in kSpace.
	  10) The units of time spent on each point.
	  */
	for(int x = 0; x < regions_->count(); x++){

		rv << QString("exafsVersion1.0,%1,%2,%3,%4,%5,%6,%7,%8,%9,%10")
			  .arg(regionType(x) == AMEXAFSRegion::Energy ? "Energy" : "kSpace")
			  .arg(regionEdgeEnergy(x))
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
	bool addRegionSuccess;

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

			addRegionSuccess = addRegion(x, oneRegion.at(3).toDouble(), oneRegion.at(4).toDouble(), oneRegion.at(5).toDouble(), oneRegion.at(8).toDouble());
			addRegionSuccess &= setRegionType(x, oneRegion.at(1) == "Energy" ? AMEXAFSRegion::Energy : AMEXAFSRegion::kSpace);
			addRegionSuccess &= setRegionEdgeEnergy(x, oneRegion.at(2).toDouble());
			addRegionSuccess &= setRegionElasticStart(x, oneRegion.at(6).toInt() == 1 ? true : false);
			addRegionSuccess &= setRegionElasticEnd(x, oneRegion.at(7).toInt() == 1 ? true : false);
			addRegionSuccess &= setRegionUnits(x, oneRegion.at(9));
			addRegionSuccess &= setRegionTimeUnits(x, oneRegion.at(10));
		}

		if (!addRegionSuccess)
			AMErrorMon::report(AMErrorReport(this,
							AMErrorReport::Alert,
							0,
							QString("There was an error reloading the scan region #%1, when reloading this scan from the database. Your database might be corrupted. Please report this bug to the Acquaman developers.").arg(x)));
	}
}
