#include "AMTrapezoidVelocityProfile.h"
#include <math.h>
AMTrapezoidVelocityProfile::AMTrapezoidVelocityProfile()
{
	startPosition_ = 0;
	endPosition_ = 0;
	acceleration_ = 0;
	time_ = 0;
	isValid_ = false;
}

AMTrapezoidVelocityProfile::AMTrapezoidVelocityProfile(double startPosition,
                                                       double endPosition,
                                                       double acceleration,
                                                       double time)
{
	startPosition_ = startPosition;
	endPosition_ = endPosition;
	acceleration_ = acceleration;
	time_ = time;
	isValid_ = true;
}

double AMTrapezoidVelocityProfile::startPosition() const
{
	return startPosition_;
}

double AMTrapezoidVelocityProfile::endPosition() const
{
	return endPosition_;
}

double AMTrapezoidVelocityProfile::acceleration() const
{
	return acceleration_;
}

double AMTrapezoidVelocityProfile::time() const
{
	return time_;
}

double AMTrapezoidVelocityProfile::targetVelocity() const
{
	double halfAccelByTime = (acceleration_*time_) / 2;
	double toRoot = pow(halfAccelByTime, 2) - acceleration_ * qAbs(endPosition_ - startPosition_);

	if(toRoot < 0) {
		return halfAccelByTime + sqrt(qAbs(toRoot));
	} else {
		return halfAccelByTime - sqrt(toRoot);
	}
}

bool AMTrapezoidVelocityProfile::isValid() const
{
	return isValid_;
}

double AMTrapezoidVelocityProfile::timeForVelocity(double startPosition,
                                                   double endPosition,
                                                   double acceleration,
                                                   double targetVelocity)
{

	double distance = qAbs(endPosition - startPosition);

	return (pow(targetVelocity, 2) + acceleration*distance) / (acceleration * targetVelocity);
}




