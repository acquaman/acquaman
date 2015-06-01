#ifndef BIOXASSIDEM1MIRRORBENDCONTROL_H
#define BIOXASSIDEM1MIRRORBENDCONTROL_H

#include "beamline/BioXAS/BioXASMirrorBendControl.h"

#define BIOXASSIDEM1MIRRORBENDCONTROL_MIN_VALUE 7500
#define BIOXASSIDEM1MIRRORBENDCONTROL_MAX_VALUE 50000

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
	virtual double calculateBendRadius(double upstreamBenderValue, double downstreamBenderValue);

	/// Calculates and returns the upstream bender value, for the given bend radius.
	virtual double calculateUpstreamBenderValue(double bend);
	/// Calculates and returns the downstream bender value, for the given bend radius.
	virtual double calculateDownstreamBenderValue(double bend);
};

#endif // BIOXASSIDEM1MIRRORBENDCONTROL_H
