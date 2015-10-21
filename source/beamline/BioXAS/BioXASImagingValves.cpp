#include "BioXASImagingValves.h"

BioXASImagingValves::BioXASImagingValves(QObject *parent) :
	BioXASValvesSetControl("BioXASImagingValves", parent)
{
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

	setValveSet(imagingValves);
}

BioXASImagingValves::~BioXASImagingValves()
{

}
