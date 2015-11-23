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


#include "SGMXASScanConfiguration2013.h"

#include "beamline/AMBeamline.h"
#include "beamline/SGM/SGMBeamline.h"

SGMXASScanConfiguration2013::~SGMXASScanConfiguration2013(){}

SGMXASScanConfiguration2013::SGMXASScanConfiguration2013(QObject *parent) :
	AMStepScanConfiguration(parent), SGMScanConfiguration2013()
{
	AMScanAxisRegion *region = new AMScanAxisRegion;
	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);

	connect(SGMBeamline::sgm()->exitSlitGap(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(SGMBeamline::sgm()->grating(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(SGMBeamline::sgm()->harmonic(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(SGMBeamline::sgm()->undulatorTracking(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(SGMBeamline::sgm()->monoTracking(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(SGMBeamline::sgm()->exitSlitTracking(), SIGNAL(valueChanged(double)), this,SLOT(checkIfMatchesBeamline()));
	connect(this->dbObject(), SIGNAL(exitSlitGapChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(this->dbObject(), SIGNAL(gratingChanged(SGMBeamlineInfo::sgmGrating)), this, SLOT(checkIfMatchesBeamline()));
	connect(this->dbObject(), SIGNAL(harmonicChanged(SGMBeamlineInfo::sgmHarmonic)), this, SLOT(checkIfMatchesBeamline()));
	connect(this->dbObject(), SIGNAL(trackingGroupChanged()), this, SLOT(checkIfMatchesBeamline()));
}

SGMXASScanConfiguration2013::SGMXASScanConfiguration2013(const SGMXASScanConfiguration2013 &original) :
	AMStepScanConfiguration(original), SGMScanConfiguration2013(original)
{
	detectorConfigurations_ = original.detectorConfigurations();
	setTrackingGroup(original.trackingGroup());
	setFluxResolutionGroup(original.fluxResolutionGroup());
	connect(SGMBeamline::sgm()->exitSlitGap(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(SGMBeamline::sgm()->grating(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(SGMBeamline::sgm()->harmonic(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(SGMBeamline::sgm()->undulatorTracking(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(SGMBeamline::sgm()->monoTracking(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(SGMBeamline::sgm()->exitSlitTracking(), SIGNAL(valueChanged(double)), this,SLOT(checkIfMatchesBeamline()));
	connect(this->dbObject(), SIGNAL(exitSlitGapChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(this->dbObject(), SIGNAL(gratingChanged(SGMBeamlineInfo::sgmGrating)), this, SLOT(checkIfMatchesBeamline()));
	connect(this->dbObject(), SIGNAL(harmonicChanged(SGMBeamlineInfo::sgmHarmonic)), this, SLOT(checkIfMatchesBeamline()));
	connect(this->dbObject(), SIGNAL(trackingGroupChanged()), this, SLOT(checkIfMatchesBeamline()));

}

AMScanConfiguration* SGMXASScanConfiguration2013::createCopy() const
{
	AMScanConfiguration *configuration = new SGMXASScanConfiguration2013(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

#include "SGMXASScanActionController.h"

AMScanController* SGMXASScanConfiguration2013::createController()
{
	SGMXASScanActionController *controller = new SGMXASScanActionController(this);
	controller->buildScanController();

	return controller;
}

#include "ui/SGM/SGMXASScanConfiguration2013View.h"

AMScanConfigurationView* SGMXASScanConfiguration2013::createView(){
	SGMXASScanConfiguration2013View *view = new SGMXASScanConfiguration2013View(this);

	AMDetectorSelector *xasDetectorSelector = new AMDetectorSelector(SGMBeamline::sgm()->XASDetectorGroup());
	QStringList preferentialOrdering;
	if(SGMBeamline::sgm()->newAmptekSDD1()){
		preferentialOrdering << SGMBeamline::sgm()->newAmptekSDD1()->name();
		xasDetectorSelector->setDetectorDefault(SGMBeamline::sgm()->newAmptekSDD1(), true);
	}
	if(SGMBeamline::sgm()->newAmptekSDD2()){
		preferentialOrdering << SGMBeamline::sgm()->newAmptekSDD2()->name();
		xasDetectorSelector->setDetectorDefault(SGMBeamline::sgm()->newAmptekSDD2(), true);
	}
	if(SGMBeamline::sgm()->newAmptekSDD3()){
		preferentialOrdering << SGMBeamline::sgm()->newAmptekSDD3()->name();
		xasDetectorSelector->setDetectorDefault(SGMBeamline::sgm()->newAmptekSDD3(), true);
	}
	if(SGMBeamline::sgm()->newAmptekSDD4()){
		preferentialOrdering << SGMBeamline::sgm()->newAmptekSDD4()->name();
		xasDetectorSelector->setDetectorDefault(SGMBeamline::sgm()->newAmptekSDD4(), true);
	}
	if(SGMBeamline::sgm()->newAmptekSDD5()){
		preferentialOrdering << SGMBeamline::sgm()->newAmptekSDD5()->name();
		xasDetectorSelector->setDetectorDefault(SGMBeamline::sgm()->newAmptekSDD5(), true);
	}
	if(SGMBeamline::sgm()->newI0Detector()){
		preferentialOrdering << SGMBeamline::sgm()->newI0Detector()->name();
		xasDetectorSelector->setDetectorDefault(SGMBeamline::sgm()->newI0Detector(), true);
	}
	if(SGMBeamline::sgm()->newTEYDetector()){
		preferentialOrdering << SGMBeamline::sgm()->newTEYDetector()->name();
		xasDetectorSelector->setDetectorDefault(SGMBeamline::sgm()->newTEYDetector(), true);
	}
	if(SGMBeamline::sgm()->newTFYDetector()){
		preferentialOrdering << SGMBeamline::sgm()->newTFYDetector()->name();
	}
	if(SGMBeamline::sgm()->newPDDetector()){
		preferentialOrdering << SGMBeamline::sgm()->newPDDetector()->name();
	}
	if(SGMBeamline::sgm()->energyFeedbackDetector()) {
		xasDetectorSelector->setDetectorDefault(SGMBeamline::sgm()->energyFeedbackDetector(), true);
	}
	if(SGMBeamline::sgm()->dwellTimeDetector()) {
		xasDetectorSelector->setDetectorDefault(SGMBeamline::sgm()->dwellTimeDetector(), true);
	}

	xasDetectorSelector->setPreferentialOrdering(preferentialOrdering);

	for(int x = 0, size = detectorConfigurations().count(); x < size; x++)
		xasDetectorSelector->setDetectorSelectedByName(detectorConfigurations().at(x).name(), true);

	view->setDetectorSelector(xasDetectorSelector);

	return view;
}

QString SGMXASScanConfiguration2013::description() const{
	AMScanAxis *xasAxis = scanAxisAt(0);
	if(xasAxis->regionCount() != 0)
		return QString("XAS Scan from %1eV to %2eV").arg((double)(xasAxis->regionAt(0)->regionStart())).arg((double)(xasAxis->regionAt(xasAxis->regionCount()-1)->regionEnd()));
	else
		return QString("XAS Scan");
}

QString SGMXASScanConfiguration2013::detailedDescription() const{
	return QString("XAS Scan from %1 to %2\nExit Slit: %3\nGrating: %4\nHarmonic: %5").arg(double(scanAxisAt(0)->regionAt(0)->regionStart())).arg(double(scanAxisAt(0)->regionAt(scanAxisAt(0)->regionCount()-1)->regionEnd())).arg(exitSlitGap(), 0, 'f', 1).arg(SGMBeamlineInfo::sgmInfo()->sgmGratingDescription(SGMBeamlineInfo::sgmGrating(grating()))).arg(SGMBeamlineInfo::sgmInfo()->sgmHarmonicDescription(SGMBeamlineInfo::sgmHarmonic(harmonic())));
}

QString SGMXASScanConfiguration2013::autoScanName() const{
	return QString("XAS");
}

QString SGMXASScanConfiguration2013::technique() const{
	return QString("XAS");
}

void SGMXASScanConfiguration2013::getSettingsFromBeamline()
{
	dbObject()->setExitSlitGap(SGMBeamline::sgm()->exitSlitGap()->value());
	dbObject()->setGrating((SGMBeamlineInfo::sgmGrating)SGMBeamline::sgm()->grating()->value());
	dbObject()->setHarmonic((SGMBeamlineInfo::sgmHarmonic)SGMBeamline::sgm()->harmonic()->value());
	dbObject()->setUndulatorTracking((int)SGMBeamline::sgm()->undulatorTracking()->value());
	dbObject()->setMonoTracking((int)SGMBeamline::sgm()->monoTracking()->value());
	dbObject()->setExitSlitTracking((int)SGMBeamline::sgm()->exitSlitTracking()->value());

}

bool SGMXASScanConfiguration2013::matchesCurrentBeamline()
{
	return matchesCurrentBeamline_;
}

void SGMXASScanConfiguration2013::checkIfMatchesBeamline()
{
	bool currentMatchStatus;
	//Ensure bl is connected AND fluxResolutionGroup/TrackingGroup have been properly initialized
	if(SGMBeamline::sgm()->isConnected() && fluxResolutionGroup().count() > 0 && trackingGroup().count() > 0)
	{
		currentMatchStatus =
			(floatCompare(dbObject()->exitSlitGap() + 1.0e-200, SGMBeamline::sgm()->exitSlitGap()->value() + 1.0e-200, 0.20) &&
			 floatCompare(dbObject()->grating() + 1.0e-200, SGMBeamline::sgm()->grating()->value()  + 1.0e-200, 0.0001) &&
			 floatCompare(dbObject()->harmonic()  + 1.0e-200, SGMBeamline::sgm()->harmonic()->value()  + 1.0e-200, 0.001) &&
			 floatCompare(dbObject()->undulatorTracking()  + 1.0e-200, SGMBeamline::sgm()->undulatorTracking()->value()  + 1.0e-200, 0.001) &&
			 floatCompare(dbObject()->monoTracking()  + 1.0e-200, SGMBeamline::sgm()->monoTracking()->value()  + 1.0e-200, 0.001) &&
			 floatCompare(dbObject()->exitSlitTracking()  + 1.0e-200, SGMBeamline::sgm()->exitSlitTracking()->value()  + 1.0e-200, 0.001));
	}
	else
	{
		currentMatchStatus = false;
	}

	if(currentMatchStatus != this->matchesCurrentBeamline_)
	{
		matchesCurrentBeamline_ = currentMatchStatus;
		emit matchingBeamlineStatusChanged(currentMatchStatus);
	}
}

inline bool SGMXASScanConfiguration2013::floatCompare(float x, float y, float epsilon)
{
	return fabs(x - y) <= ( (fabs(x) < fabs(y) ? fabs(y) : fabs(x)) * epsilon);
}
