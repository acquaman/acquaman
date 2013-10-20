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
		MessageTypeToNames_.insert(AMAgnosticDataAPIDefinitions::LoopIncremented, "LoopIncremented");
		MessageTypeToNames_.insert(AMAgnosticDataAPIDefinitions::DataAvailable, "DataAvailable");
		MessageTypeToNames_.insert(AMAgnosticDataAPIDefinitions::ControlMoved, "ControlMoved");
		MessageTypeToNames_.insert(AMAgnosticDataAPIDefinitions::InvalidMessage, "INVALIDMESSAGE");

		InputTypeToNames_.insert(AMAgnosticDataAPIDefinitions::UniqueID, "UniqueID");
		InputTypeToNames_.insert(AMAgnosticDataAPIDefinitions::NextLoopValue, "NextLoopValue");
		InputTypeToNames_.insert(AMAgnosticDataAPIDefinitions::DetectorData, "DetectorData");
		InputTypeToNames_.insert(AMAgnosticDataAPIDefinitions::DetectorDimensionalitySize, "DetectorDimensionalitySize");
		InputTypeToNames_.insert(AMAgnosticDataAPIDefinitions::DetectorDimensionalityName, "DetectorDimensionalityName");
		InputTypeToNames_.insert(AMAgnosticDataAPIDefinitions::DetectorDimensionalityUnit, "DetectorDimensionalityUnit");
		InputTypeToNames_.insert(AMAgnosticDataAPIDefinitions::ControlMovementType, "ControlMovementType");
		InputTypeToNames_.insert(AMAgnosticDataAPIDefinitions::ControlMovementValue, "ControlMovementValue");
		InputTypeToNames_.insert(AMAgnosticDataAPIDefinitions::InvalidType, "INVALIDINPUT");

		InputTypeToValueTypes_.insert(AMAgnosticDataAPIDefinitions::UniqueID, "QString");
		InputTypeToValueTypes_.insert(AMAgnosticDataAPIDefinitions::NextLoopValue, "int");
		InputTypeToValueTypes_.insert(AMAgnosticDataAPIDefinitions::DetectorData, "QList<double>");
		InputTypeToValueTypes_.insert(AMAgnosticDataAPIDefinitions::DetectorDimensionalitySize, "QList<int>");
		InputTypeToValueTypes_.insert(AMAgnosticDataAPIDefinitions::DetectorDimensionalityName, "QList<QString>");
		InputTypeToValueTypes_.insert(AMAgnosticDataAPIDefinitions::DetectorDimensionalityUnit, "QList<QString>");
		InputTypeToValueTypes_.insert(AMAgnosticDataAPIDefinitions::ControlMovementType, "QString");
		InputTypeToValueTypes_.insert(AMAgnosticDataAPIDefinitions::ControlMovementValue, "double");
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
	case AMAgnosticDataAPIDefinitions::LoopIncremented:
		jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::NextLoopValue)] = -1;
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
		break;}
	case AMAgnosticDataAPIDefinitions::ControlMoved:
		jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::ControlMovementType)] = "UNINITIALIZEDMOVEMENTTYPE";
		jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::ControlMovementValue)] = 0;
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


AMAgnosticDataAPIStartAxisMessage::AMAgnosticDataAPIStartAxisMessage(const QString &uniqueID) :
	AMAgnosticDataAPIMessage(AMAgnosticDataAPIDefinitions::AxisStarted, uniqueID)
{

}

AMAgnosticDataAPIFinishAxisMessage::AMAgnosticDataAPIFinishAxisMessage(const QString &uniqueID) :
	AMAgnosticDataAPIMessage(AMAgnosticDataAPIDefinitions::AxisFinished, uniqueID)
{

}

AMAgnosticDataAPILoopIncrementMessage::AMAgnosticDataAPILoopIncrementMessage(const QString &uniqueID, int nextLoopIncrement) :
	AMAgnosticDataAPIMessage(AMAgnosticDataAPIDefinitions::LoopIncremented, uniqueID)
{
	//jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::NextLoopValue)] = nextLoopIncrement;
	setNextLoopIncrement(nextLoopIncrement);
}

int AMAgnosticDataAPILoopIncrementMessage::nextLoopIncrement() const{
	QVariant nextLoopIncrementVariant = jsonData_.value(AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::NextLoopValue));
	int retVal;
	bool successfulConversion = false;
	retVal = nextLoopIncrementVariant.toInt(&successfulConversion);
	if(!successfulConversion)
		return -1;
	return retVal;
}

void AMAgnosticDataAPILoopIncrementMessage::setNextLoopIncrement(int nextLoopIncrement){
	jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::NextLoopValue)] = nextLoopIncrement;
}

AMAgnosticDataAPIDataAvailableMessage::AMAgnosticDataAPIDataAvailableMessage(const QString &uniqueID, QList<double> detectorData, QList<int> detectorDimensionalitySizes, QStringList detectorDimensionalityNames, QStringList detectorDimensionalityUnits) :
	AMAgnosticDataAPIMessage(AMAgnosticDataAPIDefinitions::DataAvailable, uniqueID)
{
	/*
	QVariantList initializedDetectorData;
	for(int x = 0; x < detectorData.count(); x++)
		initializedDetectorData.append(detectorData.at(x));
	jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorData)] = initializedDetectorData;

	QVariantList initializedDetectorSizes;
	for(int x = 0; x < detectorDimensionalitySizes.count(); x++)
		initializedDetectorSizes.append(detectorDimensionalitySizes.at(x));
	jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorDimensionalitySize)] = initializedDetectorSizes;

	QVariantList initializedDetectorNames;
	for(int x = 0; x < detectorDimensionalityNames.count(); x++)
		initializedDetectorNames.append(detectorDimensionalityNames.at(x));
	jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorDimensionalityName)] = initializedDetectorNames;

	QVariantList initializedDetectorUnits;
	for(int x = 0; x < detectorDimensionalityUnits.count(); x++)
		initializedDetectorUnits.append(detectorDimensionalityUnits.at(x));
	jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::DetectorDimensionalityUnit)] = initializedDetectorUnits;
	*/
	setDetectorData(detectorData);
	setDetectorDimensionalitySizes(detectorDimensionalitySizes);
	setDetectorDimensionalityNames(detectorDimensionalityNames);
	setDetectorDimensionalityUnits(detectorDimensionalityUnits);
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

AMAgnosticDataAPIControlMovedMessage::AMAgnosticDataAPIControlMovedMessage(const QString &uniqueID, const QString &movementType, double movementValue) :
	AMAgnosticDataAPIMessage(AMAgnosticDataAPIDefinitions::ControlMoved, uniqueID)
{
	/*
	jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::ControlMovementType)] = movementType;
	jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::ControlMovementValue)] = movementValue;
	*/
	setControlMovementType(movementType);
	setControlMovementValue(movementValue);
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

void AMAgnosticDataAPIControlMovedMessage::setControlMovementType(const QString &movementType){
	jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::ControlMovementType)] = movementType;
}

void AMAgnosticDataAPIControlMovedMessage::setControlMovementValue(double movementValue){
	jsonData_[AMAgnosticDataAPIDefinitions::nameFromInputType(AMAgnosticDataAPIDefinitions::ControlMovementValue)] = movementValue;
}

AMAgnosticDataMessageHandler::AMAgnosticDataMessageHandler(QObject *parent) :
	QObject(parent)
{

}

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
