#include "VESPERSScanConfigurationDbObject.h"

VESPERSScanConfigurationDbObject::~VESPERSScanConfigurationDbObject(){}

VESPERSScanConfigurationDbObject::VESPERSScanConfigurationDbObject(QObject *parent)
	: AMDbObject(parent)
{
	I0_ = VESPERS::Imini;
	It_ = VESPERS::Ipost;
	fluorescenceDetector_ = VESPERS::NoXRF;
	ccdDetector_ = VESPERS::NoCCD;
	motor_ = VESPERS::NoMotor;
	ccdFileName_ = "";
	normalPosition_ = 888888.88;
}

VESPERSScanConfigurationDbObject::VESPERSScanConfigurationDbObject(const VESPERSScanConfigurationDbObject &original)
	: AMDbObject(original)
{
	I0_ = original.incomingChoice();
	It_ = original.transmissionChoice();
	fluorescenceDetector_ = original.fluorescenceDetector();
	ccdDetector_ = original.ccdDetector();
	motor_ = original.motor();
	ccdFileName_ = original.ccdFileName();
	normalPosition_ = original.normalPosition();
}

void VESPERSScanConfigurationDbObject::setIncomingChoice(VESPERS::IonChamber I0)
 {
	if (I0_ != I0){

		I0_ = I0;
		emit incomingChoiceChanged(I0_);
		emit incomingChoiceChanged(int(I0_));
		setModified(true);
	}
}

void VESPERSScanConfigurationDbObject::setTransmissionChoice(VESPERS::IonChamber It)
{
	if (It_ != It){

		It_ = It;
		emit transmissionChoiceChanged(It_);
		emit transmissionChoiceChanged(int(It_));
		setModified(true);
	}
}

void VESPERSScanConfigurationDbObject::setFluorescenceDetector(VESPERS::FluorescenceDetectors detector)
{
	if (fluorescenceDetector_ != detector){

		fluorescenceDetector_ = detector;
		emit fluorescenceDetectorChanged(fluorescenceDetector_);
		emit fluorescenceDetectorChanged(int(fluorescenceDetector_));
		setModified(true);
	}
}

void VESPERSScanConfigurationDbObject::setMotor(VESPERS::Motors choice)
{
	if (motor_ != choice) {

		motor_ = choice;
		emit motorChanged(motor_);
		emit motorChanged(int(motor_));
		setModified(true);
	}
}

void VESPERSScanConfigurationDbObject::setCCDDetector(VESPERS::CCDDetectors ccd)
{
	if (ccdDetector_ != ccd){

		ccdDetector_ = ccd;
		emit ccdDetectorChanged(ccdDetector_);
		emit ccdDetectorChanged(int(ccdDetector_));
		setModified(true);
	}
}

void VESPERSScanConfigurationDbObject::setCCDFileName(const QString &name)
{
	ccdFileName_ = name;
	emit ccdFileNameChanged(ccdFileName_);
	setModified(true);
}

void VESPERSScanConfigurationDbObject::setNormalPosition(double newPosition)
{
	if (normalPosition_ != newPosition){

		normalPosition_ = newPosition;
		emit normalPositionChanged(newPosition);
		setModified(true);
	}
}
