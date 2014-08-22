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


#include "AMScanAxisEXAFSRegion.h"

#include "util/AMEnergyToKSpaceCalculator.h"

#include <math.h>

AMScanAxisEXAFSRegion::AMScanAxisEXAFSRegion(const AMNumber &start, const AMNumber &step, const AMNumber &end, const AMNumber &time, bool inKSpace, const AMNumber &edgeEnergy, const AMNumber &maximumTime, const AMNumber &a2, AMVariableIntegrationTime::Equation equation, QObject *parent)
	: AMScanAxisRegion(start, step, end, time, parent)
{
	inKSpace_ = inKSpace;
	edgeEnergy_ = edgeEnergy;
	maximumTime_ = maximumTime;
	a2_ = a2;
	equation_ = equation;
}

AMScanAxisEXAFSRegion::AMScanAxisEXAFSRegion(const AMScanAxisEXAFSRegion &original)
	: AMScanAxisRegion(original)
{
	inKSpace_ = original.inKSpace();
	edgeEnergy_ = original.edgeEnergy();
	maximumTime_ = original.maximumTime();
	a2_ = original.a2();
	equation_ = original.equation();
}

AMScanAxisRegion *AMScanAxisEXAFSRegion::createCopy() const
{
	return new AMScanAxisEXAFSRegion(*this);
}

void AMScanAxisEXAFSRegion::setInKSpace(bool flag)
{
	if (flag != inKSpace_){

		emit inKSpaceChanged(inKSpace_ = flag);
		setModified(true);
		switchSpace();
	}
}

void AMScanAxisEXAFSRegion::setEdgeEnergy(const AMNumber &energy)
{
	if (double(edgeEnergy_) != double(energy)){

		emit edgeEnergyChanged(edgeEnergy_ = energy);
		setModified(true);
	}
}

void AMScanAxisEXAFSRegion::setMaximumTime(const AMNumber &newMaximumTime)
{
	if (double(maximumTime_) != double(newMaximumTime)){

		emit maximumTimeChanged(maximumTime_ = newMaximumTime);
		setModified(true);
	}
}

void AMScanAxisEXAFSRegion::setA2(const AMNumber &newA2)
{
	if (double(a2_) != double(newA2)){

		emit a2Changed(a2_ = newA2);
		setModified(true);
	}
}

void AMScanAxisEXAFSRegion::setEquation(AMVariableIntegrationTime::Equation newEquation)
{
	if (double(equation_) != double(newEquation)){

		emit equationChanged(equation_ = newEquation);
		setModified(true);
	}
}

void AMScanAxisEXAFSRegion::setEquation(int newEquation)
{
	setEquation(AMVariableIntegrationTime::Equation(newEquation));
}

void AMScanAxisEXAFSRegion::switchSpace()
{
	// E = E0 + a*k^2 ; a = 3.810 945 497 eV * Angstrom
	if (inKSpace_ && edgeEnergy_.isValid()){

		setRegionStart(AMEnergyToKSpaceCalculator::k(edgeEnergy_, regionStart_));
		setRegionEnd(AMEnergyToKSpaceCalculator::k(edgeEnergy_, regionEnd_));
	}

	else if (edgeEnergy_.isValid()){

		setRegionStart(AMEnergyToKSpaceCalculator::energy(edgeEnergy_, regionStart_));
		setRegionEnd(AMEnergyToKSpaceCalculator::energy(edgeEnergy_, regionEnd_));
	}
}
