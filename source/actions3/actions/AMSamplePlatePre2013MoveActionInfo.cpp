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


#include "AMSamplePlatePre2013MoveActionInfo.h"

#include "dataman/AMSamplePlatePre2013.h"
#include "util/AMErrorMonitor.h"

AMSamplePlatePre2013MoveActionInfo::AMSamplePlatePre2013MoveActionInfo(int samplePlateId, int samplePositionIndex, QObject *parent) :
	AMActionInfo3("Sample Move", "Sample Move", ":/system-run.png", parent)
{
	setSamplePlateId(samplePlateId);
	setSamplePositionIndex(samplePositionIndex);
	samplePosition_ = 0; //NULL
	updateDescriptionText();
}

AMSamplePlatePre2013MoveActionInfo::AMSamplePlatePre2013MoveActionInfo(const AMSamplePlatePre2013MoveActionInfo &other) :
	AMActionInfo3(other)
{
	setSamplePlateId(other.samplePlateId());
	setSamplePositionIndex(other.samplePositionIndex());
	samplePosition_ = 0; //NULL
}

AMSamplePlatePre2013MoveActionInfo::~AMSamplePlatePre2013MoveActionInfo(){}

AMActionInfo3* AMSamplePlatePre2013MoveActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMSamplePlatePre2013MoveActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

QString AMSamplePlatePre2013MoveActionInfo::typeDescription() const{
	return "Sample Move";
}

int AMSamplePlatePre2013MoveActionInfo::samplePlateId() const{
	return samplePlateId_;
}

int AMSamplePlatePre2013MoveActionInfo::samplePositionIndex() const{
	return samplePositionIndex_;
}

const AMControlInfoList* AMSamplePlatePre2013MoveActionInfo::samplePosition(){
	if( (samplePlateId_ == -1) || (samplePositionIndex_ == -1) )
		return 0; //NULL

	AMSamplePlatePre2013 samplePlate;
	if(!samplePlate.loadFromDb(AMDatabase::database("user"), samplePlateId_)){
		AMErrorMon::alert(this, AMSAMPLEPLATEPRE2013MOVEACTIONINFO_INVALID_SAMPLEPLATE_ID, QString("Failed to generate sample move information, there is no plate with id %1 in the database").arg(samplePlateId_));
		return 0; //NULL
	}

	int samplePositionId = -1;
	if(samplePositionIndex_ < samplePlate.count()){
		samplePositionId = samplePlate.at(samplePositionIndex_).position().id();
	}

	if(samplePositionId < 0){
		AMErrorMon::alert(this, AMSAMPLEPLATEPRE2013MOVEACTIONINFO_INVALID_SAMPLEPOSITION_INDEX, QString("Failed to generate sample move information, there is no position at index %1 on plate %2").arg(samplePositionIndex_).arg(samplePlateId_));
		return 0; //NULL
	}

	samplePosition_ = new AMControlInfoList();
	if(!samplePosition_->loadFromDb(AMDatabase::database("user"), samplePositionId)){
		AMErrorMon::alert(this, AMSAMPLEPLATEPRE2013MOVEACTIONINFO_INVALID_SAMPLEPOSITION_ID, QString("Failed to generate sample move information, there is no position with id %1 on plate %2").arg(samplePositionIndex_).arg(samplePlateId_));
		return 0; //NULL
	}
	return samplePosition_;
}

QString AMSamplePlatePre2013MoveActionInfo::samplePlateName() const{
	if(samplePlateId() == -1)
		return "No Plate Selected";

	AMSamplePlatePre2013 samplePlate;
	if(!samplePlate.loadFromDb(AMDatabase::database("user"), samplePlateId_))
		return "Plate Error";

	return samplePlate.name();
}

QString AMSamplePlatePre2013MoveActionInfo::samplePositionName() const{
	if(samplePlateId() == -1)
		return "No Plate Selected";

	if(samplePositionIndex() < 0)
		return "No Sample Selected";

	AMSamplePlatePre2013 samplePlate;
	if(!samplePlate.loadFromDb(AMDatabase::database("user"), samplePlateId_))
		return "Plate Error";

	if(samplePositionIndex() > samplePlate.count())
		return "Sample Index Error";

	AMSamplePre2013 actualSample;
	if(!actualSample.loadFromDb(AMDatabase::database("user"), samplePlate.at(samplePositionIndex()).sampleId()))
		return "Sample Error";

	return actualSample.name();
}

void AMSamplePlatePre2013MoveActionInfo::setSamplePlateId(int samplePlateId){
	if(samplePlateId_ != samplePlateId){
		samplePlateId_ = samplePlateId;
		samplePosition_ = 0; //NULL
		setModified(true);
		updateDescriptionText();
	}
}

void AMSamplePlatePre2013MoveActionInfo::setSamplePositionIndex(int samplePositionId){
	if(samplePositionIndex_ != samplePositionId){
		samplePositionIndex_ = samplePositionId;
		samplePosition_ = 0; //NULL
		setModified(true);
		updateDescriptionText();
	}
}

void AMSamplePlatePre2013MoveActionInfo::updateDescriptionText(){
	QString description;

	if(samplePlateId_ == -1)
		description = samplePlateName();
	else if(samplePositionIndex_ == -1)
		description = QString("%1 on plate %2").arg(samplePositionName()).arg(samplePlateName());
	else
		description = QString("Moving to %1 on plate %2").arg(samplePositionName()).arg(samplePlateName());

	setShortDescription(description);
	setLongDescription(description);
}
