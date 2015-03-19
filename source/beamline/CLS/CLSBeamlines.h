#ifndef CLSBEAMLINES_H
#define CLSBEAMLINES_H

namespace CLSBeamline {
	enum CLSBeamlines
	{
		SGMBeamline = 3,
		SGMSSBeamline = 3, //??
		VESPERSBeamline = 4,
		REIXSBeamline = 5, //??
		IDEASBeamline = 5,
		BioXASMainBeamline = 6,
		BioXASSideBeamline = 7,
		BioXASImagingBeamline = 8,
		SXRMBBeamline = 9
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

	QString beamlineName(CLSBeamline::CLSBeamlines beamline) {
		QString name;
		switch (beamline) {
		case CLSBeamline::SGMBeamline:
			name = CLSBeamline::SGMBeamlineName;
			break;
//		case CLSBeamline::SGMSSBeamline: //Same as SGM
//			name = CLSBeamline::SGMSSBeamlineName;
//			break;
		case CLSBeamline::VESPERSBeamline:
			name = CLSBeamline::VESPERSBeamlineName;
			break;
		case CLSBeamline::REIXSBeamline:
			name = CLSBeamline::REIXSBeamlineName;
			break;
//		case CLSBeamline::IDEASBeamline: //same as REIX
//			name = CLSBeamline::IDEASBeamlineName;
//			break;
		case CLSBeamline::BioXASMainBeamline:
			name = CLSBeamline::BioXASMainBeamlineName;
			break;
		case CLSBeamline::BioXASSideBeamline:
			name = CLSBeamline::BioXASSideBeamlineName;
			break;
		case CLSBeamline::BioXASImagingBeamline:
			name = CLSBeamline::BioXASImagingBeamlineName;
			break;
		case CLSBeamline::SXRMBBeamline:
			name = CLSBeamline::SXRMBBeamlineName;
			break;
		default:
			name = "Unknown";
			break;
		}

		return name;
	}
}


#endif // CLSBEAMLINES_H
