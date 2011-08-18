#include "VESPERSMonochromator.h"

VESPERSMonochromator::VESPERSMonochromator(QObject *parent) :
	QObject(parent)
{
	Eo_ = new AMPVControl("Energy Setpoint", "07B2_Mono_SineB_Eo", "07B2_Mono_SineB_Eo", QString(), this);
	energy_ = new AMPVControl("Acutal Energy", "07B2_Mono_SineB_Egec:eV", "07B2_Mono_SineB_Ea", QString(), this);
	delE_ = new AMPVControl("Relative Energy", "07B2_Mono_SineB_delE", "07B2_Mono_SineB_delE", QString(), this);
	K_ = new AMPVControl("K-space", "07B2_Mono_SineB_K:fbk", "07B2_Mono_SineB_K", QString(), this);
	offsetAngle_ = new AMPVControl("Offset Angle", "07B2_Mono_SineB_ThOS", "07B2_Mono_SineB_ThOS", QString(), this);
	allowScan_ = new AMPVControl("Scan Allow Control", "07B2_Mono_ScanSineB", "07B2_Mono_ScanSineB", QString(), this);
	encoder_ = new AMPVControl("Energy Encoder Precision", "07B2_Mono_SineB_Use_eV", "07B2_Mono_SineB_Use_eV", QString(), this);

	connect(Eo_, SIGNAL(valueChanged(double)), this, SIGNAL(EoChanged(double)));
	connect(energy_, SIGNAL(setpointChanged(double)), this, SIGNAL(EaChanged(double)));
	connect(energy_, SIGNAL(valueChanged(double)), this, SIGNAL(energyChanged(double)));
	connect(delE_, SIGNAL(valueChanged(double)), this, SIGNAL(delEChanged(double)));
	connect(K_, SIGNAL(setpointChanged(double)), this, SIGNAL(KSetpointChanged(double)));
	connect(K_, SIGNAL(valueChanged(double)), this, SIGNAL(KFeedbackChanged(double)));
	connect(offsetAngle_, SIGNAL(valueChanged(double)), this, SIGNAL(offsetAngleChanged(double)));
	connect(allowScan_, SIGNAL(valueChanged(double)), this, SLOT(onAllowScanChanged(double)));
	connect(encoder_, SIGNAL(valueChanged(double)), this, SLOT(onEncoderChanged(double)));
}
