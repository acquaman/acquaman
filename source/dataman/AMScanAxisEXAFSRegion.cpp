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
