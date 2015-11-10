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


#include "VESPERSCCDDetector.h"

#include "actions3/AMActionSupport.h"
#include "actions3/VESPERS/VESPERSSetStringAction.h"
#include "beamline/AMBeamline.h"


#include <QStringBuilder>

VESPERSCCDDetector::~VESPERSCCDDetector(){}

VESPERSCCDDetector::VESPERSCCDDetector(const QString &name, const QString &description, QObject *parent)
	: AMDetector(name, description, parent)
{
	units_ = "Counts";

	allControls_ = new AMControlSet(this);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
	connect(allControls_, SIGNAL(controlSetTimedOut()), this, SLOT(onControlsTimedOut()));
	connect(&elapsedTimer_, SIGNAL(timeout()), this, SLOT(onElapsedTimeChanged()));
}

void VESPERSCCDDetector::allControlsCreated()
{
	allControls_->addControl(acquireControl_);
	allControls_->addControl(acquireTimeControl_);
	allControls_->addControl(acquisitionStatusControl_);
	allControls_->addControl(ccdFilePathControl_);
	allControls_->addControl(ccdFileBaseNameControl_);
	allControls_->addControl(ccdFileNumberControl_);

	connect(acquireTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(acquisitionTimeChanged(double)));
	connect(acquisitionStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onStatusControlChanged()));
	connect(ccdFilePathControl_, SIGNAL(valueChanged(double)), this, SLOT(onCCDFilePathChanged()));
	connect(ccdFileBaseNameControl_, SIGNAL(valueChanged(double)), this, SLOT(onCCDFileBaseNameChanged()));
	connect(ccdFileNumberControl_, SIGNAL(valueChanged(double)), this, SLOT(onCCDFileNumberChanged()));
}

void VESPERSCCDDetector::onControlsConnected(bool connected)
{
	setConnected(connected);

	if (connected)
		setReadyForAcquisition();

	else if (!isNotReadyForAcquisition())
		setNotReadyForAcquisition();
}

void VESPERSCCDDetector::onControlsTimedOut()
{
	setConnected(false);
}

void VESPERSCCDDetector::onStatusControlChanged()
{
	if (acquisitionStatusControl_->withinTolerance(1))
		setAcquiring();

	else if (acquisitionStatusControl_->withinTolerance(0)){

		if (isAcquiring())
			setAcquisitionSucceeded();

		if (!isConnected() && !isNotReadyForAcquisition())
			setNotReadyForAcquisition();

		else if (isConnected() && !isReadyForAcquisition())
			setReadyForAcquisition();
	}
}

AMAction3 *VESPERSCCDDetector::createFilePathAction(const QString &path)
{
	if (!ccdFilePathControl_->isConnected())
		return 0;

	return new VESPERSSetStringAction(new VESPERSSetStringActionInfo(path), ccdFilePathControl_);
}

AMAction3 *VESPERSCCDDetector::createFileNameAction(const QString &name)
{
	if (!ccdFileBaseNameControl_->isConnected())
		return 0;

	return new VESPERSSetStringAction(new VESPERSSetStringActionInfo(name), ccdFileBaseNameControl_);
}

AMAction3 *VESPERSCCDDetector::createFileNumberAction(int number)
{
	if (!ccdFileNumberControl_->isConnected())
		return 0;

    return AMActionSupport::buildControlMoveAction(ccdFileNumberControl_, number);

}

bool VESPERSCCDDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode)
{
	Q_UNUSED(readMode)

	return false;
}

bool VESPERSCCDDetector::initializeImplementation()
{
	setInitialized();
	return true;
}

bool VESPERSCCDDetector::acquireImplementation(AMDetectorDefinitions::ReadMode readMode)
{
	if (!isConnected() || readMode != AMDetectorDefinitions::SingleRead)
		return false;

	elapsedTimer_.start();
	elapsedTime_.restart();
	AMControl::FailureExplanation failureExplanation = acquireControl_->move(1);
	return failureExplanation == AMControl::NoFailure;
}

bool VESPERSCCDDetector::cleanupImplementation()
{
	elapsedTimer_.stop();
	setCleanedUp();
	return true;
}

bool VESPERSCCDDetector::cancelAcquisitionImplementation()
{
	elapsedTimer_.stop();
	bool stopSuccessful = acquireControl_->stop();
	setAcquisitionCancelled();
	return stopSuccessful;
}

bool VESPERSCCDDetector::setAcquisitionTime(double seconds)
{
	if(!isConnected())
		return false;

	if(!acquireTimeControl_->withinTolerance(seconds))
		acquireTimeControl_->move(seconds);

	return true;
}

double VESPERSCCDDetector::acquisitionTime() const
{
	if (isConnected())
		return acquireTimeControl_->value();

	return -1;
}

double VESPERSCCDDetector::acquisitionTimeTolerance() const
{
	if (isConnected())
		return acquireTimeControl_->tolerance();

	return -1;
}

QString VESPERSCCDDetector::ccdFilePath() const
{
	const AMProcessVariable *pv = ccdFilePathControl_->readPV();
	return VESPERS::pvToString(pv);
}

QString VESPERSCCDDetector::ccdFileBaseName() const
{
	const AMProcessVariable *pv = ccdFileBaseNameControl_->readPV();
	return VESPERS::pvToString(pv);
}

int VESPERSCCDDetector::ccdFileNumber() const
{
	return int(ccdFileNumberControl_->value());
}

void VESPERSCCDDetector::onCCDFilePathChanged()
{
	const AMProcessVariable *pv = ccdFilePathControl_->readPV();
	emit ccdPathChanged(VESPERS::pvToString(pv));
}

void VESPERSCCDDetector::onCCDFileBaseNameChanged()
{
	const AMProcessVariable *pv = ccdFileBaseNameControl_->readPV();
	emit ccdNameChanged(VESPERS::pvToString(pv));
}

void VESPERSCCDDetector::onCCDFileNumberChanged()
{
	emit ccdNumberChanged(int(ccdFileNumberControl_->value()));
}

void VESPERSCCDDetector::setCCDFilePath(QString path)
{
	AMProcessVariable *pv = ccdFilePathControl_->writePV();
	VESPERS::stringToPV(pv, path);
}

void VESPERSCCDDetector::setCCDFileBaseName(QString name)
{
	AMProcessVariable *pv = ccdFileBaseNameControl_->writePV();
	VESPERS::stringToPV(pv, name);
}

void VESPERSCCDDetector::setCCDNumber(int number)
{
	ccdFileNumberControl_->move(double(number));
}

void VESPERSCCDDetector::onElapsedTimeChanged()
{
	emit elapsedTimeChanged(elapsedTime_.elapsed()/1000.0);
}
