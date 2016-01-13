#ifndef BIOXASMAINM1MIRRORBENDCONTROL_H
#define BIOXASMAINM1MIRRORBENDCONTROL_H

#include "beamline/BioXAS/BioXASMirrorBendControl.h"

class BioXASMainM1MirrorBendControl : public BioXASMirrorBendControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMainM1MirrorBendControl(const QString &name, const QString &units, QObject *parent = 0, const QString &description = QString());
	/// Destructor.
	virtual ~BioXASMainM1MirrorBendControl();

protected:
	/// Calculates and returns the mirror bend radius, for the given bender values.
	virtual double calculateBendRadius(double upstreamBenderValue, double downstreamBenderValue) const;

	/// Calculates and returns the upstream bender value, for the given bend radius.
	virtual double calculateUpstreamBenderValue(double bend) const;
	/// Calculates and returns the downstream bender value, for the given bend radius.
	virtual double calculateDownstreamBenderValue(double bend) const;
};

#endif // BIOXASMAINM1MIRRORBENDCONTROL_H
