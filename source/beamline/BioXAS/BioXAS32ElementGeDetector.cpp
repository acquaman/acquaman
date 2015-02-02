#include "BioXAS32ElementGeDetector.h"

BioXAS32ElementGeDetector::BioXAS32ElementGeDetector(const QString &name, const QString &description, QObject *parent)
	: AMXRFDetector(name, description, parent)
{
	units_ = "Counts";

	AMAxisInfo ai("Energy", 2048, "Energy", "eV");
	ai.start = AMNumber(0);
	ai.increment = 1;
	ai.isUniform = true;
	axes_ << ai;

	// Stuff required by AMXRFDetector.
	acquireControl_ = new AMSinglePVControl("Acquisition Time", "BL1607-5-I22:SetPort_1", this, 0.5);
	acquisitionStatusControl_ = new AMReadOnlyPVControl("Status", "DXP1607-I22-01:DetectorState_RBV", this);
	acquireTimeControl_ = new AMSinglePVControl("Integration Time", "BL1607-5-I22:SetPort_1.HIGH", this, 0.001);
//	elapsedTimeControl_ = new AMReadOnlyPVControl("Elapsed Time", "dxp1607-B21-04:ElapsedReal", this);

	for (int i = 0; i < 1; i++){

		spectraControls_.append(new AMReadOnlyPVControl(QString("Raw Spectrum %1").arg(i+1), QString("DXP1607-I22-01:ARR%1:ArrayData").arg(i+1), this));
	}

	allControlsCreated();

	disconnect(acquisitionStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onStatusControlChanged()));
	connect(acquireControl_, SIGNAL(valueChanged(double)), this, SLOT(updateAcquisitionState()));
	connect(acquisitionStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(updateAcquisitionState()));
}

BioXAS32ElementGeDetector::~BioXAS32ElementGeDetector()
{

}

QString BioXAS32ElementGeDetector::synchronizedDwellKey() const
{
	return "dxp1607-B21-04:EraseStart NPP NMS";
}

bool BioXAS32ElementGeDetector::lastContinuousReading(double *outputValues) const
{
	Q_UNUSED(outputValues)

	return false;
}

bool BioXAS32ElementGeDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode)
{
	Q_UNUSED(readMode)

	return false;
}

void BioXAS32ElementGeDetector::updateAcquisitionState()
{
	if (isAcquisitionSucceeded() && (acquisitionStatusControl_->withinTolerance(1) && acquireControl_->withinTolerance(0)))
		setReadyForAcquisition();

	else if (acquisitionStatusControl_->withinTolerance(1) && acquireControl_->withinTolerance(1))
		setAcquiring();

	else if (acquisitionStatusControl_->withinTolerance(2) && acquireControl_->withinTolerance(0))
		setAcquisitionSucceeded();

	else if (acquisitionStatusControl_->withinTolerance(0) || acquisitionStatusControl_->withinTolerance(5))
		setNotReadyForAcquisition();
}
