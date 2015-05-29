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
	explicit BioXASSideM1MirrorBendControl(const QString &name, const QString &units, double upstreamLength, double downstreamLength, QObject *parent = 0, const QString &description = QString());
	/// Destructor.
	virtual ~BioXASSideM1MirrorBendControl();

	/// Returns the minimum value.
	virtual double minimumValue() const { return BIOXASSIDEM1MIRRORBENDCONTROL_MIN_VALUE; }
	/// Returns the maximum value.
	virtual double maximumValue() const { return BIOXASSIDEM1MIRRORBENDCONTROL_MAX_VALUE; }

protected:
	/// Calculates and returns the upstream bender value, for the given bend radius.
	virtual double calculateUpstreamBenderValue(double bend);
	/// Calculates and returns the downstream bender value, for the given bend radius.
	virtual double calculateDownstreamBenderValue(double bend);
	/// Calculates and returns the mirror bend radius, for the given bender values.
	virtual double calculateBendRadius(double upstreamBenderValue, double downstreamBenderValue);
};

#endif // BIOXASSIDEM1MIRRORBENDCONTROL_H
