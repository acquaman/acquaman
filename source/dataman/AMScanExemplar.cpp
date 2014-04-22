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


#include "AMScanExemplar.h"

#include "acquaman/AMScanConfiguration.h"

 AMScanExemplar::~AMScanExemplar(){}
AMScanExemplar::AMScanExemplar(QObject *parent) :
	QObject(parent)
{
	name_ = "";
	number_ = "";
	dateTime_ = QDateTime::currentDateTime();
	runName_ = "";
	runStartDateTime_ = QDateTime::currentDateTime();
	runEndDateTime_ = QDateTime::currentDateTime();
	facilityName_ = "";
	facilityDescription_ = "";
	scanConfiguration_ = 0; //NULL
	sampleName_ = "";
	sampleElements_ = "";
	sampleDateTime_ = QDateTime::currentDateTime();
	exportName_ = "";
}

QString AMScanExemplar::name() const{
	return name_;
}

QString AMScanExemplar::technique() const{
	if(scanConfiguration())
		return scanConfiguration()->technique();
	return "";
}

QString AMScanExemplar::number() const{
	return number_;
}

QDateTime AMScanExemplar::dateTime() const{
	return dateTime_;
}

QString AMScanExemplar::runName() const{
	return runName_;
}

QDateTime AMScanExemplar::runStartDateTime() const{
	return runStartDateTime_;
}

QDateTime AMScanExemplar::runEndDateTime() const{
	return runEndDateTime_;
}

QString AMScanExemplar::facilityName() const{
	return facilityName_;
}

QString AMScanExemplar::facilityDescription() const{
	return facilityDescription_;
}

const AMScanConfiguration* AMScanExemplar::scanConfiguration() const{
	return scanConfiguration_;
}

QString AMScanExemplar::sampleName() const{
	return sampleName_;
}

QString AMScanExemplar::sampleElements() const{
	return sampleElements_;
}

QDateTime AMScanExemplar::sampleDateTime() const{
	return sampleDateTime_;
}

QString AMScanExemplar::exportName() const{
	return exportName_;
}

AMScanExemplar& AMScanExemplar::operator =(const AMScanExemplar &other){
	if(this != &other){
		setName(other.name());
		setDateTime(other.dateTime());
		setRunName(other.runName());
		setRunStartDateTime(other.runStartDateTime());
		setRunEndDateTime(other.runEndDateTime());
		setFacilityName(other.facilityName());
		setFacilityDescription(other.facilityDescription());
		setScanConfiguration(other.scanConfiguration());
		setSampleName(other.sampleName());
		setSampleElements(other.sampleElements());
		setSampleDateTime(other.sampleDateTime());
		setExportName(other.exportName());
		emit settingsChanged();
	}
	return *this;
}

void AMScanExemplar::setName(const QString &name){
	if(name_ != name){
		name_ = name;
		emit settingsChanged();
	}
}

void AMScanExemplar::setDateTime(const QDateTime &dateTime){
	if(dateTime_ != dateTime){
		dateTime_ = dateTime;
		emit settingsChanged();
	}
}

void AMScanExemplar::setRunName(const QString &runName){
	if(runName_ != runName){
		runName_ = runName;
		emit settingsChanged();
	}
}

void AMScanExemplar::setRunStartDateTime(const QDateTime &runStartDateTime){
	if(runStartDateTime_ != runStartDateTime){
		runStartDateTime_ = runStartDateTime;
		emit settingsChanged();
	}
}

void AMScanExemplar::setRunEndDateTime(const QDateTime &runEndDateTime){
	if(runEndDateTime_ != runEndDateTime){
		runEndDateTime_ = runEndDateTime;
		emit settingsChanged();
	}
}

void AMScanExemplar::setFacilityName(const QString &facilityName){
	if(facilityName_ != facilityName){
		facilityName_ = facilityName;
		emit settingsChanged();
	}
}

void AMScanExemplar::setFacilityDescription(const QString &facilityDescription){
	if(facilityDescription_ != facilityDescription){
		facilityDescription_ = facilityDescription;
		emit settingsChanged();
	}
}

void AMScanExemplar::setScanConfiguration(const AMScanConfiguration *scanConfiguration){
	if(scanConfiguration_ != scanConfiguration){
		scanConfiguration_ = scanConfiguration;
		emit settingsChanged();
	}
}

void AMScanExemplar::setSampleName(const QString &sampleName){
	if(sampleName_ != sampleName){
		sampleName_ = sampleName;
		emit settingsChanged();
	}
}

void AMScanExemplar::setSampleElements(const QString &sampleElements){
	if(sampleElements_ != sampleElements){
		sampleElements_ = sampleElements;
		emit settingsChanged();
	}
}

void AMScanExemplar::setSampleDateTime(const QDateTime &sampleDateTime){
	if(sampleDateTime_ != sampleDateTime){
		sampleDateTime_ = sampleDateTime;
		emit settingsChanged();
	}
}

void AMScanExemplar::setExportName(const QString &exportName){
	if(exportName_ != exportName){
		exportName_ = exportName;
		emit settingsChanged();
	}
}
