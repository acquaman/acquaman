#include "BioXASImagingValves.h"

BioXASImagingValves::BioXASImagingValves(QObject *parent) :
	BioXASValvesSetControl("BioXASImagingValves", parent)
{
	vvrImaging1_ = new CLSBiStateControl("VVR1607-5-I10-02", "VVR1607-5-I10-02", "VVR1607-5-I10-02:state", "VVR1607-5-I10-02:opr:open", "VVR1607-5-I10-02:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrImaging2_ = new CLSBiStateControl("VVR1607-5-I10-03", "VVR1607-5-I10-03", "VVR1607-5-I10-03:state", "VVR1607-5-I10-03:opr:open", "VVR1607-5-I10-03:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrImaging3_ = new CLSBiStateControl("VVR1607-5-I10-04", "VVR1607-5-I10-04", "VVR1607-5-I10-04:state", "VVR1607-5-I10-04:opr:open", "VVR1607-5-I10-04:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrImaging4_ = new CLSBiStateControl("VVR1607-5-I10-05", "VVR1607-5-I10-05", "VVR1607-5-I10-05:state", "VVR1607-5-I10-05:opr:open", "VVR1607-5-I10-05:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrImaging5_ = new CLSBiStateControl("VVR1607-5-I10-07", "VVR1607-5-I10-07", "VVR1607-5-I10-07:state", "VVR1607-5-I10-07:opr:open", "VVR1607-5-I10-07:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrImaging6_ = new CLSBiStateControl("VVR1607-5-I10-08", "VVR1607-5-I10-08", "VVR1607-5-I10-08:state", "VVR1607-5-I10-08:opr:open", "VVR1607-5-I10-08:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrImaging7_ = new CLSBiStateControl("VVR1607-5-I10-10", "VVR1607-5-I10-10", "VVR1607-5-I10-10:state", "VVR1607-5-I10-10:opr:open", "VVR1607-5-I10-10:opr:close", new AMControlStatusCheckerDefault(4), this);

	AMControlSet *imagingValves = new AMControlSet(this);
	imagingValves->addControl(vvrImaging1_);
	imagingValves->addControl(vvrImaging2_);
	imagingValves->addControl(vvrImaging3_);
	imagingValves->addControl(vvrImaging4_);
	imagingValves->addControl(vvrImaging5_);
	imagingValves->addControl(vvrImaging6_);
	imagingValves->addControl(vvrImaging7_);

	setValveSet(imagingValves);
}

BioXASImagingValves::~BioXASImagingValves()
{

}
