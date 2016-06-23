/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "VESPERSScanConfiguration.h"

VESPERSScanConfiguration::VESPERSScanConfiguration()
	:CLSScanConfiguration(new VESPERSScanConfigurationDbObject)
{
//	dbObject_ = new VESPERSScanConfigurationDbObject;
	dbObject_ = qobject_cast<VESPERSScanConfigurationDbObject *>(configurationDbObject_);

	timeOffset_ = 0.4;
	totalTime_ = 0;
	closeFastShutter_ = false;
	returnToOriginalPosition_ = false;
	cleanupScaler_ = false;
}

VESPERSScanConfiguration::VESPERSScanConfiguration(const VESPERSScanConfiguration &original)
	:CLSScanConfiguration(new VESPERSScanConfigurationDbObject(*original.dbObject()))
{
//	dbObject_ = new VESPERSScanConfigurationDbObject(*original.dbObject());
	dbObject_ = qobject_cast<VESPERSScanConfigurationDbObject *>(configurationDbObject_);

	timeOffset_ = original.timeOffset();
	totalTime_ = original.totalTime();
	closeFastShutter_ = original.closeFastShutter();
	returnToOriginalPosition_ = original.returnToOriginalPosition();
	cleanupScaler_ = original.cleanupScaler();
}

VESPERSScanConfiguration::~VESPERSScanConfiguration()
{
//	dbObject_->deleteLater();
}

//void VESPERSScanConfiguration::dbWriteScanConfigurationDbObject(AMDbObject *object)
//{
//	VESPERSScanConfigurationDbObject *dbo;

//	if ((dbo = qobject_cast<VESPERSScanConfigurationDbObject *>(object)))
//		dbObject_ = dbo;
//}

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

	else if (motor == VESPERS::BigBeamX)
		string.append("Using big beam X motor.\n");

	else if (motor == VESPERS::BigBeamZ)
		string.append("Using big beam Z motor.\n");

	else if (motor == (VESPERS::BigBeamX | VESPERS::BigBeamZ))
		string.append("Using big beam motors: X and Z.\n");

	else if (motor == VESPERS::WireH)
		string.append("Using horizontal (H) wire pseudo motor.\n");

	else if (motor == VESPERS::WireV)
		string.append("Using vertical (V) wire pseudo motor.\n");

	else if (motor == VESPERS::WireN)
		string.append("Using normal (N) wire pseudo motor.\n");

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

//QString VESPERSScanConfiguration::regionsOfInterestHeaderString(const QList<AMRegionOfInterest *> &regions) const
//{
//	QString string = "";

//	if (!regions.isEmpty()){

//		string.append("\nRegions Of Interest\n");

//		foreach (AMRegionOfInterest *region, regions)
//			string.append(QString("%1\t%2 eV\t%3 eV\n").arg(region->name()).arg(region->lowerBound()).arg(region->upperBound()));
//	}

//	return string;
//}
