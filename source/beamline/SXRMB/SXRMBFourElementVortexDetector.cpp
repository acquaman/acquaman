#include "SXRMBFourElementVortexDetector.h"

SXRMBFourElementVortexDetector::SXRMBFourElementVortexDetector(const QString &name, const QString &description, QObject *parent)
	: AMXRFDetector(name, description, parent)
{
	units_ = "Counts";

	AMAxisInfo ai("Energy", 2048, "Energy", "eV");
	ai.start = AMNumber(0);
	ai.increment = 10;
	ai.isUniform = true;
	axes_ << ai;

	// Stuff required by AMXRFDetector.
	acquireControl_ = new AMPVControl("Acquisition Time", "dxp1606-B10-02:EraseStart", "dxp1606-B10-02:EraseStart", "dxp1606-B10-02:StopAll", this, 0.5);
	acquisitionStatusControl_ = new AMReadOnlyPVControl("Status", "dxp1606-B10-02:Acquiring", this);
	acquireTimeControl_ = new AMSinglePVControl("Integration Time", "dxp1606-B10-02:PresetReal", this, 0.001);
	elapsedTimeControl_ = new AMReadOnlyPVControl("Elapsed Time", "dxp1606-B10-02:ElapsedReal", this);

	for (int i = 0; i < 4; i++){

		icrControls_.append(new AMReadOnlyPVControl(QString("Input Counts %1").arg(i+1), QString("dxp1606-B10-02:dxp%1.ICR").arg(i+1), this, QString("The input counts for element %1 of the four element.").arg(i+1)));
		ocrControls_.append(new AMReadOnlyPVControl(QString("Output Counts %1").arg(i+1), QString("dxp1606-B10-02:dxp%1.OCR").arg(i+1), this, QString("The output counts for element %1 of the four element.").arg(i+1)));
		spectraControls_.append(new AMReadOnlyPVControl(QString("Raw Spectrum %1").arg(i+1), QString("dxp1606-B10-02:mca%1").arg(i+1), this));
	}

	allControlsCreated();

	// Our own stuff.
	maximumEnergyControl_ = new AMSinglePVControl("Maximum Energy", "dxp1606-B10-02:mcaEMax", this, 0.01);
	peakingTimeControl_ = new AMSinglePVControl("Peaking Time", "dxp1606-B10-02:EnergyPkTime", this, 0.01);

	connect(maximumEnergyControl_, SIGNAL(valueChanged(double)), this, SLOT(onMaximumEnergyChanged(double)));
	connect(peakingTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(peakingTimeChanged(double)));
}

SXRMBFourElementVortexDetector::~SXRMBFourElementVortexDetector()
{

}

QString SXRMBFourElementVortexDetector::synchronizedDwellKey() const
{
	return "dxp1606-B10-02:EraseStart NPP NMS";
}

bool SXRMBFourElementVortexDetector::lastContinuousReading(double *outputValues) const
{
	Q_UNUSED(outputValues)

	return false;
}

bool SXRMBFourElementVortexDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode)
{
	Q_UNUSED(readMode)

	return false;
}

void SXRMBFourElementVortexDetector::setMaximumEnergy(double energy)
{
	if (maximumEnergyControl_->value() != energy)
		maximumEnergyControl_->move(energy);
}

void SXRMBFourElementVortexDetector::setPeakingTime(double time)
{
	if (peakingTimeControl_->value() != time)
		peakingTimeControl_->move(time);
}

void SXRMBFourElementVortexDetector::onMaximumEnergyChanged(double newMaximum)
{
	double newScale = (newMaximum*1000)/axes_.at(0).size;
	axes_[0].increment = newScale;

	foreach (AMDataSource *source, rawSpectraSources_)
		((AM1DProcessVariableDataSource *)source)->setScale(newScale);

	emit maximumEnergyChanged(newMaximum);
}