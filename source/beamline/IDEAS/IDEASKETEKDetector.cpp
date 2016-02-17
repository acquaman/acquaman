/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "IDEASKETEKDetector.h"

#include "beamline/AMBeamline.h"
#include "beamline/AMAdvancedControlDetectorEmulator.h"

IDEASKETEKDetector::~IDEASKETEKDetector(){}

IDEASKETEKDetector::IDEASKETEKDetector(const QString &name, const QString &description, QObject *parent)
    : AMXRFDetector(name, description, parent)
{
    units_ = "Counts";

    AMAxisInfo ai("Energy", 2048, "Energy", "eV");
    ai.start = AMNumber(0);
    ai.increment = 10;
    ai.isUniform = true;
    axes_ << ai;

    // Ketex functionality

    peakingTimeControl_ = new AMPVControl("XRF1E Peaking Time","dxp1608-1002:dxp1:PeakingTime_RBV","dxp1608-1002:dxp1:PeakingTime", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);
    ketekTriggerLevel_ = new AMPVControl("XRF1E Trigger Level","dxp1608-1002:dxp1:TriggerThreshold_RBV","dxp1608-1002:dxp1:TriggerThreshold", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);
    ketekBaselineThreshold_ = new AMPVControl("XRF1E Baseline Threshold","dxp1608-1002:dxp1:BaselineThreshold_RBV","dxp1608-1002:dxp1:BaselineThreshold", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);
    preampGainControl_ = new AMPVControl("XRF1E Preamp Gain","dxp1608-1002:dxp1:PreampGain_RBV","dxp1608-1002:dxp1:PreampGain", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);

    realTimeControl_ = new AMReadOnlyPVControl("XRF1E Real Time", "dxp1608-1002:mca1.ERTM", this);

    ketekRealTime_ = new AMBasicControlDetectorEmulator("dwellTime", "Dwell Time", realTimeControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);



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

    connect(peakingTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(peakingTimeChanged(double)));

}

QString IDEASKETEKDetector::details() const
{
	return QString("%1\nAcquisition Time: %2 seconds\nPeaking Time: %3 us\n\n")
			.arg(description())
			.arg(acquisitionTime())
			.arg(peakingTime());
}

QString IDEASKETEKDetector::synchronizedDwellKey() const
{
        return "dxp1608-1002:mca1EraseStart NPP NMS";
}

bool IDEASKETEKDetector::sharesDetectorTriggerSource() const
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

void IDEASKETEKDetector::setPeakingTime(double time)
{
	if(peakingTimeControl_->value() != time)
		peakingTimeControl_->move(time);
}

void IDEASKETEKDetector::setPreampGain(double value)
{
	if(preampGainControl_->value() != value)
		preampGainControl_->move(value);

}

