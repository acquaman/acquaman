#include "BioXASSideDBHRMirrors.h"
#include "beamline/CLS/CLSMAXvMotor.h"

BioXASSideDBHRMirrors::BioXASSideDBHRMirrors(QObject *parent) :
	BioXASDBHRMirrors("SideDBHRMirrors", parent)
{
	// Initialize inherited variables.

	m1Vertical_ = new CLSMAXvMotor(name()+"M1", "SMTR1607-6-I22-07", name()+"M1", true, 0.005, 2.0, this, ":mm");
	m2Vertical_ = new CLSMAXvMotor(name()+"M2", "SMTR1607-6-I22-08", name()+"M2", true, 0.005, 2.0, this, ":mm");
	pitch_ = new CLSMAXvMotor(name()+"Pitch", "SMTR1607-6-I22-06", name()+"Pitch", true, 0.005, 2.0, this, ":deg");
}

BioXASSideDBHRMirrors::~BioXASSideDBHRMirrors()
{

}
