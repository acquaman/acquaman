#include "VESPERSScanConfiguration.h"

VESPERSScanConfiguration::VESPERSScanConfiguration()
{
	dbObject_ = new VESPERSScanConfigurationDbObject;
	timeOffset_ = 0.7;
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

QString VESPERSScanConfiguration::fluorescenceHeaderString(VESPERS::FluorescenceDetector detector) const
{
	QString string = "";

	switch((int)detector){

	case VESPERS::NoXRF:
		string.append("Fluorescence Detector:\tNone\n");
		break;
	case VESPERS::SingleElement:
		string.append("Fluorescence Detector:\tSingle Element Vortex Detector\n");
		break;
	case VESPERS::FourElement:
		string.append("Fluorescence Detector:\tFour Element Vortex Detector\n");
		break;
	case VESPERS::SingleElement | VESPERS::FourElement:
		string.append("Fluorescence Detector:\tSingle Element Vortex Detector and Four Element Vortex Detector\n");
		break;
	}

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

QString VESPERSScanConfiguration::regionOfInterestHeaderString(AMROIInfoList list) const
{
	QString string = "";

	string.append("\nRegions of Interest\n");

	for (int i = 0; i < list.count(); i++)
		string.append(list.at(i).name() + "\t" + QString::number(list.at(i).low()) + " eV\t" + QString::number(list.at(i).high()) + " eV\n");

	return string;
}

QString VESPERSScanConfiguration::motorHeaderString(VESPERS::Motor motor) const
{
	QString string = "";

	switch(int(motor)){

	case VESPERS::H:
		string.append("Using horizontal (H) pseudo motor.\n");
		break;

	case VESPERS::X:
		string.append("Using horizontal (X) real motor.\n");
		break;

	case VESPERS::V:
		string.append("Using vertical (V) pseudo motor.\n");
		break;

	case VESPERS::Z:
		string.append("Using vertical (Z) real motor.\n");
		break;

	case VESPERS::H | VESPERS::V:
		string.append("Using pseudo motors: H and V.\n");
		break;

	case VESPERS::X | VESPERS::Z:
		string.append("Using real motors: X and Z.\n");
		break;

	case VESPERS::BigBeamX:
		string.append("Using big beam X motor.\n");
		break;

	case VESPERS::BigBeamZ:
		string.append("Using big beam Z motor.\n");
		break;

	case VESPERS::BigBeamX | VESPERS::BigBeamZ:
		string.append("Using big beam motors: X and Z.\n");
		break;
	}

	return string;
}

QString VESPERSScanConfiguration::ccdDetectorHeaderString(VESPERS::CCDDetector detector) const
{
	QString string = "";
	if (detector != VESPERS::NoCCD){

		switch (int(detector)){

		case VESPERS::Roper:
			string.append(QString("CCD detector used: Roper\n").arg("Roper"));
			break;

		case VESPERS::Mar:
			string.append("CCD detector used: Mar\n");
			break;

		case VESPERS::Pilatus:
			string.append("CCD detector used: Pilatus\n");
			break;
		}

		string.append(QString("\nFilename for XRD images:\t%1\n").arg(ccdFileName()));
	}

	return string;
}
