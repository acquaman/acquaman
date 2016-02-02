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


#ifndef AMSCANAXISEXAFSREGION_H
#define AMSCANAXISEXAFSREGION_H

#include "dataman/AMScanAxisRegion.h"
#include "util/AMVariableIntegrationTime.h"

/// This class adds some of the necessities for doing EXAFS over standard energy scans.  Stores the key information that would be useful in the AMVariableIntegrationTime and AMEnergyToKSpaceCalculator.
class AMScanAxisEXAFSRegion : public AMScanAxisRegion
{
	Q_OBJECT

	Q_PROPERTY(bool inKSpace READ inKSpace WRITE setInKSpace)
	Q_PROPERTY(double edgeEnergy READ edgeEnergy WRITE setEdgeEnergy)
	Q_PROPERTY(double maximumTime READ maximumTime WRITE setMaximumTime)
	Q_PROPERTY(double a2 READ a2 WRITE setA2)
	Q_PROPERTY(int equation READ equation WRITE setEquation)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Specific EXAFS region for EXAFS scan axes.")

public:
	/// Constructor.  Allows for maximum customization.  Extends the normal region by having a flag for knowing whether the region is in k-space or not.
	Q_INVOKABLE AMScanAxisEXAFSRegion(const AMNumber &start = AMNumber(AMNumber::Null),
									  const AMNumber &step = AMNumber(AMNumber::Null),
									  const AMNumber &end = AMNumber(AMNumber::Null),
									  const AMNumber &time = AMNumber(AMNumber::Null),
									  bool inKSpace = false,
									  const AMNumber &edgeEnergy = AMNumber(AMNumber::Null),
									  const AMNumber &maximumTime = AMNumber(AMNumber::Null),
									  const AMNumber &a2 = AMNumber(2.0),
									  AMVariableIntegrationTime::Equation equation = AMVariableIntegrationTime::Geometric,
									  QObject *parent = 0);
	/// Copy constructor.
	AMScanAxisEXAFSRegion(const AMScanAxisEXAFSRegion &original);
	/// Destructor.
	virtual ~AMScanAxisEXAFSRegion(){}

	/// Method that creates an exact copy of the current object.  Caller is responisible for memory.
	virtual AMScanAxisRegion *createCopy() const;

	/// Returns the amount of time the region would take using the parameters it has.
	virtual double timePerRegion() const;
	/// Returns a string containing the information in a standard way.
	virtual QString toString(const QString &units = "") const;

	/// Returns whether this region is in k-space.
	bool inKSpace() const { return inKSpace_; }
	/// Returns the edge energy associated with this region.
	AMNumber edgeEnergy() const { return edgeEnergy_; }
	/// Returns the maximum time for the region.
	AMNumber maximumTime() const { return maximumTime_; }
	/// Returns the a2 coefficient.
	AMNumber a2() const { return a2_; }
	/// Returns the equation used for variable integration times.
	AMVariableIntegrationTime::Equation equation() const { return equation_; }

signals:
	/// Notifier that the flag has changed.
	void inKSpaceChanged(bool);
	/// Notifier that the edge energy changed.
	void edgeEnergyChanged(const AMNumber &);
	/// Notifier that the maximum time changed.
	void maximumTimeChanged(const AMNumber &);
	/// Notifier that the a2 coefficient has changed.
	void a2Changed(const AMNumber &);
	/// Notifier that the equation has changed.
	void equationChanged(AMVariableIntegrationTime::Equation);

public slots:
	/// Sets the flag for whether this is a normal region or an extended region.
	void setInKSpace(bool flag);
	/// Sets the edge energy.
	void setEdgeEnergy(const AMNumber &energy);
	/// Sets the maximum time.
	void setMaximumTime(const AMNumber &newMaximumTime);
	/// Sets the a2 coefficient.
	void setA2(const AMNumber &newA2);
	/// Sets the equation used for variable integration time calculations.
	void setEquation(AMVariableIntegrationTime::Equation newEquation);
	/// Overloaded for database loading.
	void setEquation(int newEquation);

protected:

	/// Flag that holds whether this region is in k-space or not.
	bool inKSpace_;
	/// Flag holding the edge energy.
	AMNumber edgeEnergy_;
	/// Flag holding the maximum time.  Used for variable integration time calculations.
	AMNumber maximumTime_;
	/// The parameter used for higher order equations in variable integration time calculations.
	AMNumber a2_;
	/// The equation used in variable integration time calculations.
	AMVariableIntegrationTime::Equation equation_;

private:
	/// Switches the start, step, and end position based on what the current state of the inKSpace flag is set to.  Assumes all values are appropriate.
	void switchSpace();
};

#endif // AMSCANAXISEXAFSREGION_H
