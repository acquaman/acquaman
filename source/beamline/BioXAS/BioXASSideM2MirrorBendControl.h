#ifndef BIOXASSIDEM2MIRRORBENDCONTROL_H
#define BIOXASSIDEM2MIRRORBENDCONTROL_H

#include "beamline/BioXAS/BioXASMirrorBendControl.h"

#define BIOXASSIDEM2MIRRORBENDCONTROL_MIN_VALUE
#define BIOXASSIDEM2MIRRORBENDCONTROL_MAX_VALUE

class BioXASSideM2MirrorBendControl : public BioXASMirrorBendControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideM2MirrorBendControl(const QString &name, const QString &units, double upstreamLength, double downstreamLength, QObject *parent = 0, const QString &description = QString());
	/// Destructor.
	virtual ~BioXASSideM2MirrorBendControl();

protected:
	/// Calculates and returns the upstream bender value, for the given bend radius.
	virtual double calculateUpstreamBenderValue(double bendRadius);
	/// Calculates and returns the downstream bender value, for the given bend radius.
	virtual double calculateDownstreamBenderValue(double bendRadius);
	/// Calculates and returns the mirror bend radius, for the given bender values.
	virtual double calculateBendRadius(double upstreamBenderValue, double downstreamBenderValue);
};

#endif // BIOXASSIDEM2MIRRORBENDCONTROL_H
