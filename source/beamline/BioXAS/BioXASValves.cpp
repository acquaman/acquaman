#include "BioXASValves.h"

BioXASValves::BioXASValves(QObject *parent) :
	BioXASValvesControl("BioXASValves", parent)
{
	// Create valves and sets.
	// Front end vacuum valves.



	// Side beamline vacuum valves.

	vvrSide1_ = new CLSBiStateControl("SideVVR1", "SideVVR1", "VVR1607-5-I22-02:state", "VVR1607-5-I22-02:opr:open", "VVR1607-5-I22-02:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide2_ = new CLSBiStateControl("SideVVR2", "SideVVR2", "VVR1607-5-I22-03:state", "VVR1607-5-I22-03:opr:open", "VVR1607-5-I22-03:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide3_ = new CLSBiStateControl("SideVVR3", "SideVVR3", "VVR1607-5-I22-04:state", "VVR1607-5-I22-04:opr:open", "VVR1607-5-I22-04:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide4_ = new CLSBiStateControl("SideVVR4", "SideVVR4", "VVR1607-5-I22-06:state", "VVR1607-5-I22-06:opr:open", "VVR1607-5-I22-06:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide5_ = new CLSBiStateControl("SideVVR5", "SideVVR5", "VVR1607-5-I22-07:state", "VVR1607-5-I22-07:opr:open", "VVR1607-5-I22-07:opr:close", new AMControlStatusCheckerDefault(4), this);

	AMControlSet *sideValves = new AMControlSet(this);
	sideValves->addControl(vvrSide1_);
	sideValves->addControl(vvrSide2_);
	sideValves->addControl(vvrSide3_);
	sideValves->addControl(vvrSide4_);
	sideValves->addControl(vvrSide5_);

	setSideValves(sideValves);

	// Main beamline vacuum valves.

	vvrMain1_ = new CLSBiStateControl("MainVVR1", "MainVVR1", "VVR1607-5-I21-02:state", "VVR1607-5-I21-02:opr:open", "VVR1607-5-I21-02:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrMain2_ = new CLSBiStateControl("MainVVR2", "MainVVR2", "VVR1607-5-I21-03:state", "VVR1607-5-I21-03:opr:open", "VVR1607-5-I21-03:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrMain3_ = new CLSBiStateControl("MainVVR3", "MainVVR3", "VVR1607-5-I21-04:state", "VVR1607-5-I21-04:opr:open", "VVR1607-5-I21-04:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrMain4_ = new CLSBiStateControl("MainVVR4", "MainVVR4", "VVR1607-5-I21-06:state", "VVR1607-5-I21-06:opr:open", "VVR1607-5-I21-06:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrMain5_ = new CLSBiStateControl("MainVVR5", "MainVVR5", "VVR1607-5-I21-07:state", "VVR1607-5-I21-07:opr:open", "VVR1607-5-I21-07:opr:close", new AMControlStatusCheckerDefault(4), this);

	AMControlSet *mainValves = new AMControlSet(this);
	mainValves->addControl(vvrMain1_);
	mainValves->addControl(vvrMain2_);
	mainValves->addControl(vvrMain3_);
	mainValves->addControl(vvrMain4_);
	mainValves->addControl(vvrMain5_);

	setMainValves(mainValves);

	// Imaging beamline vacuum valves.

	vvrImaging1_ = new CLSBiStateControl("ImagingVVR1", "ImagingVVR1", "VVR1607-5-I10-02:state", "VVR1607-5-I10-02:opr:open", "VVR1607-5-I10-02:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrImaging2_ = new CLSBiStateControl("ImagingVVR2", "ImagingVVR2", "VVR1607-5-I10-03:state", "VVR1607-5-I10-03:opr:open", "VVR1607-5-I10-03:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrImaging3_ = new CLSBiStateControl("ImagingVVR3", "ImagingVVR3", "VVR1607-5-I10-04:state", "VVR1607-5-I10-04:opr:open", "VVR1607-5-I10-04:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrImaging4_ = new CLSBiStateControl("ImagingVVR4", "ImagingVVR4", "VVR1607-5-I10-05:state", "VVR1607-5-I10-05:opr:open", "VVR1607-5-I10-05:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrImaging5_ = new CLSBiStateControl("ImagingVVR5", "ImagingVVR5", "VVR1607-5-I10-07:state", "VVR1607-5-I10-07:opr:open", "VVR1607-5-I10-07:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrImaging6_ = new CLSBiStateControl("ImagingVVR6", "ImagingVVR6", "VVR1607-5-I10-08:state", "VVR1607-5-I10-08:opr:open", "VVR1607-5-I10-08:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrImaging7_ = new CLSBiStateControl("ImagingVVR7", "ImagingVVR7", "VVR1607-5-I10-09:state", "VVR1607-5-I10-09:opr:open", "VVR1607-5-I10-09:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrImaging8_ = new CLSBiStateControl("ImagingVVR8", "ImagingVVR8", "VVR1607-5-I10-10:state", "VVR1607-5-I10-10:opr:open", "VVR1607-5-I10-10:opr:close", new AMControlStatusCheckerDefault(4), this);

	AMControlSet *imagingValves = new AMControlSet(this);
	imagingValves->addControl(vvrImaging1_);
	imagingValves->addControl(vvrImaging2_);
	imagingValves->addControl(vvrImaging3_);
	imagingValves->addControl(vvrImaging4_);
	imagingValves->addControl(vvrImaging5_);
	imagingValves->addControl(vvrImaging6_);
	imagingValves->addControl(vvrImaging7_);
	imagingValves->addControl(vvrImaging8_);

	setImagingValves(imagingValves);
}

BioXASValves::~BioXASValves()
{

}
