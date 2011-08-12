#include "VESPERSMonochromator.h"

VESPERSMonochromator::VESPERSMonochromator(QObject *parent) :
	QObject(parent)
{
	Eo_ = new AMProcessVariable("07B2_Mono_SineB_Eo", true, this);
	Ea_ = new AMProcessVariable("07B2_Mono_SineB_Ea", true, this);
	delE_ = new AMProcessVariable("07B2_Mono_SineB_delE", true, this);
	Efbk_ = new AMProcessVariable("07B2_Mono_SineB_Egec:eV", true, this);
	Ksp_ = new AMProcessVariable("07B2_Mono_SineB_K", true, this);
	Kfbk_ = new AMProcessVariable("07B2_Mono_SineB_K:fbk", true, this);
	offsetAngle_ = new AMProcessVariable("07B2_Mono_SineB_ThOS", true, this);
	allowScan_ = new AMProcessVariable("07B2_Mono_ScanSineB", true, this);
	encoder_ = new AMProcessVariable("07B2_Mono_SineB_Use_eV", true, this);

	connect(Eo_, SIGNAL(valueChanged(double)), this, SIGNAL(EoChanged(double)));
	connect(Ea_, SIGNAL(valueChanged(double)), this, SIGNAL(EaChanged(double)));
	connect(delE_, SIGNAL(valueChanged(double)), this, SIGNAL(delEChanged(double)));
	connect(Efbk_, SIGNAL(valueChanged(double)), this, SIGNAL(energyChanged(double)));
	connect(Ksp_, SIGNAL(valueChanged(double)), this, SIGNAL(KSetpointChanged(double)));
	connect(Kfbk_, SIGNAL(valueChanged(double)), this, SIGNAL(KFeedbackChanged(double)));
	connect(offsetAngle_, SIGNAL(valueChanged(double)), this, SIGNAL(offsetAngleChanged(double)));
	connect(allowScan_, SIGNAL(valueChanged(int)), this, SLOT(onAllowScanChanged(int)));
	connect(encoder_, SIGNAL(valueChanged(int)), this, SLOT(onEncoderChanged(int)));
}
