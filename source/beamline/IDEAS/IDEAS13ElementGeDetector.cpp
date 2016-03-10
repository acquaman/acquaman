#include "IDEAS13ElementGeDetector.h"
#include "beamline/AMAdvancedControlDetectorEmulator.h"

IDEAS13ElementGeDetector::IDEAS13ElementGeDetector(const QString &name, const QString &description, QObject *parent)
	: AMXRFDetector(name, description, parent)
{
	units_ = "Counts";

	AMAxisInfo ai("Energy", 2048, "Energy", "eV");
	ai.start = AMNumber(0);
	ai.increment = 10;
	ai.isUniform = true;
	axes_ << ai;

	// Stuff required by AMXRFDetector.
	acquireControl_ = new AMPVControl("Acquisition Time", "dxp1608-B21-13:EraseStart", "dxp1608-B21-13:EraseStart", "dxp1608-B21-13:StopAll", this, 0.5);
	acquisitionStatusControl_ = new AMReadOnlyPVControl("Status", "dxp1608-B21-13:Acquiring", this);
	acquireTimeControl_ = new AMSinglePVControl("Integration Time", "dxp1608-B21-13:PresetReal", this, 0.001);
	elapsedTimeControl_ = new AMReadOnlyPVControl("Elapsed Time", "dxp1608-B21-13:ElapsedReal", this);

	// Detector functionality
	ge13ElementRealTimeControl_ = new AMReadOnlyPVControl("13-el Ge Real Time", "dxp1608-B21-13:ElapsedReal", this);
	ge13ElementRealTime_ = new AMBasicControlDetectorEmulator("13E_dwellTime", "13-element Ge dwell time", ge13ElementRealTimeControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);

    // Energy Peaking Time
    peakingTimeControl_ = new AMSinglePVControl("Peaking Time", "dxp1608-B21-13:EnergyPkTime", this, 0.01);

    // preamp Gain controls for elements 1-12
    for(int i = 0; i < 12; i++){
        preampGainControls_ << new AMPVControl(QString("13-el Ge Preamp Gain %1").arg(i+1),QString("dxp1608-B12-13:dxp%1:PreampGain_RBV").arg(i+1), "dxp1608-B12-13:dxp%1:PreampGain", QString(), this);
    }

	// Currently only using 12 due to lack of electronics.
	for (int i = 0; i < 12; i++){

		icrControls_.append(new AMReadOnlyPVControl(QString("Input Counts %1").arg(i+1), QString("dxp1608-B21-13:dxp%1:InputCountRate").arg(i+1), this, QString("The input counts for element %1 of the four element.").arg(i+1)));
		ocrControls_.append(new AMReadOnlyPVControl(QString("Output Counts %1").arg(i+1), QString("dxp1608-B21-13:dxp%1:OutputCountRate").arg(i+1), this, QString("The output counts for element %1 of the four element.").arg(i+1)));
		spectraControls_.append(new AMReadOnlyPVControl(QString("Raw Spectrum %1").arg(i+1), QString("dxp1608-B21-13:mca%1").arg(i+1), this));
	}

	allControlsCreated();

	foreach (AMDataSource *source, rawSpectraSources_)
		((AM1DProcessVariableDataSource *)source)->setScale(10);

}

QString IDEAS13ElementGeDetector::details() const
{
	return QString("%1\nAcquisition Time: %2 seconds\n\n")
			.arg(description())
			.arg(acquisitionTime());
}

double IDEAS13ElementGeDetector::preampGainAt(int index) const
{
   return preampGainControls_.at(index)->value();
}


QString IDEAS13ElementGeDetector::synchronizedDwellKey() const
{
	return "dxp1608-B21-13:EraseStart NPP NMS";
}

bool IDEAS13ElementGeDetector::sharesDetectorTriggerSource() const
{
	return currentlySynchronizedDwell();
}

#include "beamline/AMBeamline.h"
AMDetectorTriggerSource* IDEAS13ElementGeDetector::detectorTriggerSource()
{
	if(currentlySynchronizedDwell())
		return AMBeamline::bl()->synchronizedDwellTime()->triggerSource();

	return 0;
}

AMDetectorDwellTimeSource* IDEAS13ElementGeDetector::detectorDwellTimeSource()
{
	if(currentlySynchronizedDwell())
		return AMBeamline::bl()->synchronizedDwellTime()->dwellTimeSource();

	return 0;
}

bool IDEAS13ElementGeDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode)
{
	Q_UNUSED(readMode)

	return false;
}
