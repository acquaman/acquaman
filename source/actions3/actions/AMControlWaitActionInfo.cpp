/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMControlWaitActionInfo.h"

AMControlWaitActionInfo::AMControlWaitActionInfo(const AMControlInfo &setpoint, double timeout, AMControlWaitActionInfo::MatchType matchType, QObject *parent) :
	AMActionInfo3("Wait for control", "Wait for control", QString(), parent)
{
	controlInfo_ = setpoint;
	timeout_ = timeout;
	matchType_ = matchType;

}

AMControlWaitActionInfo::~AMControlWaitActionInfo(){}

AMControlWaitActionInfo::AMControlWaitActionInfo(const AMControlWaitActionInfo &other) :
	AMActionInfo3(other)
{
	controlInfo_.setValuesFrom(other.controlInfo());
	timeout_ = other.timeout();
	matchType_ = other.matchType();

}

AMActionInfo3 *AMControlWaitActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMControlWaitActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

double AMControlWaitActionInfo::timeout() const
{
	return timeout_;
}

AMControlWaitActionInfo::MatchType AMControlWaitActionInfo::matchType() const
{
	return matchType_;
}

void AMControlWaitActionInfo::setSetpoint(double setpoint)
{
	controlInfo_.setValue(setpoint);
	setModified(true);
	updateDescriptionText();
}

void AMControlWaitActionInfo::setControlInfo(const AMControlInfo &controlInfo)
{
	controlInfo_.setValuesFrom(controlInfo);
	setModified(true);
	updateDescriptionText();
}

void AMControlWaitActionInfo::setTimeout(double newTimeout)
{
	if (newTimeout <= 0)
		return;

	if (newTimeout == timeout_)
		return;

	timeout_ = newTimeout;
	setModified(true);
	updateDescriptionText();
}

void AMControlWaitActionInfo::setMatchType(AMControlWaitActionInfo::MatchType newMatchType)
{
	if (newMatchType == matchType_)
		return;

	matchType_ = newMatchType;
	setModified(true);
	updateDescriptionText();
}

int AMControlWaitActionInfo::matchTypeAsInt() const
{
	return int(matchType());
}

void AMControlWaitActionInfo::setMatchTypeAsInt(int newMatchType)
{
	setMatchType(AMControlWaitActionInfo::MatchType(newMatchType));
}

void AMControlWaitActionInfo::updateDescriptionText()
{
	QString d;

	switch(matchType_) {
	case AMControlWaitActionInfo::MatchEqual:
		d = QString("Waiting for %1 to become equal to %2.").arg(controlInfo_.name()).arg(controlInfo_.value());
		break;
	case AMControlWaitActionInfo::MatchGreaterThan:
		d = QString("Waiting for %1 to become greater than %2.").arg(controlInfo_.name()).arg(controlInfo_.value());
		break;
	case AMControlWaitActionInfo::MatchLessThan:
		d = QString("Waiting for %1 to become less than %2.").arg(controlInfo_.name()).arg(controlInfo_.value());
		break;
	case AMControlWaitActionInfo::MatchWithinTolerance:
		d = QString("Waiting for %1 to move within tolerance (%2) of %3.")
				.arg(controlInfo_.name())
				.arg(controlInfo_.tolerance())
				.arg(controlInfo_.value());
		break;
	case AMControlWaitActionInfo::MatchInvalid:
		d = QString("This control wait is invalid.");
		break;
	}

	setShortDescription(d);
	setLongDescription(d);
}
