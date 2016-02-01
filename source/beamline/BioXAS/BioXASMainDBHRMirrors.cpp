#include "BioXASMainDBHRMirrors.h"

BioXASMainDBHRMirrors::BioXASMainDBHRMirrors(QObject *parent) :
	BioXASDBHRMirrors("MainDBHRMirrors", parent)
{
	// Initialize inherited variables.

	m1Vertical_ = new CLSMAXvMotor(name()+"M1", "SMTR1607-7-I21-08", name()+"M1", true, 0.005, 2.0, this, ":mm");
	m2Vertical_ = new CLSMAXvMotor(name()+"M2", "SMTR1607-7-I21-09", name()+"M2", true, 0.005, 2.0, this, ":mm");
	pitch_ = new CLSMAXvMotor(name()+"Pitch", "SMTR1607-7-I21-07", name()+"Pitch", true, 0.005, 2.0, this, ":deg");
}

BioXASMainDBHRMirrors::~BioXASMainDBHRMirrors()
{

}
