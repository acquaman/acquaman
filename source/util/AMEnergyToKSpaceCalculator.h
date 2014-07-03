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


#ifndef AMENERGYTOKSPACECALCULATOR_H
#define AMENERGYTOKSPACECALCULATOR_H

#include "dataman/AMNumber.h"

/// This class allows seamless conversion between eV and k-space.
class AMEnergyToKSpaceCalculator
{
public:
	/// Constructor.  Builds a calculator.
	AMEnergyToKSpaceCalculator();
	/// Constructor.  Builds the calculator with a value given for the edge energy.
	AMEnergyToKSpaceCalculator(const AMNumber &edgeEnergy);
	/// Destructor.
	virtual ~AMEnergyToKSpaceCalculator() {}

	/// Returns the edge energy used for the calculation.
	AMNumber edgeEnergy() const { return edgeEnergy_; }
	/// Sets the edge energy.
	void setEdgeEnergy(const AMNumber &edge);

	/// Returns the energy associated with a given k-value.
	AMNumber energy(const AMNumber &k) const;
	/// Sets all the energy values from the start to the end in the given steps.  Requires a vaild double pointer to fill the data into.
	bool energyValues(const AMNumber &start, const AMNumber &step, const AMNumber &end, double *energyValues) const;

	/// Returns the k-value associated with the given energy.
	AMNumber k(const AMNumber &energy) const;
	/// Sets all the k values from the start to the end in the given steps.  Requires a valid double pointer to fill the data into.
	bool kValues(const AMNumber &start, const AMNumber &step, const AMNumber &end, double *kValues) const;

	/// Static method.  Provide an edge energy and a given k value and it will return the energy associated with it.
	static AMNumber energy(const AMNumber &edgeEnergy, const AMNumber &k);
	/// Static method.  Provide an edge energy and a given energy value and it will return the energy associated with it.
	static AMNumber k(const AMNumber &edgeEnergy, const AMNumber &energy);

protected:
	/// The edge energy.
	AMNumber edgeEnergy_;
};

#endif // AMENERGYTOKSPACECALCULATOR_H
