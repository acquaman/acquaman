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


#include "AMAgnosticDataAPI.h"

#include <QStringList>

#include "source/qjson/serializer.h"
#include "source/qjson/parser.h"

namespace AMAgnosticDataAPIDefinitions{
	QMap<AMAgnosticDataAPIDefinitions::MessageType, QString> MessageTypeToNames_;
	QMap<AMAgnosticDataAPIDefinitions::InputType, QString> InputTypeToNames_;
	QMap<AMAgnosticDataAPIDefinitions::InputType, QString> InputTypeToValueTypes_;
	bool definitionsPopulated_ = false;

	bool ensureDefinitionsPopulated(){
		MessageTypeToNames_.insert(AMAgnosticDataAPIDefinitions::AxisStarted, "AxisStarted");
		MessageTypeToNames_.insert(AMAgnosticDataAPIDefinitions::AxisFinished, "AxisFinished");
		MessageTypeToNames_.insert(AMAgnosticDataAPIDefinitions::AxisValueFinished, "AxisValueFinished");
		MessageTypeToNames_.insert(AMAgnosticDataAPIDefinitions::DataAvailable, "DataAvailable");
		MessageTypeToNames_.insert(AMAgnosticDataAPIDefinitions::ControlMoved, "ControlMoved");
		MessageTypeToNames_.insert(AMAgnosticDataAPIDefinitions::InvalidMessage, "INVALIDMESSAGE");

		InputTypeToNames_.insert(AMAgnosticDataAPIDefinitions::UniqueID, "UniqueID");
		InputTypeToNames_.insert(AMAgnosticDataAPIDefinitions::DetectorData, "DetectorData");
		InputTypeToNames_.insert(AMAgnosticDataAPIDefinitions::DetectorDimensionalitySize, "DetectorDimensionalitySize");
		InputTypeToNames_.insert(AMAgnosticDataAPIDefinitions::DetectorDimensionalityName, "DetectorDimensionalityName");
		InputTypeToNames_.insert(AMAgnosticDataAPIDefinitions::DetectorDimensionalityUnit, "DetectorDimensionalityUnit");
		InputTypeToNames_.insert(AMAgnosticDataAPIDefinitions::ControlMovementType, "ControlMovementType");
		InputTypeToNames_.insert(AMAgnosticDataAPIDefinitions::ControlMovementValue, "ControlMovementValue");
		InputTypeToNames_.insert(AMAgnosticDataAPIDefinitions::ControlMovementFeedback, "ControlMovementFeedback");
		InputTypeToNames_.insert(AMAgnosticDataAPIDefinitions::DetectorUsesAMDS, "DetectorUsesAMDS");
		InputTypeToNames_.insert(AMAgnosticDataAPIDefinitions::DetectorDataAsAMDS, "DetectorDataAsAMDS");
		InputTypeToNames_.insert(AMAgnosticDataAPIDefinitions::InvalidType, "INVALIDINPUT");

		InputTypeToValueTypes_.insert(AMAgnosticDataAPIDefinitions::UniqueID, "QString");
		InputTypeToValueTypes_.insert(AMAgnosticDataAPIDefinitions::DetectorData, "QList<double>");
		InputTypeToValueTypes_.insert(AMAgnosticDataAPIDefinitions::DetectorDimensionalitySize, "QList<int>");
		InputTypeToValueTypes_.insert(AMAgnosticDataAPIDefinitions::DetectorDimensionalityName, "QList<QString>");
		InputTypeToValueTypes_.insert(AMAgnosticDataAPIDefinitions::DetectorDimensionalityUnit, "QList<QString>");
		InputTypeToValueTypes_.insert(AMAgnosticDataAPIDefinitions::ControlMovementType, "QString");
		InputTypeToValueTypes_.insert(AMAgnosticDataAPIDefinitions::ControlMovementValue, "double");
		InputTypeToValueTypes_.insert(AMAgnosticDataAPIDefinitions::ControlMovementFeedback, "double");
		InputTypeToValueTypes_.insert(AMAgnosticDataAPIDefinitions::DetectorUsesAMDS, "bool");
		InputTypeToValueTypes_.insert(AMAgnosticDataAPIDefinitions::DetectorDataAsAMDS, "quint64");
		InputTypeToValueTypes_.insert(AMAgnosticDataAPIDefinitions::InvalidType, "INVALID");

		definitionsPopulated_ = true;
		return true;
	}

	QString nameFromMessageType(AMAgnosticDataAPIDefinitions::MessageType messageType){
		if(!definitionsPopulated_)
			ensureDefinitionsPopulated();
		if(MessageTypeToNames_.contains(messageType))
			return MessageTypeToNames_.value(messageType);
		return MessageTypeToNames_.value(AMAgnosticDataAPIDefinitions::InvalidMessage);
	}

	QString nameFromInputType(AMAgnosticDataAPIDefinitions::InputType inputType){
		if(!definitionsPopulated_)
			ensureDefinitionsPopulated();
		if(InputTypeToNames_.contains(inputType))
			return InputTypeToNames_.value(inputType);
		return InputTypeToNames_.value(AMAgnosticDataAPIDefinitions::InvalidType);
	}

	QString valueTypeFromInputtype(AMAgnosticDataAPIDefinitions::InputType inputType){
		if(!definitionsPopulated_)
			ensureDefinitionsPopulated();
		if(InputTypeToValueTypes_.contains(inputType))
			return InputTypeToValueTypes_.value(inputType);
		return InputTypeToValueTypes_.value(AMAgnosticDataAPIDefinitions::InvalidType);
	}
}

 AMAgnosticDataAPIMessage::~AMAgnosticDataAPIMessage(){}
AMAgnosticDataAPIMessage::AMAgnosticDataAPIMessage(AMAgnosticDataAPIDefinitions::MessageType messageType, const QString &uniqueID)
{
	jsonData_["message"] = AMAgnosticDataAPIDefinitions::nameFromMessageType(messageType);
	jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::UniqueID)] = uniqueID;

	messageType_ = messageType;
	switch(messageType){
	case AMAgnosticDataAPIDefinitions::InvalidMessage:
		break;
	case AMAgnosticDataAPIDefinitions::AxisStarted:
		break;
	case AMAgnosticDataAPIDefinitions::AxisFinished:
		break;
	case AMAgnosticDataAPIDefinitions::AxisValueFinished:
		break;
	case AMAgnosticDataAPIDefinitions::DataAvailable:{
		QVariantList uninitializedDetectorData;
		uninitializedDetectorData << -1 << -1 << -1;
		jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorData)] = uninitializedDetectorData;
		QVariantList uninitializedDetectorSizes;
		uninitializedDetectorSizes << 0 << 0 << 0;
		jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorDimensionalitySize)] = uninitializedDetectorSizes;
		QVariantList uninitializedDetectorNames;
		uninitializedDetectorNames << "UNINITIALIZEDNAME0" << "UNINITIALIZEDNAME1" << "UNINITIALIZEDNAME2";
		jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorDimensionalityName)] = uninitializedDetectorNames;
		QVariantList uninitializedDetectorUnits;
		uninitializedDetectorUnits << "UNINITIALIZEDUNIT0" << "UNINITIALIZEDUNIT1" << "UNINITIALIZEDUNIT2";
		jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorDimensionalityUnit)] = uninitializedDetectorUnits;
		jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorUsesAMDS)] = false;
		jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorDataAsAMDS)] = 0;
		break;}
	case AMAgnosticDataAPIDefinitions::ControlMoved:
		jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::ControlMovementType)] = "UNINITIALIZEDMOVEMENTTYPE";
		jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::ControlMovementValue)] = 0;
		jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::ControlMovementFeedback)] = 0;
		break;
	}
}

AMAgnosticDataAPIMessage::AMAgnosticDataAPIMessage(const AMAgnosticDataAPIMessage &original)
{
	messageType_ = original.messageType();
	jsonData_ = original.jsonData_;
}

AMAgnosticDataAPIDefinitions::MessageType AMAgnosticDataAPIMessage::messageType() const{
	return messageType_;
}

AMAgnosticDataAPIMessage& AMAgnosticDataAPIMessage::operator =(const AMAgnosticDataAPIMessage &other){
	if(this != &other){
		messageType_ = other.messageType();
		jsonData_ = other.JSONData();
	}

	return *this;
}

QString AMAgnosticDataAPIMessage::uniqueID() const{
	return jsonData_.value(AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::UniqueID)).toString();
}

QVariant AMAgnosticDataAPIMessage::value(const QString &key) const{
	if(jsonData_.contains(key))
		return jsonData_.value(key);
	return "INVALIDKEY";
}

QVariantMap AMAgnosticDataAPIMessage::JSONData() const{
	return jsonData_;
}

QString AMAgnosticDataAPIMessage::toJSON() const{
	QJson::Serializer jserializer;
	QByteArray jsonPrint = jserializer.serialize(jsonData_);
	QString retVal;
	retVal = QString(jsonPrint);
	return retVal;
}

void AMAgnosticDataAPIMessage::setValue(const QString &key, QVariant &value){
	jsonData_.insert(key, value);
}


AMAgnosticDataAPIStartAxisMessage::~AMAgnosticDataAPIStartAxisMessage(){}

AMAgnosticDataAPIStartAxisMessage::AMAgnosticDataAPIStartAxisMessage(const QString &uniqueID) :
	AMAgnosticDataAPIMessage(AMAgnosticDataAPIDefinitions::AxisStarted, uniqueID)
{

}

AMAgnosticDataAPIFinishAxisMessage::~AMAgnosticDataAPIFinishAxisMessage(){}

AMAgnosticDataAPIFinishAxisMessage::AMAgnosticDataAPIFinishAxisMessage(const QString &uniqueID) :
	AMAgnosticDataAPIMessage(AMAgnosticDataAPIDefinitions::AxisFinished, uniqueID)
{

}

AMAgnosticDataAPIAxisValueFinishedMessage::~AMAgnosticDataAPIAxisValueFinishedMessage(){}

AMAgnosticDataAPIAxisValueFinishedMessage::AMAgnosticDataAPIAxisValueFinishedMessage(const QString &uniqueID) :
	AMAgnosticDataAPIMessage(AMAgnosticDataAPIDefinitions::AxisValueFinished, uniqueID)
{

}

AMAgnosticDataAPIDataAvailableMessage::~AMAgnosticDataAPIDataAvailableMessage(){}

AMAgnosticDataAPIDataAvailableMessage::AMAgnosticDataAPIDataAvailableMessage(const QString &uniqueID, QList<double> detectorData, QList<int> detectorDimensionalitySizes, QStringList detectorDimensionalityNames, QStringList detectorDimensionalityUnits, bool detectorUsesAMDS) :
	AMAgnosticDataAPIMessage(AMAgnosticDataAPIDefinitions::DataAvailable, uniqueID)
{
	setDetectorData(detectorData);
	setDetectorDimensionalitySizes(detectorDimensionalitySizes);
	setDetectorDimensionalityNames(detectorDimensionalityNames);
	setDetectorDimensionalityUnits(detectorDimensionalityUnits);
	setDetectorUsesAMDS(detectorUsesAMDS);
	setDetectorDataAsAMDS(0);
}

QList<double> AMAgnosticDataAPIDataAvailableMessage::detectorData() const{
	QList<double> retVal;
	QVariantList detectorDataValues = jsonData_.value(AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorData)).toList();

	bool oneConversion;
	bool successfulConversion = true;
	for(int x = 0; x < detectorDataValues.count(); x++){
		retVal.append(detectorDataValues.at(x).toDouble(&oneConversion));
		successfulConversion = successfulConversion && oneConversion;
	}

	if(!successfulConversion)
		retVal.clear();
	return retVal;
}

QList<int> AMAgnosticDataAPIDataAvailableMessage::detectorDimensionalitySizes() const{
	QList<int> retVal;
	QVariantList detectorDimensionalitySizes = jsonData_.value(AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorDimensionalitySize)).toList();

	bool oneConversion;
	bool successfulConversion = true;
	for(int x = 0; x < detectorDimensionalitySizes.count(); x++){
		retVal.append(detectorDimensionalitySizes.at(x).toInt(&oneConversion));
		successfulConversion = successfulConversion && oneConversion;
	}

	if(!successfulConversion)
		retVal.clear();
	return retVal;
}

QStringList AMAgnosticDataAPIDataAvailableMessage::detectorDimensionalityNames() const{
	QStringList retVal;
	QVariantList detectorDimensionalityNames = jsonData_.value(AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorDimensionalityName)).toList();

	for(int x = 0; x < detectorDimensionalityNames.count(); x++)
		retVal.append(detectorDimensionalityNames.at(x).toString());
	return retVal;
}

QStringList AMAgnosticDataAPIDataAvailableMessage::detectorDimensionalityUnits() const{
	QStringList retVal;
	QVariantList detectorDimensionalityUnits = jsonData_.value(AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorDimensionalityUnit)).toList();

	for(int x = 0; x < detectorDimensionalityUnits.count(); x++)
		retVal.append(detectorDimensionalityUnits.at(x).toString());
	return retVal;
}

bool AMAgnosticDataAPIDataAvailableMessage::detectorUsesAMDS() const{
	QVariant detectorUsesAMDSVariant = jsonData_.value(AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorUsesAMDS));
	return detectorUsesAMDSVariant.toBool();
}

quint64 AMAgnosticDataAPIDataAvailableMessage::detectorDataAsAMDS() const{
	if(detectorUsesAMDS()){
		QVariant detectorDataAsAMDSVariant = jsonData_.value(AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorDataAsAMDS));
		quint64 retVal;
		bool successfulConversion = false;
		retVal = detectorDataAsAMDSVariant.toULongLong(&successfulConversion);
		if(!successfulConversion)
			return 0;
		return retVal;
	}

	return 0;
}

void AMAgnosticDataAPIDataAvailableMessage::setDetectorData(QList<double> detectorData){
	QVariantList initializedDetectorData;
	for(int x = 0; x < detectorData.count(); x++)
		initializedDetectorData.append(detectorData.at(x));
	jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorData)] = initializedDetectorData;
}

void AMAgnosticDataAPIDataAvailableMessage::setDetectorDimensionalitySizes(QList<int> detectorDimensionalitySizes){
	QVariantList initializedDetectorSizes;
	for(int x = 0; x < detectorDimensionalitySizes.count(); x++)
		initializedDetectorSizes.append(detectorDimensionalitySizes.at(x));
	jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorDimensionalitySize)] = initializedDetectorSizes;
}

void AMAgnosticDataAPIDataAvailableMessage::setDetectorDimensionalityNames(QStringList detectorDimensionalityNames){
	QVariantList initializedDetectorNames;
	for(int x = 0; x < detectorDimensionalityNames.count(); x++)
		initializedDetectorNames.append(detectorDimensionalityNames.at(x));
	jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorDimensionalityName)] = initializedDetectorNames;
}

void AMAgnosticDataAPIDataAvailableMessage::setDetectorDimensionalityUnits(QStringList detectorDimensionalityUnits){
	QVariantList initializedDetectorUnits;
	for(int x = 0; x < detectorDimensionalityUnits.count(); x++)
		initializedDetectorUnits.append(detectorDimensionalityUnits.at(x));
	jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorDimensionalityUnit)] = initializedDetectorUnits;
}

void AMAgnosticDataAPIDataAvailableMessage::setDetectorUsesAMDS(bool usesAMDS){
	jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorUsesAMDS)] = usesAMDS;
}

void AMAgnosticDataAPIDataAvailableMessage::setDetectorDataAsAMDS(quint64 dataAsAMDS){
	if(detectorUsesAMDS()){
		jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorDataAsAMDS)] = dataAsAMDS;
	}
}

 AMAgnosticDataAPIControlMovedMessage::~AMAgnosticDataAPIControlMovedMessage(){}
AMAgnosticDataAPIControlMovedMessage::AMAgnosticDataAPIControlMovedMessage(const QString &uniqueID, const QString &movementType, double movementValue, double movementFeedback)
	: AMAgnosticDataAPIMessage(AMAgnosticDataAPIDefinitions::ControlMoved, uniqueID)
{
	setControlMovementType(movementType);
	setControlMovementValue(movementValue);
	setControlMovementFeedback(movementFeedback);
}

QString AMAgnosticDataAPIControlMovedMessage::controlMovementType() const{
	QVariant controlMovementTypeVariant = jsonData_.value(AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::ControlMovementType));
	QString retVal;
	bool validMovementType = true;
	retVal = controlMovementTypeVariant.toString();
	// FIGURE THIS OUT
	if(!validMovementType)
		return "INVALIDMOVEMENTTYPE";
	return retVal;
}

double AMAgnosticDataAPIControlMovedMessage::controlMovementValue() const{
	QVariant controlMovementValueVariant = jsonData_.value(AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::ControlMovementValue));
	double retVal;
	bool successfulConversion = false;
	retVal = controlMovementValueVariant.toDouble(&successfulConversion);
	if(!successfulConversion)
		return 0;
	return retVal;
}

double AMAgnosticDataAPIControlMovedMessage::controlMovementFeedback() const{
	QVariant controlMovementFeedbackVariant = jsonData_.value(AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::ControlMovementFeedback));
	double retVal;
	bool successfulConversion = false;
	retVal = controlMovementFeedbackVariant.toDouble(&successfulConversion);
	if(!successfulConversion)
		return 0;
	return retVal;
}

void AMAgnosticDataAPIControlMovedMessage::setControlMovementType(const QString &movementType){
	jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::ControlMovementType)] = movementType;
}

void AMAgnosticDataAPIControlMovedMessage::setControlMovementValue(double movementValue){
	jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::ControlMovementValue)] = movementValue;
}

void AMAgnosticDataAPIControlMovedMessage::setControlMovementFeedback(double movementFeedback){
	jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::ControlMovementFeedback)] = movementFeedback;
}

 AMAgnosticDataMessageHandler::~AMAgnosticDataMessageHandler(){}
AMAgnosticDataMessageHandler::AMAgnosticDataMessageHandler(QObject *parent) :
	QObject(parent)
{

}

 AMAgnosticDataMessageQEventHandler::~AMAgnosticDataMessageQEventHandler(){}
AMAgnosticDataMessageQEventHandler::AMAgnosticDataMessageQEventHandler(QObject *parent) :
	AMAgnosticDataMessageHandler(parent)
{

}

bool AMAgnosticDataMessageQEventHandler::addReceiver(QObject *receiver){
	if(!receiver || receivers_.contains(receiver))
		return false;
	receivers_.append(receiver);
	return true;
}

bool AMAgnosticDataMessageQEventHandler::removeReceiver(QObject *receiver){
	if(!receiver || !receivers_.contains(receiver))
		return false;
	receivers_.removeAll(receiver);
	return true;
}

#include <QCoreApplication>
void AMAgnosticDataMessageQEventHandler::postMessage(const AMAgnosticDataAPIMessage &message){
	AMAgnositicDataEvent *dataEvent;

	for(int x = 0; x < receivers_.count(); x++){
		dataEvent = new AMAgnositicDataEvent();
		dataEvent->message_ = message;
		QCoreApplication::postEvent(receivers_.at(x), dataEvent);
	}
}

namespace AMAgnosticDataAPISupport{
	QHash<QString, AMAgnosticDataMessageHandler*> registeredHandlers_;

	bool registerHandler(const QString &lookupKey, AMAgnosticDataMessageHandler *messageHandler){
		if(registeredHandlers_.contains(lookupKey))
			return false;

		registeredHandlers_.insert(lookupKey, messageHandler);
		return true;
	}

	const QHash<QString, AMAgnosticDataMessageHandler*> registeredHandlers(){
		return registeredHandlers_;
	}

	AMAgnosticDataMessageHandler* handlerFromLookupKey(const QString &lookupKey){
		if(registeredHandlers_.contains(lookupKey))
			return registeredHandlers_.value(lookupKey);
		return 0;
	}
}
 AMAgnositicDataEvent::~AMAgnositicDataEvent(){}
