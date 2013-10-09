#include "VESPERSSingleElementVortexDetector.h"

VESPERSSingleElementVortexDetector::VESPERSSingleElementVortexDetector(const QString &name, const QString &description, QObject *parent)
	: AMXRFDetector(name, description, parent)
{
	units_ = "Counts";

	// NOTE! This can't be done in the constructor because it depends on the maximum energy.
	axes_ = QList<AMAxisInfo>();
	AMAxisInfo ai("Energy", 2048, "Energy", "eV");
	ai.start = AMNumber(0);
	ai.increment = 10;
	ai.isUniform = true;
	axes_ << ai;

	acquireControl_ = new AMPVControl("AcquisitionTime", "IOC1607-004:mca1EraseStart", "IOC1607-004:mca1EraseStart", "IOC1607-004:mca1Stop", this, 0.5);
	acquisitionStatusControl_ = new AMReadOnlyPVControl("Status", "IOC1607-004:mca1.ACQG", this);
	acquireTimeControl_ = new AMPVControl("IntegrationTime", "IOC1607-004:mca1.PRTM", "IOC1607-004:mca1.PRTM", QString() , this, 0.05);
	elapsedTimeControl_ = new AMReadOnlyPVControl("ElapsedTime", "IOC1607-004:mca1.ERTM", this);

	AMReadOnlyPVControl *inputCountsControl = new AMReadOnlyPVControl("Input Counts", "IOC1607-004:dxp1.ICR", this, "The input counts for the single element.");
	icrControls_.append(inputCountsControl);
	AMReadOnlyPVControl *outputCountsControl = new AMReadOnlyPVControl("Output Counts", "IOC1607-004:dxp1.OCR", this, "The output counts for the single element.");
	ocrControls_.append(outputCountsControl);
	AMReadOnlyPVControl *spectrumControl = new AMReadOnlyPVControl("Raw Spectrum", "IOC1607-004:mca1", this);
	spectraControls_.append(spectrumControl);

	allControlsCreated();
}

QString VESPERSSingleElementVortexDetector::synchronizedDwellKey() const
{
	return "IOC1607-004:mca1EraseStart";
}

bool VESPERSSingleElementVortexDetector::sharesDetectorTriggerSource()
{
	return currentlySynchronizedDwell();
}

#include "beamline/AMBeamline.h"
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

const double *VESPERSSingleElementVortexDetector::data() const
{
	return 0;
}
