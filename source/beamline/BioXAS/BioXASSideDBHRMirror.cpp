#include "BioXASSideDBHRMirror.h"
#include "beamline/CLS/CLSMAXvMotor.h"

BioXASSideDBHRMirror::BioXASSideDBHRMirror(QObject *parent) :
    BioXASDBHRMirror(parent)
{
	// Set motor control instances.

	setPitchControl(new CLSMAXvMotor("BioXASSideDBHRPitch", "SMTR1607-6-I22-06", "BioXASSideDBHRPitch", true, 0.005, 2.0, this, ":deg"));
	setM1MirrorControl(new CLSMAXvMotor("BioXASSideDBHRM1", "SMTR1607-6-I22-07", "BioXASSideDBHRM1", true, 0.005, 2.0, this, ":mm"));
	setM2MirrorControl(new CLSMAXvMotor("BioXASSideDBHRM2", "SMTR1607-6-I22-08", "BioXASSideDBHRM2", true, 0.005, 2.0, this, ":mm"));
}

BioXASSideDBHRMirror::~BioXASSideDBHRMirror()
{

}
