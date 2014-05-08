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
	isTransScan_ = true;
	useRef_ = true;


	edge_ = "";
	energy_ = 0.0;
	useFixedTime_ = false;
	numberOfScans_ = 1;
	totalTime_ = 0;
	timeOffset_ = 0.4;


	AMScanAxisRegion *region = new AMScanAxisEXAFSRegion;
	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);
	computeTotalTime();
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
	useRef_ = original.useRef();
	timeOffset_ = original.timeOffset();
	totalTime_ = original.totalTime();



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
    foreach (AMScanAxisRegion *region, scanAxisAt(0)->regions().toList()){

	    AMScanAxisEXAFSRegion *exafsRegion = qobject_cast<AMScanAxisEXAFSRegion *>(region);
	    int numberOfPoints = int((double(exafsRegion->regionEnd()) - double(exafsRegion->regionStart()))/double(exafsRegion->regionStep()) + 1);
	    if (exafsRegion->inKSpace() && exafsRegion->maximumTime().isValid()){

		    QVector<double> regionTimes = QVector<double>(numberOfPoints);
		    AMVariableIntegrationTime calculator(exafsRegion->equation(), exafsRegion->regionTime(), exafsRegion->maximumTime(), exafsRegion->regionStart(), exafsRegion->regionStep(), exafsRegion->regionEnd(), exafsRegion->a2());
		    calculator.variableTime(regionTimes.data());

		    for (int i = 0; i < numberOfPoints; i++)
			    time += regionTimes.at(i);
	    }

	    else
		    time += double(exafsRegion->regionTime())*numberOfPoints;
    }

    totalTime_ = time + 27; // There is a 27 second miscellaneous startup delay.
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
