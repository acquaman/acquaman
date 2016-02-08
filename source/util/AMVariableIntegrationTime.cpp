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


#include "AMVariableIntegrationTime.h"

#include <math.h>
#include <QVector>

AMVariableIntegrationTime::AMVariableIntegrationTime()
{
	equation_ = Constant;
	t0_ = 0.0;
	tf_ = 0.0;
	k0_ = 0.0;
	kStep_ = 0.0;
	kf_ = 0.0;
	a0_ = 0.0;
	a1_ = 0.0;
	a2_ = 0.0;
	isValid_ = false;
}

AMVariableIntegrationTime::AMVariableIntegrationTime(Equation equation)
{
	equation_ = equation;
	t0_ = 0.0;
	tf_ = 0.0;
	k0_ = 0.0;
	kStep_ = 0.0;
	kf_ = 0.0;
	a0_ = 0.0;
	a1_ = 0.0;
	a2_ = 0.0;
	isValid_ = false;
}

AMVariableIntegrationTime::AMVariableIntegrationTime(Equation equation, double t0, double tf, double k0, double kf, double a2)
{
	equation_ = equation;
	t0_ = t0;
	tf_ = tf;
	k0_ = k0;
	kStep_ = 0.0;
	kf_ = kf;
	a0_ = 0.0;
	a1_ = 0.0;
	a2_ = a2;
	isValid_ = false;
	computeCoefficients();
}

AMVariableIntegrationTime::AMVariableIntegrationTime(Equation equation, double t0, double tf, double k0, double kStep, double kf, double a2)
{
	equation_ = equation;
	t0_ = t0;
	tf_ = tf;
	k0_ = k0;
	kStep_ = kStep;
	kf_ = kf;
	a0_ = 0.0;
	a1_ = 0.0;
	a2_ = a2;
	isValid_ = false;
	computeCoefficients();
}

void AMVariableIntegrationTime::setEquation(Equation equation)
{
	if (equation_ != equation){

		isValid_ = false;
		equation_ = equation;
	}
}

void AMVariableIntegrationTime::setT0(double t0)
{
	if (t0_ != t0){

		isValid_ = false;
		t0_ = t0;
	}
}

void AMVariableIntegrationTime::setTf(double tf)
{
	if (tf_ != tf){

		isValid_ = false;
		tf_ = tf;
	}
}

void AMVariableIntegrationTime::setK0(double k0)
{
	if (k0_ != k0){

		isValid_ = false;
		k0_ = k0;
	}
}

void AMVariableIntegrationTime::setKStep(double kStep)
{
	if (kStep_ != kStep){

		isValid_ = false;
		kStep_ = kStep;
	}
}

void AMVariableIntegrationTime::setKf(double kf)
{
	if (kf_ != kf){

		isValid_ = false;
		kf_ = kf;
	}
}

void AMVariableIntegrationTime::setA2(double a2)
{
	if (a2_ != a2){

		isValid_ = false;
		a2_ = a2;
	}
}

QString AMVariableIntegrationTime::equationString() const
{
	switch(equation_){

	case Constant:
		return "t = a0";

	case Linear:
		return "t = a0 + a1*k";

	case Quadratic:
		return "t = a0 + a1*k^2";

	case Geometric:
		return "t = a0 + a1*k^a2";

	case Exponential:
		return "t = a0 + a1*exp(a2*k)";

	case Logarithmic:
		return "t = a0 + a1*log10(|k + a2|)";

	case SmoothStep:
		return "t = a0 + a1/(1 + exp(-a2*(k - kmax/2)/2))";
	}

	return "";
}

double AMVariableIntegrationTime::variableTimeAt(double k) const
{
	if (!isValid_ && !computeCoefficients())
		return -1.0;

	double time = variableTimes_.value(k, -1.0);

	if (time == -1.0){

		switch(equation_){

		case Constant:
			time = t0_;
			break;

		case Linear:

			time = a0_ + a1_*k;
			break;

		case Quadratic:

			time = a0_ + a1_*k*k;
			break;

		case Geometric:

			time = a0_ + a1_*pow(k, a2_);
			break;

		case Exponential:

			time = a0_ + a1_*exp(a2_*k);
			break;

		case Logarithmic:

			time = a0_ + a1_*log10(fabs(k + a2_));
			break;

		case SmoothStep:

			time = a0_ + a1_/(1 + exp(-1.0*a2_*k));
			break;
		}
	}

	return time;
}

bool AMVariableIntegrationTime::variableTime(double *times) const
{
	// If all the data is good already, just copy it.
	if (isValid_ && !variableTimes_.isEmpty()){

		QVector<double> data = variableTimes_.values().toVector();
		memcpy(times, data.constData(), data.size()*sizeof(double));

		return true;
	}

	if (!isValid_ && !computeCoefficients())
		return false;

	if (kStep_ <= 0.0 || k0_ > kf_)
		return false;

	variableTimes_.clear();

	switch(equation_){

	case Constant:{

		int points = int(round((kf_-k0_)/kStep_));
		double k = k0_;

		for (int i = 0; i < points; i++){

			variableTimes_.insert(k, t0_);
			k += kStep_;
		}

		break;
	}

	case Linear:{

		int points = int(round((kf_-k0_)/kStep_));
		double k = k0_;

		for (int i = 0; i < points; i++){

			variableTimes_.insert(k, a0_ + a1_*k);
			k += kStep_;
		}

		break;
	}

	case Quadratic:{

		int points = int(round((kf_-k0_)/kStep_));
		double k = k0_;

		for (int i = 0; i < points; i++){

			variableTimes_.insert(k, a0_ + a1_*k*k);
			k += kStep_;
		}

		break;
	}

	case Geometric:{

		int points = int(round((kf_-k0_)/kStep_));
		double k = k0_;

		for (int i = 0; i < points; i++){

			variableTimes_.insert(k, a0_ + a1_*pow(k, a2_));
			k += kStep_;
		}

		break;
	}

	case Exponential:{

		int points = int(round((kf_-k0_)/kStep_));
		double k = k0_;

		for (int i = 0; i < points; i++){

			variableTimes_.insert(k, a0_ + a1_*exp(a2_*k));
			k += kStep_;
		}

		break;
	}

	case Logarithmic:{

		int points = int(round((kf_-k0_)/kStep_));
		double k = k0_;

		for (int i = 0; i < points; i++){

			variableTimes_.insert(k, a0_ + a1_*log10(fabs(k + a2_)));
			k += kStep_;
		}

		break;
	}

	case SmoothStep:{

		int points = int(round((kf_-k0_)/kStep_));
		double k = k0_;

		for (int i = 0; i < points; i++){

			variableTimes_.insert(k, a0_ + a1_/(1 + exp(-1.0*a2_*(k - kf_/2)/2)));
			k += kStep_;
		}

		break;
	}
	}

	QVector<double> data = variableTimes_.values().toVector();
	memcpy(times, data.constData(), data.size()*sizeof(double));

	return true;
}

bool AMVariableIntegrationTime::computeCoefficients() const
{
	bool configured = true;
	configured &= (t0_ >= 0 && tf_ >= t0_);
	configured &= (k0_ >= 0 && kf_ > k0_);

	if (configured){

		double k0Calculated = 0.0;
		double kfCalculated = 0.0;

		switch(equation_){

		case Constant:
			a0_ = t0_;
			a1_ = 0.0;
			break;

		case Linear:

			k0Calculated = k0_;
			kfCalculated = kf_;
			break;

		case Quadratic:

			k0Calculated = k0_*k0_;
			kfCalculated = kf_*kf_;
			break;

		case Geometric:

			k0Calculated = pow(k0_, a2_);
			kfCalculated = pow(kf_, a2_);
			break;

		case Exponential:

			k0Calculated = exp(a2_*k0_);
			kfCalculated = exp(a2_*kf_);
			break;

		case Logarithmic:

			k0Calculated = log10(fabs(k0_ + a2_));
			kfCalculated = log10(fabs(kf_ + a2_));
			break;

		case SmoothStep:

			k0Calculated = 1/(1 + exp(-1.0*a2_*(k0_ - kf_/2)/2));
			kfCalculated = 1/(1 + exp(-1.0*a2_*(kf_ - kf_/2)/2));
			break;
		}

		if (equation_ != Constant){

			a0_ = (t0_*kfCalculated - tf_*k0Calculated)/(kfCalculated - k0Calculated);
			a1_ = (-t0_ + tf_)/(kfCalculated - k0Calculated);
		}

		isValid_ = true;
		variableTimes_.clear();
	}

	return configured;
}
