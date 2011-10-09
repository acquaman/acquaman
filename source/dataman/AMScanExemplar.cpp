#include "AMScanExemplar.h"

#include "acquaman/AMScanConfiguration.h"

AMScanExemplar::AMScanExemplar(QObject *parent) :
	QObject(parent)
{
	scanConfiguration_ = 0; //NULL
}

QString AMScanExemplar::name() const{
	return name_;
}

QString AMScanExemplar::technique() const{
	return technique_;
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
	return exportName();
}

AMScanExemplar& AMScanExemplar::operator =(const AMScanExemplar &other){
	if(this != &other){
		setName(other.name());
		setTechnique(other.technique());
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
	}
	return *this;
}

void AMScanExemplar::setName(const QString &name){
	name_ = name;
}

void AMScanExemplar::setTechnique(const QString &technique){
	technique_ = technique;
}

void AMScanExemplar::setDateTime(const QDateTime &dateTime){
	dateTime_ = dateTime;
}

void AMScanExemplar::setRunName(const QString &runName){
	runName_ = runName;
}

void AMScanExemplar::setRunStartDateTime(const QDateTime &runStartDateTime){
	runStartDateTime_ = runStartDateTime;
}

void AMScanExemplar::setRunEndDateTime(const QDateTime &runEndDateTime){
	runEndDateTime_ = runEndDateTime;
}

void AMScanExemplar::setFacilityName(const QString &facilityName){
	facilityName_ = facilityName;
}

void AMScanExemplar::setFacilityDescription(const QString &facilityDescription){
	facilityDescription_ = facilityDescription;
}

void AMScanExemplar::setScanConfiguration(const AMScanConfiguration *scanConfiguration){
	scanConfiguration_ = scanConfiguration;
}

void AMScanExemplar::setSampleName(const QString &sampleName){
	sampleName_ = sampleName;
}

void AMScanExemplar::setSampleElements(const QString &sampleElements){
	sampleElements_ = sampleElements;
}

void AMScanExemplar::setSampleDateTime(const QDateTime &sampleDateTime){
	sampleDateTime_ = sampleDateTime;
}

void AMScanExemplar::setExportName(const QString &exportName){
	exportName_ = exportName;
}
