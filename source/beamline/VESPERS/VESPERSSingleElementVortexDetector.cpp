#include "VESPERSSingleElementVortexDetector.h"

#include "beamline/AMBeamline.h"

VESPERSSingleElementVortexDetector::~VESPERSSingleElementVortexDetector(){}

VESPERSSingleElementVortexDetector::VESPERSSingleElementVortexDetector(const QString &name, const QString &description, QObject *parent)
	: AMXRFDetector(name, description, parent)
{
	units_ = "Counts";

	AMAxisInfo ai("Energy", 2048, "Energy", "eV");
	ai.start = AMNumber(0);
	ai.increment = 10;
	ai.isUniform = true;
	axes_ << ai;

	// Stuff required by AMXRFDetector.
	acquireControl_ = new AMPVControl("Acquisition Time", "IOC1607-004:mca1EraseStart", "IOC1607-004:mca1EraseStart", "IOC1607-004:mca1Stop", this, 0.5);
	acquisitionStatusControl_ = new AMReadOnlyPVControl("Status", "IOC1607-004:mca1.ACQG", this);
	acquireTimeControl_ = new AMSinglePVControl("Integration Time", "IOC1607-004:mca1.PRTM", this, 0.001);
	elapsedTimeControl_ = new AMReadOnlyPVControl("Elapsed Time", "IOC1607-004:mca1.ERTM", this);

	icrControls_.append(new AMReadOnlyPVControl("Input Counts", "IOC1607-004:dxp1.ICR", this, "The input counts for the single element."));
	ocrControls_.append(new AMReadOnlyPVControl("Output Counts", "IOC1607-004:dxp1.OCR", this, "The output counts for the single element."));
	spectraControls_.append(new AMReadOnlyPVControl("Raw Spectrum", "IOC1607-004:mca1", this));

	allControlsCreated();

	// Our own stuff.
	maximumEnergyControl_ = new AMSinglePVControl("Maximum Energy", "IOC1607-004:dxp1.EMAX", this, 0.01);
	peakingTimeControl_ = new AMSinglePVControl("Peaking Time", "IOC1607-004:dxp1.PKTIM", this, 0.01);

	connect(maximumEnergyControl_, SIGNAL(valueChanged(double)), this, SLOT(onMaximumEnergyChanged(double)));
	connect(peakingTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(peakingTimeChanged(double)));
}

QString VESPERSSingleElementVortexDetector::synchronizedDwellKey() const
{
	return "IOC1607-004:mca1EraseStart NPP NMS";
}

bool VESPERSSingleElementVortexDetector::sharesDetectorTriggerSource()
{
	return currentlySynchronizedDwell();
}

AMDetectorTriggerSource* VESPERSSingleElementVortexDetector::detectorTriggerSource()
{
	if(currentlySynchronizedDwell())
		return AMBeamline::bl()->synchronizedDwellTime()->triggerSource();

	return 0;
}

AMDetectorDwellTimeSource* VESPERSSingleElementVortexDetector::detectorDwellTimeSource()
{
	if(currentlySynchronizedDwell())
		return AMBeamline::bl()->synchronizedDwellTime()->dwellTimeSource();

	return 0;
}

bool VESPERSSingleElementVortexDetector::lastContinuousReading(double *outputValues) const
{
	Q_UNUSED(outputValues)

	return false;
}

bool VESPERSSingleElementVortexDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode)
{
	Q_UNUSED(readMode)

	return false;
}

void VESPERSSingleElementVortexDetector::setMaximumEnergy(double energy)
{
	if (maximumEnergyControl_->value() != energy)
		maximumEnergyControl_->move(energy);
}

void VESPERSSingleElementVortexDetector::setPeakingTime(double time)
{
	if (peakingTimeControl_->value() != time)
		peakingTimeControl_->move(time);
}

void VESPERSSingleElementVortexDetector::onMaximumEnergyChanged(double newMaximum)
{
	double newScale = (newMaximum*1000)/axes_.at(0).size;
	axes_[0].increment = newScale;

	foreach (AMDataSource *source, rawSpectraSources_)
		((AM1DProcessVariableDataSource *)source)->setScale(newScale);

	emit maximumEnergyChanged(newMaximum);
}
