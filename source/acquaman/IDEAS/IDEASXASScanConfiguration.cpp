#include "IDEASXASScanConfiguration.h"

#include "beamline/AMBeamline.h"
#include "acquaman/IDEAS/IDEASXASScanActionController.h"
#include "ui/IDEAS/IDEASXASScanConfigurationView.h"
#include "dataman/AMScanAxisEXAFSRegion.h"

IDEASXASScanConfiguration::IDEASXASScanConfiguration(QObject *parent) :
	AMStepScanConfiguration(parent)
{
	setName("Unnamed Scan");
	setUserScanName("Unnamed Scan");
//	I0Channel_ = "I_0";
//	ItChannel_ = "I_sample";
//	IrChannel_ = "I_ref";
	isXRFScan_ = true;
	isTransScan_ = false;
	useRef_ = false;


	edge_ = "";
	energy_ = 7112;
	useFixedTime_ = false;
	numberOfScans_ = 1;


	AMScanAxisRegion *region = new AMScanAxisEXAFSRegion(8800.0, 5.0, 8970.0, 1.0);
	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	region = new AMScanAxisEXAFSRegion(8970, 0.5, 9040, 1);
	axis->appendRegion(region);
	region = new AMScanAxisEXAFSRegion(3.24, 0.01, 10, 1, true, 9000, 10);
	axis->appendRegion(region);
	appendScanAxis(axis);
}

IDEASXASScanConfiguration::IDEASXASScanConfiguration(const IDEASXASScanConfiguration &original) :
	AMStepScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.userScanName());
//	I0Channel_ = original.I0Channel();
//	ItChannel_ = original.ItChannel();
//	IrChannel_ = original.IrChannel();
	isXRFScan_ = original.isXRFScan();
	isTransScan_ = original.isTransScan();

	edge_ = original.edge();
	energy_ = original.energy();
	useFixedTime_ = original.useFixedTime();
	numberOfScans_ = original.numberOfScans();

	computeTotalTime();


}

IDEASXASScanConfiguration::~IDEASXASScanConfiguration()
{

}

AMScanConfiguration* IDEASXASScanConfiguration::createCopy() const{
	return new IDEASXASScanConfiguration(*this);
}

AMScanController* IDEASXASScanConfiguration::createController()
{
	IDEASXASScanActionController *controller = new IDEASXASScanActionController(this);
	controller->buildScanController();

	return controller;
}

AMScanConfigurationView* IDEASXASScanConfiguration::createView(){
	return new IDEASXASScanConfigurationView(this);
}

QString IDEASXASScanConfiguration::detailedDescription() const{
	return QString("XAS Scan from %1 to %2").arg(double(scanAxisAt(0)->regionAt(0)->regionStart())).arg(double(scanAxisAt(0)->regionAt(scanAxisAt(0)->regionCount()-1)->regionEnd()));
}

QString IDEASXASScanConfiguration::headerText() const
{
	QString header("Configuration of the Scan\n\n");

	header.append("Scanned Edge:\t" + edge() + "\n");

	return header;
}

void IDEASXASScanConfiguration::computeTotalTimeImplementation()
{
	double time = 0;

	// Some region stuff.

	totalTime_ = time + 12; // There is a 9 second miscellaneous startup delay.
	setExpectedDuration(totalTime_);
	emit totalTimeChanged(totalTime_);
}

void IDEASXASScanConfiguration::setEdge(QString edgeName)
{
	if (edge_ != edgeName){

		edge_ = edgeName;
		emit edgeChanged(edgeName);
		setModified(true);
	}
}

void IDEASXASScanConfiguration::setEnergy(double edgeEnergy)
{
	if (energy_ != edgeEnergy){

//		exafsRegions()->setDefaultEdgeEnergy(edgeEnergy);
		foreach (AMScanAxisRegion *region, scanAxisAt(0)->regions().toList())
			((AMScanAxisEXAFSRegion *)region)->setEdgeEnergy(edgeEnergy);

		energy_ = edgeEnergy;
		emit energyChanged(energy_);
		setModified(true);
	}
}

void IDEASXASScanConfiguration::setUseFixedTime(bool fixed)
{
	if (useFixedTime_ != fixed){

		useFixedTime_ = fixed;
		emit useFixedTimeChanged(useFixedTime_);
		computeTotalTime();
		setModified(true);
	}
}

void IDEASXASScanConfiguration::setNumberOfScans(int num)
{
	if (numberOfScans_ != num){

		numberOfScans_ = num;
		emit numberOfScansChanged(numberOfScans_);
		setModified(true);
	}
}
