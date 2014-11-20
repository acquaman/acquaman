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


#include "AMSampleMoveActionInfo.h"

#include "dataman/AMSample.h"
#include "dataman/database/AMConstDbObject.h"

AMSampleMoveActionInfo::AMSampleMoveActionInfo(const AMSample *sample, QObject *parent) :
	AMActionInfo3("Sample Move", "Sample Move", ":/system-run.png", parent )
{
	sample_ = 0; //NULL
	setSample(sample);
}

AMSampleMoveActionInfo::AMSampleMoveActionInfo(const AMSampleMoveActionInfo &other) :
	AMActionInfo3(other)
{
	sample_ = 0; //NULL
	setSample(other.sample());
}

AMSampleMoveActionInfo::~AMSampleMoveActionInfo()
{
	if(sample_){
		sample_->setObject(0);
		sample_->deleteLater();
	}
}

AMActionInfo3* AMSampleMoveActionInfo::createCopy() const{
	return new AMSampleMoveActionInfo(*this);
}

QString AMSampleMoveActionInfo::typeDescription() const{
	return "Sample Move";
}

const AMSample* AMSampleMoveActionInfo::sample() const{
	const AMSample *asSample = qobject_cast<const AMSample*>(sample_->object());
	return asSample;
}

void AMSampleMoveActionInfo::setSample(const AMSample *sample){
	if(!sample_)
		sample_ = new AMConstDbObject(sample);
	else
		sample_->setObject(sample);
	setModified(true);

	updateDescriptionText();
}

void AMSampleMoveActionInfo::updateDescriptionText(){
	QString description;
	if(sample_->object())
		description = QString("Moving to %1").arg(sample_->object()->name());
	else
		description = QString("No Sample Selected");

	setShortDescription(description);
	setLongDescription(description);
}

AMConstDbObject* AMSampleMoveActionInfo::dbReadSample() const{
	return sample_;
}

void AMSampleMoveActionInfo::dbLoadSample(AMConstDbObject *newSample){
	sample_ = newSample;
}
