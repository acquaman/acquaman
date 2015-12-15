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


#include "AMDetectorTriggerSource.h"

#include "AMDetector.h"
#include "beamline/AMControl.h"

AMDetectorTriggerSource::AMDetectorTriggerSource(const QString &name, QObject *parent) :
	QObject(parent)
{
	name_ = name;
}

AMDetectorTriggerSource::~AMDetectorTriggerSource(){}

void AMDetectorTriggerSource::trigger(AMDetectorDefinitions::ReadMode readMode){
	emit triggered(readMode);
}

void AMDetectorTriggerSource::setSucceeded(){
	emit succeeded();
}

void AMDetectorTriggerSource::setFailed(){
	emit failed();
}

AMArmedDetectorTriggerSource::AMArmedDetectorTriggerSource(const QString &name, QObject *parent) :
	AMDetectorTriggerSource(name, parent)
{
	detectorArmingMapper_ = new QSignalMapper(this);
	triggerControl_ = 0;
}

AMArmedDetectorTriggerSource::~AMArmedDetectorTriggerSource()
{
}

void AMArmedDetectorTriggerSource::trigger(AMDetectorDefinitions::ReadMode readMode)
{
	readMode_ = readMode;
	armedDetectors_.clear();
	for(int x = 0, size = triggerSourceDetectors_.count(); x < size; x++)
		detectorArmingMapper_->setMapping(triggerSourceDetectors_.at(x), triggerSourceDetectors_.at(x));
	connect(detectorArmingMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onDetectorArmed(QObject*)));

	for(int x = 0, size = triggerSourceDetectors_.count(); x < size; x++)
		triggerSourceDetectors_.at(x)->arm();
}

void AMArmedDetectorTriggerSource::addDetector(AMDetector *detector)
{
	triggerSourceDetectors_.append(detector);
	connect(detector, SIGNAL(armed()), detectorArmingMapper_, SLOT(map()));
}

void AMArmedDetectorTriggerSource::setTriggerControl(AMControl *triggerControl)
{
	triggerControl_ = triggerControl;
}

void AMArmedDetectorTriggerSource::onDetectorArmed(QObject *detector)
{
	AMDetector *asDetector = qobject_cast<AMDetector*>(detector);
	if(asDetector){
		armedDetectors_.append(asDetector);
		detectorArmingMapper_->removeMappings(asDetector);
	}

	if(armedDetectors_.count() == triggerSourceDetectors_.count()){
		disconnect(detectorArmingMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onDetectorArmed(QObject*)));

		if(triggerControl_)
			triggerControl_->move(1);

		emit triggered(readMode_);
	}
}

AMDetectorDwellTimeSource::AMDetectorDwellTimeSource(const QString &name, QObject *parent) :
	QObject(parent)
{
	name_ = name;
}

AMDetectorDwellTimeSource::~AMDetectorDwellTimeSource(){}

void AMDetectorDwellTimeSource::requestSetDwellTime(double dwellSeconds)
{
    emit setDwellTime(dwellSeconds);
}

void AMDetectorDwellTimeSource:: requestSetDarkCurrentCorrectionTime(double timeSeconds)
{
    emit setDarkCurrentCorrectionTime(timeSeconds);

    emit darkCurrentTimeChanged(timeSeconds);
}

void AMDetectorDwellTimeSource::setSucceeded(){
	emit succeeded();
}

void AMDetectorDwellTimeSource::setFailed(){
	emit failed();
}
