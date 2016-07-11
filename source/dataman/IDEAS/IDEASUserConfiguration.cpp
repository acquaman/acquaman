#include "IDEASUserConfiguration.h"

IDEASUserConfiguration::IDEASUserConfiguration(QObject *parent)
	: CLSUserConfiguration(parent)
{
	setName("User Configuration");
	setFluorescenceDetector(IDEAS::Ketek);
}

IDEASUserConfiguration::~IDEASUserConfiguration()
{

}

void IDEASUserConfiguration::dbLoadFluorescenceDetector(int detector)
{
	setFluorescenceDetector((IDEAS::FluorescenceDetectors)detector);
}

void IDEASUserConfiguration::setFluorescenceDetector(IDEAS::FluorescenceDetectors detector)
{
	if (fluorescenceDetector_ != detector){

		fluorescenceDetector_ = detector;
		setModified(true);
	}
}
