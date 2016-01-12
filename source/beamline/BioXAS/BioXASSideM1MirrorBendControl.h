#ifndef BIOXASSIDEM1MIRRORBENDCONTROL_H
#define BIOXASSIDEM1MIRRORBENDCONTROL_H

#include "beamline/BioXAS/BioXASMirrorBendControl.h"

class BioXASSideM1MirrorBendControl : public BioXASMirrorBendControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideM1MirrorBendControl(const QString &name, const QString &units, QObject *parent = 0, const QString &description = QString());
	/// Destructor.
	virtual ~BioXASSideM1MirrorBendControl();

protected:
	/// Calculates and returns the mirror bend radius, for the given bender values.
	virtual double calculateBendRadius(double upstreamBenderValue, double downstreamBenderValue) const;

	/// Calculates and returns the mirror bend radius for the upstream bender value.
	virtual double calculateUpstreamBendRadius(double upstreamForce) const;
	/// Calculates and returns the mirror bend radius for the downstream bender value.
	virtual double calculateDownstreamBendRadius(double downstreamForce) const;

	/// Calculates and returns the upstream bender value, for the given bend radius.
	virtual double calculateUpstreamBenderValue(double bend) const;
	/// Calculates and returns the downstream bender value, for the given bend radius.
	virtual double calculateDownstreamBenderValue(double bend) const;
};

#endif // BIOXASSIDEM1MIRRORBENDCONTROL_H
