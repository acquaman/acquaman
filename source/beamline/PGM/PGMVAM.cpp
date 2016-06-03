#include "PGMVAM.h"

PGMVAM::PGMVAM(const QString &name, QObject *parent) :
	AMConnectedControl(name, "", parent)
{
	// Upper blade control.

	upperBlade_ = new AMPVwStatusControl("SMTR16113I2001", "SMTR16113I2001:enc:fbk", "SMTR16113I2001:step", "SMTR16113I2001:status", "SMTR16113I2001:stop", this);
	addChildControl(upperBlade_);

	// Lower blade control.

	lowerBlade_ = new AMPVwStatusControl("SMTR16113I2002", "SMTR16113I2002:enc:fbk", "SMTR16113I2002:step", "SMTR16113I2002:status", "SMTR16113I2002:stop", this);
	addChildControl(lowerBlade_);

	// Outboard blade control.

	outboardBlade_ = new AMPVwStatusControl("SMTR16113I2003", "SMTR16113I2003:enc:fbk", "SMTR16113I2003:step", "SMTR16113I2003:status", "SMTR16113I2003:stop", this);
	addChildControl(outboardBlade_);

	// Inboard blade control.

	inboardBlade_ = new AMPVwStatusControl("SMTR16113I2004", "SMTR16113I2004:enc:fbk", "SMTR16113I2004:step", "SMTR16113I2004:status", "SMTR16113I2004:stop", this);
	addChildControl(inboardBlade_);
}

PGMVAM::~PGMVAM()
{

}

