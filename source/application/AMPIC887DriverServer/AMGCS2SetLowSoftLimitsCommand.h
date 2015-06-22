#ifndef AMGCS2SETLOWSOFTLIMITSCOMMAND_H
#define AMGCS2SETLOWSOFTLIMITSCOMMAND_H

#include "AMGCS2Command.h"
#include "AMGCS2.h"
#include <QHash>
class AMGCS2SetLowSoftLimitsCommand : public AMGCS2Command
{
public:
	AMGCS2SetLowSoftLimitsCommand(const QHash<AMGCS2::Axis, double>& axisLowLimits);

	virtual ~AMGCS2SetLowSoftLimitsCommand() {}

protected:
	virtual bool validateArguments();

	virtual bool runImplementation();

	QHash<AMGCS2::Axis, double> axisLowLimits_;
};

#endif // AMGCS2SETLOWSOFTLIMITSCOMMAND_H
