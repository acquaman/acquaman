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


#include "AMEnergyToKSpaceCalculator.h"

#include <math.h>

AMEnergyToKSpaceCalculator::AMEnergyToKSpaceCalculator()
{
	edgeEnergy_ = AMNumber();
}

AMEnergyToKSpaceCalculator::AMEnergyToKSpaceCalculator(const AMNumber &edgeEnergy)
{
	edgeEnergy_ = edgeEnergy;
}

void AMEnergyToKSpaceCalculator::setEdgeEnergy(const AMNumber &edge)
{
	edgeEnergy_ = edge;
}

AMNumber AMEnergyToKSpaceCalculator::k(const AMNumber &energy) const
{
	if (edgeEnergy_.isValid() && double(energy) > double(edgeEnergy_))
		return AMNumber(sqrt((double(energy)-double(edgeEnergy_))/3.810945497));

	else if (edgeEnergy_.isValid())
		return AMNumber(0.0);

	return AMNumber();
}

AMNumber AMEnergyToKSpaceCalculator::k(const AMNumber &edgeEnergy, const AMNumber &energy)
{
	if (edgeEnergy.isValid() && energy.isValid() && double(energy) > double(edgeEnergy))
		return AMNumber(sqrt((double(energy)-double(edgeEnergy))/3.810945497));

	else if (edgeEnergy.isValid() && energy.isValid())
		return AMNumber(0.0);

	return AMNumber();
}

bool AMEnergyToKSpaceCalculator::kValues(const AMNumber &start, const AMNumber &step, const AMNumber &end, double *kValues) const
{
	if (edgeEnergy_.isValid() && double(start) < double(end)){

		double edge = double(edgeEnergy_);
		double energyStart = double(start);
		double energyStep = double(step);
		double energyEnd = double(end);
		int points = int(round((energyEnd-energyStart)/energyStep));

		for (int i = 0; i < points; i++){

			double energy = energyStart + i*energyStep;

			if (energy > edge)
				kValues[i] = sqrt(((energyStart + i*energyStep) - edge)/3.810945497);

			else
				kValues[i] = 0.0;
		}

		return true;
	}

	return false;
}

AMNumber AMEnergyToKSpaceCalculator::energy(const AMNumber &k) const
{
	if (edgeEnergy_.isValid())
		return AMNumber(double(edgeEnergy_) + 3.810945497*double(k)*double(k));

	return AMNumber();
}

AMNumber AMEnergyToKSpaceCalculator::energy(const AMNumber &edgeEnergy, const AMNumber &k)
{
	if (edgeEnergy.isValid() && k.isValid())
		return AMNumber(double(edgeEnergy) + 3.810945497*double(k)*double(k));

	return AMNumber();
}

bool AMEnergyToKSpaceCalculator::energyValues(const AMNumber &start, const AMNumber &step, const AMNumber &end, double *energyValues) const
{
	if (edgeEnergy_.isValid() && double(start) < double(end)){

		double edge = double(edgeEnergy_);
		double kStart = double(start);
		double kStep = double(step);
		double kEnd = double(end);
		int points = int(round((kEnd-kStart)/kStep));

		for (int i = 0; i < points; i++){

			double kSquared = (kStart + kStep*i) * (kStart + kStep*i);
			energyValues[i] = edge + 3.810945497*kSquared;
		}

		return true;
	}

	return false;
}
