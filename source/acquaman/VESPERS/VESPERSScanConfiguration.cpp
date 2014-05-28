#include "VESPERSScanConfiguration.h"

 VESPERSScanConfiguration::~VESPERSScanConfiguration(){}
VESPERSScanConfiguration::VESPERSScanConfiguration()
{
	dbObject_ = new VESPERSScanConfigurationDbObject;
	timeOffset_ = 0.4;
	totalTime_ = 0;
}

VESPERSScanConfiguration::VESPERSScanConfiguration(const VESPERSScanConfiguration &original)
{
	dbObject_ = new VESPERSScanConfigurationDbObject(*original.dbObject());
	timeOffset_ = original.timeOffset();
	totalTime_ = original.totalTime();
}

void VESPERSScanConfiguration::dbWriteScanConfigurationDbObject(AMDbObject *object)
{
	VESPERSScanConfigurationDbObject *dbo;

	if ((dbo = qobject_cast<VESPERSScanConfigurationDbObject *>(object)))
		dbObject_ = dbo;
}

QString VESPERSScanConfiguration::fluorescenceHeaderString(VESPERS::FluorescenceDetectors detector) const
{
	QString string = "";

	if (detector == VESPERS::NoXRF)
		string.append("Fluorescence Detector:\tNone\n");

	else if (detector == VESPERS::SingleElement)
		string.append("Fluorescence Detector:\tSingle Element Vortex Detector\n");

	else if (detector == VESPERS::FourElement)
		string.append("Fluorescence Detector:\tFour Element Vortex Detector\n");

	else if (detector == (VESPERS::SingleElement | VESPERS::FourElement))
		string.append("Fluorescence Detector:\tSingle Element Vortex Detector and Four Element Vortex Detector\n");

	return string;
}

QString VESPERSScanConfiguration::incomingChoiceHeaderString(VESPERS::IonChamber detector) const
{
	QString string = "";

	switch(detector){

	case VESPERS::Isplit:
		string.append("I0:\tIsplit - The split ion chamber.\n");
		break;
	case VESPERS::Iprekb:
		string.append("I0:\tIprekb - The ion chamber before the KB mirror box.\n");
		break;
	case VESPERS::Imini:
		string.append("I0:\tImini - The small ion chamber immediately after the KB mirror box.\n");
		break;
	case VESPERS::Ipost:
		string.append("I0:\tIpost - The ion chamber at the end of the beamline.\n");
		break;
	}

	return string;
}

QString VESPERSScanConfiguration::transmissionChoiceHeaderString(VESPERS::IonChamber detector) const
{
	QString string = "";

	switch(detector){

	case VESPERS::Isplit:
		string.append("It:\tIsplit - The split ion chamber.\n");
		break;
	case VESPERS::Iprekb:
		string.append("It:\tIprekb - The ion chamber before the KB mirror box.\n");
		break;
	case VESPERS::Imini:
		string.append("It:\tImini - The small ion chamber immediately after the KB mirror box.\n");
		break;
	case VESPERS::Ipost:
		string.append("It:\tIpost - The ion chamber at the end of the beamline.\n");
		break;
	}

	return string;
}

QString VESPERSScanConfiguration::motorHeaderString(VESPERS::Motors motor) const
{
	QString string = "";

	if (motor == VESPERS::H)
		string.append("Using horizontal (H) pseudo motor.\n");

	else if (motor == VESPERS::X)
		string.append("Using horizontal (X) real motor.\n");

	else if (motor == VESPERS::V)
		string.append("Using vertical (V) pseudo motor.\n");

	else if (motor == VESPERS::Z)
		string.append("Using vertical (Z) real motor.\n");

	else if (motor == (VESPERS::H | VESPERS::V))
		string.append("Using pseudo motors: H and V.\n");

	else if (motor == (VESPERS::X | VESPERS::Z))
		string.append("Using real motors: X and Z.\n");

	return string;
}

QString VESPERSScanConfiguration::ccdDetectorHeaderString(VESPERS::CCDDetectors detector) const
{
	QString string = "";

	if (!detector.testFlag(VESPERS::NoCCD)){

		if (detector == VESPERS::Roper)
			string.append(QString("CCD detector used: Roper\n").arg("Roper"));

		else if (detector == VESPERS::Mar)
			string.append("CCD detector used: Mar\n");

		else if (detector == VESPERS::Pilatus)
			string.append("CCD detector used: Pilatus\n");

		string.append(QString("\nFilename for XRD images:\t%1\n").arg(ccdFileName()));
	}

	return string;
}
