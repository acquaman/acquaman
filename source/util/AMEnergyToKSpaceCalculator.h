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
	/// Sets all the k values fromm the start to the end in the given steps.  Requires a valid double pointer to fill the data into.
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
