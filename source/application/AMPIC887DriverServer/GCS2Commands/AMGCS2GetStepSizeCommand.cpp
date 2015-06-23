#include "AMGCS2GetStepSizeCommand.h"

AMGCS2GetStepSizeCommand::AMGCS2GetStepSizeCommand(const QList<AMGCS2::Axis>& axes)
{
	axesToQuery_ = axes;
}

QString AMGCS2GetStepSizeCommand::outputString() const
{
}

QHash<AMGCS2::Axis, double> AMGCS2GetStepSizeCommand::axisStepSizes() const
{
}

bool AMGCS2GetStepSizeCommand::validateArguments()
{
}

bool AMGCS2GetStepSizeCommand::runImplementation()
{
}
