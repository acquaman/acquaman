#include "SXRMBFourElementVortexDetector.h"

#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

SXRMBFourElementVortexDetector::SXRMBFourElementVortexDetector(const QString &name, const QString &description, QObject *parent)
	: AMXRFDetector(name, description, parent)
{
	dataReady_ = false;
	dataReadyCounter_ = -1;

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

	mcaRatePVControl_ = new AMSinglePVControl("MCA Rate control", "dxp1606-B10-02:ReadAll.SCAN", this, 0.5);
	dxpRatePVControl_ = new AMSinglePVControl("DXP Rate control", "dxp1606-B10-02:ReadDXPs.SCAN", this, 0.5);
	statusRatePVControl_ = new AMSinglePVControl("Status Rate control", "dxp1606-B10-02:StatusAll.SCAN", this, 0.5);

	allControls_->addControl(maximumEnergyControl_);
	allControls_->addControl(peakingTimeControl_);
	allControls_->addControl(mcaRatePVControl_);
	allControls_->addControl(dxpRatePVControl_);
	allControls_->addControl(statusRatePVControl_);

	connect(maximumEnergyControl_, SIGNAL(valueChanged(double)), this, SLOT(onMaximumEnergyChanged(double)));
	connect(peakingTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(peakingTimeChanged(double)));

	disconnect(acquisitionStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onStatusControlChanged()));
	connect(acquisitionStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(updateAcquisitionState()));
	connect(acquireControl_, SIGNAL(valueChanged(double)), this, SLOT(updateAcquisitionState()));

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(updateAcquisitionState()));

	connect(dataSource()->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onDataChanged()));

}

SXRMBFourElementVortexDetector::~SXRMBFourElementVortexDetector()
{

}

QString SXRMBFourElementVortexDetector::details() const
{
	return QString("%1\nAcquisition Time: %2 seconds\nMaximum Energy: %3 keV\nPeaking Time: %4 us\n\n")
			.arg(description())
			.arg(acquisitionTime())
			.arg(maximumEnergy())
			.arg(peakingTime());
}

QString SXRMBFourElementVortexDetector::synchronizedDwellKey() const
{
	return "dxp1606-B10-02:EraseStart NPP NMS";
}

AMAction3* SXRMBFourElementVortexDetector::createInitializationActions()
{
	AMListAction3 *detectorInitializationActions = new AMListAction3(new AMListActionInfo3("4E Votex Detector Initialization", "4E Votex Detector Initialization Actions"), AMListAction3::Sequential);
	detectorInitializationActions->addSubAction(AMXRFDetector::createInitializationActions());

	AMListAction3 *scanPVInitializationListActions = new AMListAction3(new AMListActionInfo3("Votex Detector Scan PV Initialization", "Votex Detector Scan PV Initialization Actions"), AMListAction3::Parallel);
	detectorInitializationActions->addSubAction(scanPVInitializationListActions);

	AMListAction3 *scanPVInitializationActions = 0;

	// add the MCA initialization actions
	scanPVInitializationActions = new AMListAction3(new AMListActionInfo3("Votex MCA Rate initialization", "Votex MCA Rate initialization"), AMListAction3::Sequential);
	scanPVInitializationActions->addSubAction(AMActionSupport::buildControlMoveAction(mcaRatePVControl_, 0));  // passive
	scanPVInitializationActions->addSubAction(AMActionSupport::buildControlWaitAction(mcaRatePVControl_, 0));

	scanPVInitializationListActions->addSubAction(scanPVInitializationActions);

	// add the DXPs initialization actions
	scanPVInitializationActions = new AMListAction3(new AMListActionInfo3("Votex DXPs Rate initialization", "Votex DXPs Rate initialization"), AMListAction3::Sequential);
	scanPVInitializationActions->addSubAction(AMActionSupport::buildControlMoveAction(dxpRatePVControl_, 0));  // passive
	scanPVInitializationActions->addSubAction(AMActionSupport::buildControlWaitAction(dxpRatePVControl_, 0));

	scanPVInitializationListActions->addSubAction(scanPVInitializationActions);

	// add the status initialization actions
	scanPVInitializationActions = new AMListAction3(new AMListActionInfo3("Votex Staus Rate initialization", "Votex Status Rate initialization"), AMListAction3::Sequential);
	scanPVInitializationActions->addSubAction(AMActionSupport::buildControlMoveAction(statusRatePVControl_, 9));  // 0.1 second
	scanPVInitializationActions->addSubAction(AMActionSupport::buildControlWaitAction(statusRatePVControl_, 9));

	scanPVInitializationListActions->addSubAction(scanPVInitializationActions);


	return detectorInitializationActions;
}

AMAction3* SXRMBFourElementVortexDetector::createCleanupActions()
{
	AMListAction3 *detectorCleanupListActions = new AMListAction3(new AMListActionInfo3("4E Votex Detector Cleanup", "4E Votex Detector Cleanup Actions"), AMListAction3::Sequential);
	detectorCleanupListActions->addSubAction(AMXRFDetector::createCleanupActions());

	AMListAction3 *scanPVCleanupListActions = new AMListAction3(new AMListActionInfo3("Votex Detector Scan PV Cleanup", "Votex Detector Scan PV Cleanup Actions"), AMListAction3::Parallel);
	detectorCleanupListActions->addSubAction(scanPVCleanupListActions);

	AMListAction3 *scanPVCleanupActions = 0;

	// add the MCA cleanup actions
	scanPVCleanupActions = new AMListAction3(new AMListActionInfo3("Votex MCA cleanup", "Votex MCA cleanup"), AMListAction3::Sequential);
	scanPVCleanupActions->addSubAction(AMActionSupport::buildControlMoveAction(mcaRatePVControl_, mcaRatePVControl_->value()));
	scanPVCleanupActions->addSubAction(AMActionSupport::buildControlWaitAction(mcaRatePVControl_, mcaRatePVControl_->value()));

	scanPVCleanupListActions->addSubAction(scanPVCleanupActions);

	// add the DXPs cleanup actions
	scanPVCleanupActions = new AMListAction3(new AMListActionInfo3("Votex DXPs cleanup", "Votex DXPs Rate cleanup"), AMListAction3::Sequential);
	scanPVCleanupActions->addSubAction(AMActionSupport::buildControlMoveAction(dxpRatePVControl_, dxpRatePVControl_->value()));
	scanPVCleanupActions->addSubAction(AMActionSupport::buildControlWaitAction(dxpRatePVControl_, dxpRatePVControl_->value()));

	scanPVCleanupListActions->addSubAction(scanPVCleanupActions);

	// add the status cleanup actions
	scanPVCleanupActions = new AMListAction3(new AMListActionInfo3("Votex Status Rate cleanup", "Votex Status Rate cleanup"), AMListAction3::Sequential);
	scanPVCleanupActions->addSubAction(AMActionSupport::buildControlMoveAction(statusRatePVControl_, statusRatePVControl_->value()));
	scanPVCleanupActions->addSubAction(AMActionSupport::buildControlWaitAction(statusRatePVControl_, statusRatePVControl_->value()));

	scanPVCleanupListActions->addSubAction(scanPVCleanupActions);

	return detectorCleanupListActions;
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

void SXRMBFourElementVortexDetector::updateAcquisitionState()
{
	bool acquisitionStatusReady = acquisitionStatusControl_->withinTolerance(1);
	bool acquisitionControlAcquiring = acquireControl_->withinTolerance(1);

	if (acquisitionStatusReady) {

		if (isNotReadyForAcquisition()) {
			setReadyForAcquisition();

		} else if (acquisitionControlAcquiring && !isAcquiring()) {
			// starts to acquire new data
			dataReady_ = false;
			dataReadyCounter_ = elements();
			setAcquiring();
		}

	} else if (dataReady_) { // data is ready, we can say Hooray! and prepare for next acquiring
		if (isAcquiring()) {
			setAcquisitionSucceeded();
		}

		if (!isConnected() && !isNotReadyForAcquisition()) {
			setNotReadyForAcquisition();

		} else if (isConnected() && !isReadyForAcquisition()) {
			setReadyForAcquisition();
		}
	}
}

void SXRMBFourElementVortexDetector::onDataChanged()
{
	if (!dataReady_ && isAcquiring()) {

		dataReadyCounter_ --;
		if (dataReadyCounter_ == 0) {
			// we got all the data and try to say Hooray.
			dataReady_ = true;
			updateAcquisitionState();
		}
	}
}
