#include "REIXSControlMoveAction.h"

#include "beamline/REIXS/REIXSBeamline.h"

QMap<QString, AMControl*> REIXSControlMoveAction::name2Control_;

AMControl * REIXSControlMoveAction::getControlForAction(const AMControlInfo &controlInfo) const
{
	if(name2Control_.isEmpty())
		populateControlLookup();

	return name2Control_.value(controlInfo.name(), 0);
}

void REIXSControlMoveAction::populateControlLookup() const
{
	REIXSBeamline* bl = REIXSBeamline::bl();

	name2Control_.insert("sampleX", bl->sampleChamber()->x());
	name2Control_.insert("sampleY", bl->sampleChamber()->y());
	name2Control_.insert("sampleZ", bl->sampleChamber()->z());
	name2Control_.insert("sampleTheta", bl->sampleChamber()->r());
	name2Control_.insert("loadLockZ", bl->sampleChamber()->loadLockZ());
	name2Control_.insert("loadLockTheta", bl->sampleChamber()->loadLockR());

	name2Control_.insert("spectrometer", bl->spectrometer());

	// These should probably not be shown to end-users, but we want them for comissioning

	name2Control_.insert("spectrometerRotationDrive", bl->spectrometer()->spectrometerRotationDrive());
	name2Control_.insert("detectorTranslation", bl->spectrometer()->detectorTranslation());
	name2Control_.insert("detectorTiltDrive", bl->spectrometer()->detectorTiltDrive());
	name2Control_.insert("hexapodX", bl->spectrometer()->hexapod()->x());
	name2Control_.insert("hexapodX", bl->spectrometer()->hexapod()->y());
	name2Control_.insert("hexapodX", bl->spectrometer()->hexapod()->z());
	name2Control_.insert("hexapodR", bl->spectrometer()->hexapod()->r());
	name2Control_.insert("hexapodS", bl->spectrometer()->hexapod()->s());
	name2Control_.insert("hexapodT", bl->spectrometer()->hexapod()->t());
	name2Control_.insert("hexapodU", bl->spectrometer()->hexapod()->u());
	name2Control_.insert("hexapodV", bl->spectrometer()->hexapod()->v());
	name2Control_.insert("hexapodW", bl->spectrometer()->hexapod()->w());
}
