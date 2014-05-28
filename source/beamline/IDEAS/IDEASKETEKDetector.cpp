#include "IDEASKETEKDetector.h"

#include "beamline/AMBeamline.h"

IDEASKETEKDetector::IDEASKETEKDetector(const QString &name, const QString &description, QObject *parent)
    : AMXRFDetector(name, description, parent)
{
    units_ = "Counts";

    AMAxisInfo ai("Energy", 2048, "Energy", "eV");
    ai.start = AMNumber(0);
    ai.increment = 10;
    ai.isUniform = true;
    axes_ << ai;

    // Stuff required by AMXRFDetector.
    acquireControl_ = new AMPVControl("Acquisition Time", "dxp1608-1002:mca1EraseStart", "dxp1608-1002:mca1EraseStart", "dxp1608-1002:mca1Stop", this, 0.5);
    acquisitionStatusControl_ = new AMReadOnlyPVControl("Status", "dxp1608-1002:mca1.ACQG", this);
    acquireTimeControl_ = new AMSinglePVControl("Integration Time", "dxp1608-1002:mca1.PRTM", this, 0.0001);
    elapsedTimeControl_ = new AMReadOnlyPVControl("Elapsed Time", "dxp1608-1002:mca1.ERTM", this);

    acquireTimeControl_->setAttemptMoveWhenWithinTolerance(false);

    icrControls_.append(new AMReadOnlyPVControl("Input Counts", "dxp1608-1002:dxp1:InputCountRate", this, "The input counts for the KETEK."));
    ocrControls_.append(new AMReadOnlyPVControl("Output Counts", "dxp1608-1002:dxp1:OutputCountRate", this, "The output counts for the KETEK."));
    spectraControls_.append(new AMReadOnlyPVControl("Raw Spectrum", "dxp1608-1002:mca1", this));

    allControlsCreated();

    foreach (AMDataSource *source, rawSpectraSources_)
            ((AM1DProcessVariableDataSource *)source)->setScale(10);
}

QString IDEASKETEKDetector::synchronizedDwellKey() const
{
        return "dxp1608-1002:mca1EraseStart NPP NMS";
}

bool IDEASKETEKDetector::sharesDetectorTriggerSource()
{
        return currentlySynchronizedDwell();
}

AMDetectorTriggerSource* IDEASKETEKDetector::detectorTriggerSource()
{
        if(currentlySynchronizedDwell())
                return AMBeamline::bl()->synchronizedDwellTime()->triggerSource();

        return 0;
}

AMDetectorDwellTimeSource* IDEASKETEKDetector::detectorDwellTimeSource()
{
        if(currentlySynchronizedDwell())
                return AMBeamline::bl()->synchronizedDwellTime()->dwellTimeSource();

        return 0;
}

bool IDEASKETEKDetector::lastContinuousReading(double *outputValues) const
{
        Q_UNUSED(outputValues)

        return false;
}

bool IDEASKETEKDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode)
{
        Q_UNUSED(readMode)

        return false;
}
