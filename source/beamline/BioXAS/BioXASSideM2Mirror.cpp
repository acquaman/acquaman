#include "BioXASSideM2Mirror.h"

BioXASSideM2Mirror::BioXASSideM2Mirror(QObject *parent) :
    BioXASM2Mirror(parent)
{
	// Initialize member variables.

	screen_ = new AMPVControl("BioXAS Side M2 Mirror Screen Control", "VSC1607-5-I22-02:InBeam", "VSC1607-5-I22-02:InBeam", QString(), this);

	// Make connections.

	connect( screen_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
}

BioXASSideM2Mirror::~BioXASSideM2Mirror()
{

}
