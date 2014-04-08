#include "AMScanAxisEXAFSRegion.h"

#include "util/AMEnergyToKSpaceCalculator.h"

#include <math.h>

AMScanAxisEXAFSRegion::AMScanAxisEXAFSRegion(bool inKSpace, const AMNumber &edgeEnergy, const AMNumber &start, const AMNumber &step, const AMNumber &end, const AMNumber &time, const AMNumber &maximumTime, QObject *parent)
	: AMScanAxisRegion(start, step, end, time, parent)
{
	inKSpace_ = inKSpace;
	edgeEnergy_ = edgeEnergy;
	maximumTime_ = maximumTime;
}

AMScanAxisEXAFSRegion::AMScanAxisEXAFSRegion(const AMNumber &start, const AMNumber &step, const AMNumber &end, const AMNumber &time, QObject *parent)
	: AMScanAxisRegion(start, step, end, time, parent)
{
	inKSpace_ = false;
	edgeEnergy_ = AMNumber(AMNumber::Null);
	maximumTime_ = AMNumber(AMNumber::Null);
}

AMScanAxisEXAFSRegion::AMScanAxisEXAFSRegion(const AMScanAxisEXAFSRegion &original)
	: AMScanAxisRegion(original)
{
	inKSpace_ = original.inKSpace();
	edgeEnergy_ = original.edgeEnergy();
	maximumTime_ = original.maximumTime();
}

void AMScanAxisEXAFSRegion::setInKSpace(bool flag)
{
	emit inKSpaceChanged(inKSpace_ = flag);
	switchSpace();
}

void AMScanAxisEXAFSRegion::setEdgeEnergy(const AMNumber &energy)
{
	emit edgeEnergyChanged(edgeEnergy_ = energy);
}

void AMScanAxisEXAFSRegion::setMaximumTime(const AMNumber &newMaximumTime)
{
	emit maximumTimeChanged(newMaximumTime);
}

void AMScanAxisEXAFSRegion::switchSpace()
{
	// E = E0 + a*k^2 ; a = 3.810 945 497 eV * Angstrom
	if (inKSpace_ && edgeEnergy_.isValid()){

		setRegionStart(AMEnergyToKSpaceCalculator::k(edgeEnergy_, regionStart_));
		setRegionStep(AMEnergyToKSpaceCalculator::k(edgeEnergy_, regionStep_));
		setRegionEnd(AMEnergyToKSpaceCalculator::k(edgeEnergy_, regionEnd_));
	}

	else if (edgeEnergy_.isValid()){

		setRegionStart(AMEnergyToKSpaceCalculator::energy(edgeEnergy_, regionStart_));
		setRegionStep(AMEnergyToKSpaceCalculator::energy(edgeEnergy_, regionStep_));
		setRegionEnd(AMEnergyToKSpaceCalculator::energy(edgeEnergy_, regionEnd_));
	}
}
