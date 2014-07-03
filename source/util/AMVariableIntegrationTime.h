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


#ifndef AMVARIABLEINTEGRATIONTIME_H
#define AMVARIABLEINTEGRATIONTIME_H

#include <QMap>
#include <QString>

/// This class computes the appropriate dwell time, given an equation and min/max values.
class AMVariableIntegrationTime
{
public:

	/// Equation enum.  Choice of equation dictates resulting time values for given k.
	enum Equation
	{
		Constant,
		Linear,
		Quadratic,
		Geometric,
		Exponential,
		Logarithmic,
		SmoothStep
	};

	/// Constructor.  Builds the calculator.
	AMVariableIntegrationTime();
	/// Constructor.  Builds the calculator with the given equation.
	AMVariableIntegrationTime(Equation equation);
	/// Constructor.  Builds the calculator and fully configures it for single values.
	AMVariableIntegrationTime(Equation equation, double t0, double tf, double k0, double kf, double a2);
	/// Constructor.  Builds the calculator and fully configures it.
	AMVariableIntegrationTime(Equation equation, double t0, double tf, double k0, double kStep, double kf, double a2);
	/// Destructor.
	virtual ~AMVariableIntegrationTime() {}

	/// Returns the current equation.
	Equation equation() const { return equation_; }
	/// Returns the current equation as a string.
	QString equationString() const;
	/// Returns the intial time.
	double t0() const { return t0_; }
	/// Returns the final time.
	double tf() const { return tf_; }
	/// Returns the initial k value.
	double k0() const { return k0_; }
	/// Returns the step amount in k.
	double kStep() const { return kStep_; }
	/// Returns the final k value.
	double kf() const { return kf_; }
	/// Returns the current a0 coefficient.  0 if unable to compute.
	double a0() const { return a0_; }
	/// Returns the current a1 coefficient.  0 if unable to compute.
	double a1() const { return a1_; }
	/// Returns the current a2 coefficient.
	double a2() const { return a2_; }

	/// Returns the size of the current k-time array.
	int size() const { return variableTimes_.size(); }

	/// Sets the equation.
	void setEquation(Equation equation);
	/// Sets the initial time.
	void setT0(double t0);
	/// Sets the final time.
	void setTf(double tf);
	/// Sets the initial k value.
	void setK0(double k0);
	/// Sets the step amount in k.
	void setKStep(double kStep);
	/// Sets the final k value.
	void setKf(double kf);
	/// Sets the a2 coefficient.
	void setA2(double a2);

	/// Returns a time for given k value.  Returns -1.0 if not configured.
	double variableTimeAt(double k) const;
	/// Sets all the times to the given double pointer of capacity size().  Returns false if array was unable to complete.
	bool variableTime(double *times) const;

protected:
	/// Computes the two coefficients based on the current configuration.
	bool computeCoefficients() const;

	/// The equation being used.
	Equation equation_;
	/// The initial time.
	double t0_;
	/// The final time.
	double tf_;
	/// The initial k value.
	double k0_;
	/// The step size between k0 and kf.
	double kStep_;
	/// The final k value.
	double kf_;
	/// The first coefficient.
	mutable double a0_;
	/// The second coefficient.
	mutable double a1_;
	/// The third coefficient (not computed).
	double a2_;

	/// The actual data.
	mutable QMap<double, double> variableTimes_;
	/// Flag used to know if recomputation is necessary.
	mutable bool isValid_;
};

#endif // AMVARIABLEINTEGRATIONTIME_H
