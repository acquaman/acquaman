#include "SXRMBUserConfiguration.h"

SXRMBUserConfiguration::SXRMBUserConfiguration(QObject *parent)
	: CLSUserConfiguration(parent)
{
	setName("User Configuration");

	setFluorescenceDetector(SXRMB::BrukerDetector);
}

SXRMBUserConfiguration::~SXRMBUserConfiguration()
{

}

void SXRMBUserConfiguration::dbLoadFluorescenceDetector(int detector) {
	setFluorescenceDetector((SXRMB::FluorescenceDetectors)detector);
}

void SXRMBUserConfiguration::setFluorescenceDetector(SXRMB::FluorescenceDetectors detector)
{
	if (fluorescenceDetector_ != detector){

		fluorescenceDetector_ = detector;
		setModified(true);
	}
}
