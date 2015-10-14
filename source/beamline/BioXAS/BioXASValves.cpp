#include "BioXASValves.h"

BioXASValves::BioXASValves(QObject *parent) :
	BioXASBeamlineComponent("BioXASValves", parent)
{
	// Create valves control.

	valvesControl_ = new BioXASValvesControl("BioXASValvesControl", this);

	// Create valves and sets, and add them to the valves control.
	// Front end vacuum valves.

	vvr1_ = new CLSBiStateControl("VVR1", "VVR1", "VVR1407-I00-01:state", "VVR1407-I00-01:opr:open", "VVR1407-I00-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvr2_ = new CLSBiStateControl("VVR2", "VVR2", "VVR1607-5-I00-01:state", "VVR1607-5-I00-01:opr:open", "VVR1607-5-I00-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvr3_ = new CLSBiStateControl("VVR3", "VVR3", "VVR1607-5-I22-01:state", "VVR1607-5-I22-01:opr:open", "VVR1607-5-I22-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvr4_ = new CLSBiStateControl("VVR4", "VVR4", "VVR1607-5-I21-01:state", "VVR1607-5-I21-01:opr:open", "VVR1607-5-I21-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvr5_ = new CLSBiStateControl("VVR5", "VVR5", "VVR1607-5-I10-01:state", "VVR1607-5-I10-01:opr:open", "VVR1607-5-I10-01:opr:close", new AMControlStatusCheckerDefault(4), this);

	frontEndValves_ = new AMControlSet(this);
	frontEndValves_->addControl(vvr1_);
	frontEndValves_->addControl(vvr2_);
	frontEndValves_->addControl(vvr3_);
	frontEndValves_->addControl(vvr4_);
	frontEndValves_->addControl(vvr5_);

	valvesControl_->setFrontEndValves(frontEndValves_);

	// Side beamline vacuum valves.

	vvrSide1_ = new CLSBiStateControl("SideVVR1", "SideVVR1", "VVR1607-5-I22-02:state", "VVR1607-5-I22-02:opr:open", "VVR1607-5-I22-02:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide2_ = new CLSBiStateControl("SideVVR2", "SideVVR2", "VVR1607-5-I22-03:state", "VVR1607-5-I22-03:opr:open", "VVR1607-5-I22-03:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide3_ = new CLSBiStateControl("SideVVR3", "SideVVR3", "VVR1607-5-I22-04:state", "VVR1607-5-I22-04:opr:open", "VVR1607-5-I22-04:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide4_ = new CLSBiStateControl("SideVVR5", "SideVVR5", "VVR1607-5-I22-06:state", "VVR1607-5-I22-06:opr:open", "VVR1607-5-I22-06:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide5_ = new CLSBiStateControl("SideVVR6", "SideVVR6", "VVR1607-5-I22-07:state", "VVR1607-5-I22-07:opr:open", "VVR1607-5-I22-07:opr:close", new AMControlStatusCheckerDefault(4), this);

	sideValves_ = new AMControlSet(this);
	sideValves_->addControl(vvrSide1_);
	sideValves_->addControl(vvrSide2_);
	sideValves_->addControl(vvrSide3_);
	sideValves_->addControl(vvrSide4_);
	sideValves_->addControl(vvrSide5_);

	valvesControl_->setSideValves(sideValves_);

	// Main beamline vacuum valves.

	vvrMain1_ = new CLSBiStateControl("MainVVR1", "MainVVR1", "VVR1607-5-I21-02:state", "VVR1607-5-I21-02:opr:open", "VVR1607-5-I21-02:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrMain2_ = new CLSBiStateControl("MainVVR2", "MainVVR2", "VVR1607-5-I21-03:state", "VVR1607-5-I21-03:opr:open", "VVR1607-5-I21-03:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrMain3_ = new CLSBiStateControl("MainVVR3", "MainVVR3", "VVR1607-5-I21-04:state", "VVR1607-5-I21-04:opr:open", "VVR1607-5-I21-04:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrMain4_ = new CLSBiStateControl("MainVVR5", "MainVVR5", "VVR1607-5-I21-06:state", "VVR1607-5-I21-06:opr:open", "VVR1607-5-I21-06:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrMain5_ = new CLSBiStateControl("MainVVR6", "MainVVR6", "VVR1607-5-I21-07:state", "VVR1607-5-I21-07:opr:open", "VVR1607-5-I21-07:opr:close", new AMControlStatusCheckerDefault(4), this);

	mainValves_ = new AMControlSet(this);
	mainValves_->addControl(vvrMain1_);
	mainValves_->addControl(vvrMain2_);
	mainValves_->addControl(vvrMain3_);
	mainValves_->addControl(vvrMain4_);
	mainValves_->addControl(vvrMain5_);

	valvesControl_->setMainValves(mainValves_);

	// Imaging beamline vacuum valves.

	vvrImaging1_ = new CLSBiStateControl("ImagingVVR1", "ImagingVVR1", "VVR1607-5-I10-02:state", "VVR1607-5-I10-02:opr:open", "VVR1607-5-I10-02:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrImaging2_ = new CLSBiStateControl("ImagingVVR2", "ImagingVVR2", "VVR1607-5-I10-03:state", "VVR1607-5-I10-03:opr:open", "VVR1607-5-I10-03:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrImaging3_ = new CLSBiStateControl("ImagingVVR3", "ImagingVVR3", "VVR1607-5-I10-04:state", "VVR1607-5-I10-04:opr:open", "VVR1607-5-I10-04:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrImaging4_ = new CLSBiStateControl("ImagingVVR4", "ImagingVVR4", "VVR1607-5-I10-05:state", "VVR1607-5-I10-05:opr:open", "VVR1607-5-I10-05:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrImaging5_ = new CLSBiStateControl("ImagingVVR6", "ImagingVVR6", "VVR1607-5-I10-07:state", "VVR1607-5-I10-07:opr:open", "VVR1607-5-I10-07:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrImaging6_ = new CLSBiStateControl("ImagingVVR7", "ImagingVVR7", "VVR1607-5-I10-08:state", "VVR1607-5-I10-08:opr:open", "VVR1607-5-I10-08:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrImaging7_ = new CLSBiStateControl("ImagingVVR8", "ImagingVVR8", "VVR1607-5-I10-09:state", "VVR1607-5-I10-09:opr:open", "VVR1607-5-I10-09:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrImaging8_ = new CLSBiStateControl("ImagingVVR9", "ImagingVVR9", "VVR1607-5-I10-10:state", "VVR1607-5-I10-10:opr:open", "VVR1607-5-I10-10:opr:close", new AMControlStatusCheckerDefault(4), this);

	imagingValves_ = new AMControlSet(this);
	imagingValves_->addControl(vvrImaging1_);
	imagingValves_->addControl(vvrImaging2_);
	imagingValves_->addControl(vvrImaging3_);
	imagingValves_->addControl(vvrImaging4_);
	imagingValves_->addControl(vvrImaging5_);
	imagingValves_->addControl(vvrImaging6_);
	imagingValves_->addControl(vvrImaging7_);
	imagingValves_->addControl(vvrImaging8_);

	valvesControl_->setImagingValves(imagingValves_);
}

BioXASValves::~BioXASValves()
{

}

bool BioXASValves::isConnected() const
{
	bool connected = (
				frontEndValves_ && frontEndValves_->isConnected() &&
				sideValves_ && sideValves_->isConnected() &&
				mainValves_ && mainValves_->isConnected() &&
				imagingValves_ && imagingValves_->isConnected()
				);

	return connected;
}
