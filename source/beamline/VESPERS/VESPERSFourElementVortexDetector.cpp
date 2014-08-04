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


#include "VESPERSFourElementVortexDetector.h"

 VESPERSFourElementVortexDetector::~VESPERSFourElementVortexDetector(){}
VESPERSFourElementVortexDetector::VESPERSFourElementVortexDetector(const QString &name, const QString &description, QObject *parent)
	: AMXRFDetector(name, description, parent)
{
	units_ = "Counts";

	AMAxisInfo ai("Energy", 2048, "Energy", "eV");
	ai.start = AMNumber(0);
	ai.increment = 10;
	ai.isUniform = true;
	axes_ << ai;

	// Stuff required by AMXRFDetector.
	acquireControl_ = new AMPVControl("Acquisition Time", "dxp1607-B21-04:EraseStart", "dxp1607-B21-04:EraseStart", "dxp1607-B21-04:StopAll", this, 0.5);
	acquisitionStatusControl_ = new AMReadOnlyPVControl("Status", "dxp1607-B21-04:Acquiring", this);
	acquireTimeControl_ = new AMSinglePVControl("Integration Time", "dxp1607-B21-04:PresetReal", this, 0.001);
	elapsedTimeControl_ = new AMReadOnlyPVControl("Elapsed Time", "dxp1607-B21-04:ElapsedReal", this);

	for (int i = 0; i < 4; i++){

		icrControls_.append(new AMReadOnlyPVControl(QString("Input Counts %1").arg(i+1), QString("dxp1607-B21-04:dxp%1:InputCountRate").arg(i+1), this, QString("The input counts for element %1 of the four element.").arg(i+1)));
		ocrControls_.append(new AMReadOnlyPVControl(QString("Output Counts %1").arg(i+1), QString("dxp1607-B21-04:dxp%1:OutputCountRate").arg(i+1), this, QString("The output counts for element %1 of the four element.").arg(i+1)));
		spectraControls_.append(new AMReadOnlyPVControl(QString("Raw Spectrum %1").arg(i+1), QString("dxp1607-B21-04:mca%1").arg(i+1), this));
	}

	allControlsCreated();

	// Our own stuff.
	maximumEnergyControl_ = new AMSinglePVControl("Maximum Energy", "dxp1607-B21-04:mcaEMax", this, 0.01);
	peakingTimeControl_ = new AMSinglePVControl("Peaking Time", "dxp1607-B21-04:EnergyPkTime", this, 0.01);

	connect(maximumEnergyControl_, SIGNAL(valueChanged(double)), this, SLOT(onMaximumEnergyChanged(double)));
	connect(peakingTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(peakingTimeChanged(double)));
}

QString VESPERSFourElementVortexDetector::synchronizedDwellKey() const
{
	return "dxp1607-B21-04:EraseStart NPP NMS";
}

bool VESPERSFourElementVortexDetector::sharesDetectorTriggerSource() const
{
	return currentlySynchronizedDwell();
}

#include "beamline/AMBeamline.h"
AMDetectorTriggerSource* VESPERSFourElementVortexDetector::detectorTriggerSource()
{
	if(currentlySynchronizedDwell())
		return AMBeamline::bl()->synchronizedDwellTime()->triggerSource();

	return 0;
}

AMDetectorDwellTimeSource* VESPERSFourElementVortexDetector::detectorDwellTimeSource()
{
	if(currentlySynchronizedDwell())
		return AMBeamline::bl()->synchronizedDwellTime()->dwellTimeSource();

	return 0;
}

bool VESPERSFourElementVortexDetector::lastContinuousReading(double *outputValues) const
{
	Q_UNUSED(outputValues)

	return false;
}

bool VESPERSFourElementVortexDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode)
{
	Q_UNUSED(readMode)

	return false;
}

//const double *VESPERSFourElementVortexDetector::data() const
//{
//	return 0;
//}

void VESPERSFourElementVortexDetector::setMaximumEnergy(double energy)
{
	if (maximumEnergyControl_->value() != energy)
		maximumEnergyControl_->move(energy);
}

void VESPERSFourElementVortexDetector::setPeakingTime(double time)
{
	if (peakingTimeControl_->value() != time)
		peakingTimeControl_->move(time);
}

void VESPERSFourElementVortexDetector::onMaximumEnergyChanged(double newMaximum)
{
	double newScale = (newMaximum*1000)/axes_.at(0).size;
	axes_[0].increment = newScale;

	foreach (AMDataSource *source, rawSpectraSources_)
		((AM1DProcessVariableDataSource *)source)->setScale(newScale);

	emit maximumEnergyChanged(newMaximum);
}
