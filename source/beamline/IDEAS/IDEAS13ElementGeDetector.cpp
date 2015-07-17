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
    preampGainControl1_ = new AMPVControl("13-el Ge Preamp Gain 1","dxp1608-B12-13:dxp1:PreampGain_RBV","dxp1608-B12-13:dxp1:PreampGain", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);
    preampGainControl2_ = new AMPVControl("13-el Ge Preamp Gain 2","dxp1608-B12-13:dxp2:PreampGain_RBV","dxp1608-B12-13:dxp2:PreampGain", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);
    preampGainControl3_ = new AMPVControl("13-el Ge Preamp Gain 3","dxp1608-B12-13:dxp3:PreampGain_RBV","dxp1608-B12-13:dxp3:PreampGain", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);
    preampGainControl4_ = new AMPVControl("13-el Ge Preamp Gain 4","dxp1608-B12-13:dxp4:PreampGain_RBV","dxp1608-B12-13:dxp4:PreampGain", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);
    preampGainControl5_ = new AMPVControl("13-el Ge Preamp Gain 5","dxp1608-B12-13:dxp5:PreampGain_RBV","dxp1608-B12-13:dxp5:PreampGain", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);
    preampGainControl6_ = new AMPVControl("13-el Ge Preamp Gain 6","dxp1608-B12-13:dxp6:PreampGain_RBV","dxp1608-B12-13:dxp6:PreampGain", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);
    preampGainControl7_ = new AMPVControl("13-el Ge Preamp Gain 7","dxp1608-B12-13:dxp7:PreampGain_RBV","dxp1608-B12-13:dxp7:PreampGain", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);
    preampGainControl8_ = new AMPVControl("13-el Ge Preamp Gain 8","dxp1608-B12-13:dxp8:PreampGain_RBV","dxp1608-B12-13:dxp8:PreampGain", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);
    preampGainControl9_ = new AMPVControl("13-el Ge Preamp Gain 9","dxp1608-B12-13:dxp9:PreampGain_RBV","dxp1608-B12-13:dxp9:PreampGain", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);
    preampGainControl10_ = new AMPVControl("13-el Ge Preamp Gain 10","dxp1608-B12-13:dxp10:PreampGain_RBV","dxp1608-B12-13:dxp10:PreampGain", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);
    preampGainControl11_ = new AMPVControl("13-el Ge Preamp Gain 11","dxp1608-B12-13:dxp11:PreampGain_RBV","dxp1608-B12-13:dxp11:PreampGain", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);
    preampGainControl12_ = new AMPVControl("13-el Ge Preamp Gain 12","dxp1608-B12-13:dxp12:PreampGain_RBV","dxp1608-B12-13:dxp12:PreampGain", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);

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

bool IDEAS13ElementGeDetector::lastContinuousReading(double *outputValues) const
{
	Q_UNUSED(outputValues)

	return false;
}

bool IDEAS13ElementGeDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode)
{
	Q_UNUSED(readMode)

	return false;
}
