#ifndef BIOXASSIDEM2MIRRORBENDCONTROL_H
#define BIOXASSIDEM2MIRRORBENDCONTROL_H

#include "beamline/BioXAS/BioXASMirrorBendControl.h"

class BioXASSideM2MirrorBendControl : public BioXASMirrorBendControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideM2MirrorBendControl(const QString &name, const QString &units, QObject *parent = 0, const QString &description = QString());
	/// Destructor.
	virtual ~BioXASSideM2MirrorBendControl();

protected:
	/// Calculates and returns the mirror bend radius, for the given bender values.
	virtual double calculateBendRadius(double upstreamBenderValue, double downstreamBenderValue);

	/// Calculates and returns the mirror bend radius for the upstream bender value.
	virtual double calculateUpstreamBendRadius(double upstreamForce);
	/// Calculates and returns the mirror bend radius for the downstream bender value.
	virtual double calculateDownstreamBendRadius(double downstreamForce);

	/// Calculates and returns the upstream bender value, for the given bend radius.
	virtual double calculateUpstreamBenderValue(double bendRadius);
	/// Calculates and returns the downstream bender value, for the given bend radius.
	virtual double calculateDownstreamBenderValue(double bendRadius);
};

#endif // BIOXASSIDEM2MIRRORBENDCONTROL_H
