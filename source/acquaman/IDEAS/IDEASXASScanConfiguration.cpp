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
	I0Channel_ = "I_0";
	ItChannel_ = "I_sample";
	IrChannel_ = "I_ref";
	isXRFScan_ = true;
	isTransScan_ = true;

	AMScanAxisRegion *region = new AMScanAxisEXAFSRegion(6900, 0.5, 7150.0, 1.0, false, 7112.0);
	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);
}

IDEASXASScanConfiguration::IDEASXASScanConfiguration(const IDEASXASScanConfiguration &original) :
	AMStepScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.userScanName());
	I0Channel_ = original.I0Channel();
	ItChannel_ = original.ItChannel();
	IrChannel_ = original.IrChannel();
	isXRFScan_ = original.isXRFScan();
	isTransScan_ = original.isTransScan();


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
