#include "SXRMBBrukerDetector.h"

#include "beamline/AMBeamline.h"

SXRMBBrukerDetector::SXRMBBrukerDetector(const QString &name, const QString &description, QObject *parent)
	: AMXRFDetector(name, description, parent)
{
	units_ = "Counts";

	AMAxisInfo ai("Energy", 2048, "Energy", "eV");
	ai.start = AMNumber(-450);
	ai.increment = 5.0;
	ai.isUniform = true;
	axes_ << ai;

	// Stuff required by AMXRFDetector.
	acquireControl_ = new AMPVControl("Acquisition Time", "mca1606-B10-03:mca1EraseStart", "mca1606-B10-03:mca1EraseStart", "mca1606-B10-03:mca1Stop", this, 0.5);
	acquisitionStatusControl_ = new AMReadOnlyPVControl("Status", "mca1606-B10-03:mca1.ACQG", this);
	acquireTimeControl_ = new AMSinglePVControl("Integration Time", "mca1606-B10-03:mca1.PRTM", this, 0.001);
	elapsedTimeControl_ = new AMReadOnlyPVControl("Elapsed Time", "mca1606-B10-03:mca1.ERTM", this);

	spectraControls_.append(new AMReadOnlyPVControl("Raw Spectrum", "mca1606-B10-03:mca1", this));

	allControlsCreated();

	((AM1DProcessVariableDataSource *)rawSpectraSources_.first())->setScale(ai.increment);
	((AM1DProcessVariableDataSource *)rawSpectraSources_.first())->setOffset(ai.start);

	// Stuff specialized by SXRMBBrukerDetector
	deadTimeControl_ = new AMReadOnlyPVControl("Dead Time", "mca1606-B10-03:mca1.DTIM", this);

	allControls_->addControl(deadTimeControl_);
	connect(deadTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(deadTimeChanged()));
	connect(this, SIGNAL(connected(bool)), this, SLOT(onDeadTimeControlConnected(bool)));
}

SXRMBBrukerDetector::~SXRMBBrukerDetector()
{

}

double SXRMBBrukerDetector::deadTime() const
{
	return deadTimeControl_->value()/100.0;
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
