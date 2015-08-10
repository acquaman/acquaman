#include "BioXASSideDBHRMirrors.h"
#include "beamline/CLS/CLSMAXvMotor.h"

BioXASSideDBHRMirrors::BioXASSideDBHRMirrors(QObject *parent) :
	BioXASDBHRMirrors(parent)
{
	// Initialize inherited variables.

	m1Vertical_ = new CLSMAXvMotor("BioXASSideDBHRM1", "SMTR1607-6-I22-07", "BioXASSideDBHRM1", true, 0.005, 2.0, this, ":mm");
	m2Vertical_ = new CLSMAXvMotor("BioXASSideDBHRM2", "SMTR1607-6-I22-08", "BioXASSideDBHRM2", true, 0.005, 2.0, this, ":mm");
	pitch_ = new CLSMAXvMotor("BioXASSideDBHRPitch", "SMTR1607-6-I22-06", "BioXASSideDBHRPitch", true, 0.005, 2.0, this, ":deg");
}

BioXASSideDBHRMirrors::~BioXASSideDBHRMirrors()
{

}
