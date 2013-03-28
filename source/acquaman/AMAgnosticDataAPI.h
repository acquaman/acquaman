#ifndef AMAGNOSTICDATAAPI_H
#define AMAGNOSTICDATAAPI_H

#include <QMap>
#include <QVariant>
#include <QEvent>

namespace AMAgnosticDataAPIDefinitions{
	enum MessageType{
		AxisStarted = 0,
		AxisFinished = 1,
		LoopIncremented = 2,
		DataAvailable = 3,
		ControlMoved = 4,
		InvalidMessage = 5
	};

	enum InputType{
		UniqueID = 0,
		NextLoopValue = 1,
		DetectorData = 2,
		DetectorDimensionalitySize = 3,
		DetectorDimensionalityName = 4,
		DetectorDimensionalityUnit = 5,
		ControlMovementType = 6,
		ControlMovementValue = 7,
		InvalidType = 8
	};

	enum EventType{
		MessageEvent = QEvent::User+1212
	};

	extern QMap<AMAgnosticDataAPIDefinitions::MessageType, QString> MessageTypeToNames_;
	extern QMap<AMAgnosticDataAPIDefinitions::InputType, QString> InputTypeToNames_;
	extern QMap<AMAgnosticDataAPIDefinitions::InputType, QString> InputTypeToValueTypes_;
	extern bool definitionsPopulated_;

	bool ensureDefinitionsPopulated();
	QString nameFromMessageType(AMAgnosticDataAPIDefinitions::MessageType messageType);
	QString nameFromInputType(AMAgnosticDataAPIDefinitions::InputType inputType);
	QString valueTypeFromInputtype(AMAgnosticDataAPIDefinitions::InputType inputType);
}

class AMAgnosticDataAPIMessage
{
public:
	/// Default constructor takes the messageType from the definitions enum and the unique identifier.
	AMAgnosticDataAPIMessage(AMAgnosticDataAPIDefinitions::MessageType messageType, const QString &uniqueID);

	AMAgnosticDataAPIMessage(const AMAgnosticDataAPIMessage &original);

	AMAgnosticDataAPIMessage& operator=(const AMAgnosticDataAPIMessage &other);

	/// Returns the message type as its definition enum.
	AMAgnosticDataAPIDefinitions::MessageType messageType() const;

	/// Returns the unique identifier string
	QString uniqueID() const;

	/// Returns the value for the given key. If no such key exists a string with "INVALIDKEY" is returned.
	QVariant value(const QString &key) const;

	QVariantMap JSONData() const;

	/// Returns the value map as a printed json object
	QString toJSON() const;

	/// Sets the value associated with the key. If the key is already present, the value is overwritten. If the key is not present, it is added.
	void setValue(const QString &key, QVariant &value);

protected:
	/// Holds the definition enum for the message type.
	AMAgnosticDataAPIDefinitions::MessageType messageType_;

	/// Holds the mapping of keys to values including the key "message" as the given message type's equivalent string.
	QVariantMap jsonData_;
};

class AMAgnosticDataAPIStartAxisMessage : public AMAgnosticDataAPIMessage
{
public:
	/// Constructs a "Axis Started" message with the given initial values
	AMAgnosticDataAPIStartAxisMessage(const QString &uniqueID);
};

class AMAgnosticDataAPIFinishAxisMessage : public AMAgnosticDataAPIMessage
{
public:
	/// Constructs a "Axis Finished" message with the given initial values
	AMAgnosticDataAPIFinishAxisMessage(const QString &uniqueID);
};

class AMAgnosticDataAPILoopIncrementMessage : public AMAgnosticDataAPIMessage
{
public:
	/// Constructs a "Loop Incremented" message with the given initial values
	AMAgnosticDataAPILoopIncrementMessage(const QString &uniqueID, int nextLoopIncrement);

	/// Returns the next loop incrememnt value. Returns -1 if the value is somehow invalid.
	int nextLoopIncrement() const;

	/// Sets the next loop increment value
	void setNextLoopIncrement(int nextLoopIncrement);
};

class AMAgnosticDataAPIDataAvailableMessage : public AMAgnosticDataAPIMessage
{
public:
	/// Constructs a "Data Available" message with the given initial values
	AMAgnosticDataAPIDataAvailableMessage(const QString &uniqueID, QList<double> detectorData, QList<int> detectorDimensionalitySizes, QStringList detectorDimensionalityNames, QStringList detectorDimensionalityUnits);

	/// Returns the detector data as a list of doubles. Returns an empty list if the value is somehow invalid.
	QList<double> detectorData() const;

	/// Returns the detector's list of dimension sizes as a list of ints. Returns an empty list if the value is somehow invalid.
	QList<int> detectorDimensionalitySizes() const;

	/// Returns the detector's list of dimension names as a list of string. Returns an empty list if the value is somehow invalid.
	QStringList detectorDimensionalityNames() const;

	/// Returns the detector's list of dimension units as a list of string. Returns an empty list if the value is somehow invalid.
	QStringList detectorDimensionalityUnits() const;

	/// Sets the detector data from a list of doubles.
	void setDetectorData(QList<double> detectorData);

	/// Sets the detector's list of dimension sizes from a list of ints.
	void setDetectorDimensionalitySizes(QList<int> detectorDimensionalitySizes);

	/// Sets the detector's list of dimension names from a list of strings.
	void setDetectorDimensionalityNames(QStringList detectorDimensionalityNames);

	/// Sets the detector's list of dimension units from a list of strings.
	void setDetectorDimensionalityUnits(QStringList detectorDimensionalityUnits);
};

class AMAgnosticDataAPIControlMovedMessage : public AMAgnosticDataAPIMessage
{
public:
	/// Constructs a "Control Moved" message with the given initial values
	AMAgnosticDataAPIControlMovedMessage(const QString &uniqueID, const QString &movementType, double movementValue);

	/// Returns the control's movement type as a string. If the value is invalid a string containing "INVALIDMOVEMENTTYPE" is returned
	QString controlMovementType() const;

	/// Returns the control's movement value as a double. If the value is invalid 0 is returned.
	double controlMovementValue() const;

	/// Sets the control movement type to the given string
	void setControlMovementType(const QString &movementType);

	/// Sets the control movement value to the given double
	void setControlMovementValue(double movementValue);
};

class AMAgnositicDataEvent : public QEvent
{
public:
	AMAgnositicDataEvent() : QEvent( (QEvent::Type)AMAgnosticDataAPIDefinitions::MessageEvent), message_(AMAgnosticDataAPIDefinitions::InvalidMessage, "AutoGeneratedKey")
	{}

	AMAgnosticDataAPIMessage message_;
};

#include <QObject>

class AMAgnosticDataMessageHandler : public QObject
{
Q_OBJECT
public:
	AMAgnosticDataMessageHandler(QObject *parent = 0);

	virtual void postMessage(const AMAgnosticDataAPIMessage &message) = 0;
};

class AMAgnosticDataMessageQEventHandler : public AMAgnosticDataMessageHandler
{
Q_OBJECT
public:
	AMAgnosticDataMessageQEventHandler(QObject *parent = 0);

	bool addReceiver(QObject *receiver);
	bool removeReceiver(QObject *receiver);

	virtual void postMessage(const AMAgnosticDataAPIMessage &message);

protected:
	QList<QObject*> receivers_;
};

namespace AMAgnosticDataAPISupport{
	extern QHash<QString, AMAgnosticDataMessageHandler*> registeredHandlers_;

	bool registerHandler(const QString &lookupKey, AMAgnosticDataMessageHandler *messageHandler);

	const QHash<QString, AMAgnosticDataMessageHandler*> registeredHandlers();

	AMAgnosticDataMessageHandler* handlerFromLookupKey(const QString &lookupKey);
}

#endif // AMAGNOSTICDATAAPI_H
