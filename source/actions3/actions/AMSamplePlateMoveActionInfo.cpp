/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMSamplePlateMoveActionInfo.h"

#include "dataman/AMSamplePlate.h"
#include "util/AMErrorMonitor.h"

AMSamplePlateMoveActionInfo::AMSamplePlateMoveActionInfo(int samplePlateId, int samplePositionIndex, QObject *parent) :
	AMActionInfo3("Sample Move", "Sample Move", ":/system-run.png", parent)
{
	//samplePlateId_ = samplePlateId;
	//samplePositionId_ = samplePositionId;
	setSamplePlateId(samplePlateId);
	setSamplePositionIndex(samplePositionIndex);
	samplePosition_ = 0; //NULL
	updateDescriptionText();
}

AMSamplePlateMoveActionInfo::AMSamplePlateMoveActionInfo(const AMSamplePlateMoveActionInfo &other) :
	AMActionInfo3(other)
{
	//samplePlateId_ = other.samplePlateId();
	//samplePositionId_ = other.samplePositionId();
	setSamplePlateId(other.samplePlateId());
	setSamplePositionIndex(other.samplePositionIndex());
	samplePosition_ = 0; //NULL
}

AMActionInfo3* AMSamplePlateMoveActionInfo::createCopy() const{
	return new AMSamplePlateMoveActionInfo(*this);
}

QString AMSamplePlateMoveActionInfo::typeDescription() const{
	return "Sample Move";
}

int AMSamplePlateMoveActionInfo::samplePlateId() const{
	return samplePlateId_;
}

int AMSamplePlateMoveActionInfo::samplePositionIndex() const{
	return samplePositionIndex_;
}

const AMControlInfoList* AMSamplePlateMoveActionInfo::samplePosition(){
	if( (samplePlateId_ == -1) || (samplePositionIndex_ == -1) )
		return 0; //NULL

	AMSamplePlate samplePlate;
	if(!samplePlate.loadFromDb(AMDatabase::database("user"), samplePlateId_)){
		AMErrorMon::alert(this, AMSAMPLEPLATEMOVEACTIONINFO_INVALID_SAMPLEPLATE_ID, QString("Failed to generate sample move information, there is no plate with id %1 in the database").arg(samplePlateId_));
		//return samplePosition_;
		return 0; //NULL
	}

	int samplePositionId = -1;
	if(samplePositionIndex_ < samplePlate.count()){
		//samplePositionId = samplePlate.at(samplePositionIndex_).id();
		samplePositionId = samplePlate.at(samplePositionIndex_).position().id();
	}
	/*
	int indexOfSamplePosition = -1;
	for(int x = 0; x < samplePlate.count(); x++){
		if(samplePlate.at(x).id() == samplePositionIndex_){
			indexOfSamplePosition = x;
			break;
		}
	}
	*/
	if(samplePositionId < 0){
		AMErrorMon::alert(this, AMSAMPLEPLATEMOVEACTIONINFO_INVALID_SAMPLEPOSITION_INDEX, QString("Failed to generate sample move information, there is no position at index %1 on plate %2").arg(samplePositionIndex_).arg(samplePlateId_));
		//return samplePosition_;
		return 0; //NULL
	}

	samplePosition_ = new AMControlInfoList();
	if(!samplePosition_->loadFromDb(AMDatabase::database("user"), samplePositionId)){
		AMErrorMon::alert(this, AMSAMPLEPLATEMOVEACTIONINFO_INVALID_SAMPLEPOSITION_ID, QString("Failed to generate sample move information, there is no position with id %1 on plate %2").arg(samplePositionIndex_).arg(samplePlateId_));
		return 0; //NULL
	}
	return samplePosition_;
}

QString AMSamplePlateMoveActionInfo::samplePlateName() const{
	if(samplePlateId() == -1)
		return "No Plate Selected";

	AMSamplePlate samplePlate;
	if(!samplePlate.loadFromDb(AMDatabase::database("user"), samplePlateId_))
		return "Plate Error";

	return samplePlate.name();
}

QString AMSamplePlateMoveActionInfo::samplePositionName() const{
	if(samplePlateId() == -1)
		return "No Plate Selected";

	if(samplePositionIndex() < 0)
		return "No Sample Selected";

	AMSamplePlate samplePlate;
	if(!samplePlate.loadFromDb(AMDatabase::database("user"), samplePlateId_))
		return "Plate Error";

	if(samplePositionIndex() > samplePlate.count())
		return "Sample Index Error";

	AMSample actualSample;
	if(!actualSample.loadFromDb(AMDatabase::database("user"), samplePlate.at(samplePositionIndex()).sampleId()))
		return "Sample Error";

	return actualSample.name();
}

void AMSamplePlateMoveActionInfo::setSamplePlateId(int samplePlateId){
	if(samplePlateId_ != samplePlateId){
		samplePlateId_ = samplePlateId;
		samplePosition_ = 0; //NULL
		setModified(true);
		updateDescriptionText();
	}
}

void AMSamplePlateMoveActionInfo::setSamplePositionIndex(int samplePositionId){
	if(samplePositionIndex_ != samplePositionId){
		samplePositionIndex_ = samplePositionId;
		samplePosition_ = 0; //NULL
		setModified(true);
		updateDescriptionText();
	}
}

void AMSamplePlateMoveActionInfo::updateDescriptionText(){
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
