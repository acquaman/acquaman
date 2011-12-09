#include "AMRegionScanConfiguration.h"
#include "util/AMErrorMonitor.h"

AMRegionScanConfiguration::AMRegionScanConfiguration(QObject *parent, bool setup)
	: AMScanConfiguration(parent)
{
	userScanName_ = "$sample - $technique";

	if (setup){

		regions_ = new AMRegionsList(this);
		connect(regions_, SIGNAL(regionsChanged()), this, SLOT(onRegionsChanged()));
		connect(regions_, SIGNAL(regionsChanged()), this, SIGNAL(configurationChanged()));
	}
}

AMRegionScanConfiguration::AMRegionScanConfiguration(const AMScanConfiguration &original, bool setup)
	: AMScanConfiguration(original.parent())
{
	qDebug() << "Using AMScanConfiguration copy constructor";

	if (setup){

		regions_ = new AMRegionsList(this);
		connect(regions_, SIGNAL(regionsChanged()), this, SLOT(onRegionsChanged()));
		connect(regions_, SIGNAL(regionsChanged()), this, SIGNAL(configurationChanged()));
	}

	setUserScanName(original.userScanName());
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

bool AMRegionScanConfiguration::setStartEnergy(double startValue){
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

	for(int x = 0; x < regions_->count(); x++)
		rv << QString("regionsVersion1.0,%1,%2,%3,%4,%5,%6").arg(regionStart(x)).arg(regionDelta(x)).arg(regionEnd(x)).arg(regionElasticStart(x) == true ? 1 : 0).arg(regionElasticEnd(x) == true ? 1 : 0).arg(regionTime(x));
	if(rv.isEmpty())
		return QString("");
	return rv.join("\n");
}

void AMRegionScanConfiguration::dbLoadRegions(const QString &XASRegionsString){
	if(XASRegionsString.isEmpty())
		return;
	QStringList allRegions = XASRegionsString.split("\n", QString::SkipEmptyParts);
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
		// Beginning of a more succinct usage of AMRegions.  Version 1.0	For now this the same as xasVersions1.1.
		else if (oneRegion.at(0) == "regionsVersion1.0"){

			addRegionSuccess = addRegion(x, oneRegion.at(1).toDouble(), oneRegion.at(2).toDouble(), oneRegion.at(3).toDouble(), oneRegion.at(6).toDouble());
			addRegionSuccess &= setRegionElasticStart(x, oneRegion.at(4).toInt() == 1 ? true : false);
			addRegionSuccess &= setRegionElasticEnd(x, oneRegion.at(5).toInt() == 1 ? true : false);
		}

		if (!addRegionSuccess)
			AMErrorMon::report(AMErrorReport(this,
							AMErrorReport::Alert,
							0,
							QString("There was an error reloading the scan region #%1, when reloading this scan from the database. Your database might be corrupted. Please report this bug to the Acquaman developers.").arg(x)));
	}
}
