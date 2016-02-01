#ifndef CLSFACILITYID_H
#define CLSFACILITYID_H

#include <QString>

namespace CLSFacilityID {
	/// The No of the beamline should be matching with the order of AMFacility in AMDatamanAppController::startupPopulateNewDatabase()
	enum Beamlines
	{
		SGMBeamline = 3,
		SGMSSBeamline = 3, //??
		VESPERSBeamline = 4,
		REIXSBeamline = 5, //??
		IDEASBeamline = 6,
		BioXASSideBeamline = 7,
		BioXASMainBeamline = 8,
		BioXASImagingBeamline = 9,
		SXRMBBeamline = 10
	};

	const QString SGMBeamlineName = "SGM";
	const QString SGMSSBeamlineName = "SGM";
	const QString VESPERSBeamlineName = "VESPERS";
	const QString REIXSBeamlineName = "REIXS";
	const QString IDEASBeamlineName = "IDEAS";
	const QString BioXASMainBeamlineName = "BioXASMain";
	const QString BioXASSideBeamlineName = "BioXASSide";
	const QString BioXASImagingBeamlineName = "BioXASImaging";
	const QString SXRMBBeamlineName = "SXRMB";

	inline QString beamlineName(CLSFacilityID::Beamlines beamline) {
		QString name;
		switch (beamline) {
		case CLSFacilityID::SGMBeamline:
			name = CLSFacilityID::SGMBeamlineName;
			break;
//		case CLSBeamline::SGMSSBeamline: //Same as SGM
//			name = CLSBeamline::SGMSSBeamlineName;
//			break;
		case CLSFacilityID::VESPERSBeamline:
			name = CLSFacilityID::VESPERSBeamlineName;
			break;
		case CLSFacilityID::REIXSBeamline:
			name = CLSFacilityID::REIXSBeamlineName;
			break;
//		case CLSBeamline::IDEASBeamline: //same as REIX
//			name = CLSBeamline::IDEASBeamlineName;
//			break;
		case CLSFacilityID::BioXASMainBeamline:
			name = CLSFacilityID::BioXASMainBeamlineName;
			break;
		case CLSFacilityID::BioXASSideBeamline:
			name = CLSFacilityID::BioXASSideBeamlineName;
			break;
		case CLSFacilityID::BioXASImagingBeamline:
			name = CLSFacilityID::BioXASImagingBeamlineName;
			break;
		case CLSFacilityID::SXRMBBeamline:
			name = CLSFacilityID::SXRMBBeamlineName;
			break;
		default:
			name = "Unknown";
			break;
		}

		return name;
	}
}


#endif // CLSFACILITYID_H
