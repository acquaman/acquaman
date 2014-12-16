#include "SXRMBBrukerDetector.h"

#include "beamline/AMBeamline.h"

SXRMBBrukerDetector::SXRMBBrukerDetector(const QString &name, const QString &description, QObject *parent)
	: AMXRFDetector(name, description, parent)
{
	units_ = "Counts";

	eVPerBin_ = 5.0;

	AMAxisInfo ai("Energy", 2048, "Energy", "eV");
	ai.start = AMNumber(-475);
	ai.increment = eVPerBin_;
	ai.isUniform = true;
	axes_ << ai;

	// Stuff required by AMXRFDetector.
	acquireControl_ = new AMPVControl("Acquisition Time", "mca1606-B10-03:mca1EraseStart", "mca1606-B10-03:mca1EraseStart", "mca1606-B10-03:mca1Stop", this, 0.5);
	acquisitionStatusControl_ = new AMReadOnlyPVControl("Status", "mca1606-B10-03:mca1.ACQG", this);
	acquireTimeControl_ = new AMSinglePVControl("Integration Time", "mca1606-B10-03:mca1.PRTM", this, 0.001);
	elapsedTimeControl_ = new AMReadOnlyPVControl("Elapsed Time", "mca1606-B10-03:mca1.ERTM", this);

//	icrControls_.append(new AMReadOnlyPVControl("Input Counts", "IOC1607-004:dxp1.ICR", this, "The input counts for the single element."));
//	ocrControls_.append(new AMReadOnlyPVControl("Output Counts", "IOC1607-004:dxp1.OCR", this, "The output counts for the single element."));
	spectraControls_.append(new AMReadOnlyPVControl("Raw Spectrum", "mca1606-B10-03:mca1", this));

	allControlsCreated();

	((AM1DProcessVariableDataSource *)rawSpectraSources_.first())->setScale(ai.increment);
	((AM1DProcessVariableDataSource *)rawSpectraSources_.first())->setOffset(ai.start);

	// Stuff specialized by SXRMBBrukerDetector
	deadTimeControl_ = new AMReadOnlyPVControl("Dead Time", "mca1606-B10-03:mca1.DTIM", this);
	connect(deadTimeControl_, SIGNAL(connected(bool)), this, SLOT(onDeadTimeControlConnected(bool)));
	connect(deadTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(deadTimeChanged()));
	if (deadTimeControl_->isConnected())
		emit deadTimeChanged();

}

SXRMBBrukerDetector::~SXRMBBrukerDetector()
{

}

double SXRMBBrukerDetector::deadTime() const
{
	return deadTimeControl_->value();
}

double SXRMBBrukerDetector::deadTimeAt(int index) const
{
	Q_UNUSED(index)

	return deadTime();
}

QString SXRMBBrukerDetector::synchronizedDwellKey() const
{
	return "mca1606-B10-03:mca1EraseStart NPP NMS";
}

bool SXRMBBrukerDetector::lastContinuousReading(double *outputValues) const
{
	Q_UNUSED(outputValues)

	return false;
}

bool SXRMBBrukerDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode)
{
	Q_UNUSED(readMode)

	return false;
}

void SXRMBBrukerDetector::onDeadTimeControlConnected(bool connected)
{
	if (connected)
		emit deadTimeChanged();
}
