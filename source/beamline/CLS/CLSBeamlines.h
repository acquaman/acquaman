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

	const QString SGM_bl_name = "SGM";
	const QString SGMSS_bl_name = "SGM";
	const QString VESPERS_bl_name = "VESPERS";
	const QString REIXS_bl_name = "REIXS";
	const QString IDEAS_bl_name = "IDEAS";
	const QString BioXAS_Main_bl_name = "BioXASMain";
	const QString BioXAS_Side_bl_name = "BioXASSide";
	const QString BioXAS_Imaging_bl_name = "BioXASImaging";
	const QString SXRMB_bl_name = "SXRMB";
}


#endif // CLSBEAMLINES_H
