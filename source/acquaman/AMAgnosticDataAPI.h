#ifndef AMAGNOSTICDATAAPI_H
#define AMAGNOSTICDATAAPI_H

#include <QMap>
#include <QVariant>
#include <QEvent>

/// The namespace that defines many common messages used by the AMAgnosticDataAPI.
namespace AMAgnosticDataAPIDefinitions
{
	/// The message type enum.  Provides an easy way to know what kind of message is coming.
	enum MessageType
	{
		AxisStarted = 0,		///< Used to denote a new axis has been started.  In higher dimensional scans, this message would occur at the beginning of every scan loop.
		AxisFinished = 1,		///< Used to denote that the current axis has finished.  In higher dimensional scans, this message would appear at the end of every scan loop.
		LoopIncremented = 2,	///< Used to denote that the scan has stepped.
		DataAvailable = 3,		///< Provides the data for a single detector of any dimensionality.
		ControlMoved = 4,		///< Provides the data for a single control that was moved.
		InvalidMessage = 5		///< Catch all error message.
	};

	/// The input type enum.  Used for getting data out of the message.
	enum InputType
	{
		UniqueID = 0,						///< The unique identifier to ensure that there is no ambiguity on what the message is about.
		NextLoopValue = 1,					///< The next loop value.
		DetectorData = 2,					///< The data for the detector.  Can be almost anything since detectors have many configurations and dimensionalities.
		DetectorDimensionalitySize = 3,		///< The size of the detector.
		DetectorDimensionalityName = 4,		///< The name of the detector.
		DetectorDimensionalityUnit = 5,		///< The units of the detector.
		ControlMovementType = 6,			///< Which type of control was moved.
		ControlMovementValue = 7,			///< The value of the control, regardless of what type of control it is.
		ControlMovementFeedback = 8,		///< The feedback value of the control, regardless of what type of control it is.
		InvalidType = 9						///< Catch all error type.
	};

	/// The event type enum.  There is only one event type because there is only one message.
	enum EventType
	{
		MessageEvent = QEvent::User+1212		///< Defines the unique event type for messages.
	};

	/// The mapping between the MessageType definitions and the string used by json.
	extern QMap<AMAgnosticDataAPIDefinitions::MessageType, QString> MessageTypeToNames_;
	/// The mapping between the InputType to their respective names.
	extern QMap<AMAgnosticDataAPIDefinitions::InputType, QString> InputTypeToNames_;
	/// The mapping between the InputType and their respective data types (as a string).
	extern QMap<AMAgnosticDataAPIDefinitions::InputType, QString> InputTypeToValueTypes_;
	/// Flag used to know whether the all of the maps have been built.
	extern bool definitionsPopulated_;

	/// Builds all the maps between enums and strings.
	bool ensureDefinitionsPopulated();
	/// Returns the mapped message type based on the enum provided.
	QString nameFromMessageType(AMAgnosticDataAPIDefinitions::MessageType messageType);
	/// Returns the mapped input type name based on the enum provided.
	QString nameFromInputType(AMAgnosticDataAPIDefinitions::InputType inputType);
	/// Returns the mapped input type value based on the enum provided.
	QString valueTypeFromInputtype(AMAgnosticDataAPIDefinitions::InputType inputType);
}

class AMAgnosticDataAPIMessage
{
public:
	/// Default constructor takes the messageType from the definitions enum and the unique identifier.
	AMAgnosticDataAPIMessage(AMAgnosticDataAPIDefinitions::MessageType messageType, const QString &uniqueID);
	/// Copy constructor.
	AMAgnosticDataAPIMessage(const AMAgnosticDataAPIMessage &original);
	/// Destructor.
	virtual ~AMAgnosticDataAPIMessage();

	/// The equals operator.
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
	/// Destrutor.
	virtual ~AMAgnosticDataAPIStartAxisMessage();
};

class AMAgnosticDataAPIFinishAxisMessage : public AMAgnosticDataAPIMessage
{
public:
	/// Constructs a "Axis Finished" message with the given initial values
	AMAgnosticDataAPIFinishAxisMessage(const QString &uniqueID);
	/// Destructor.
	virtual ~AMAgnosticDataAPIFinishAxisMessage();
};

class AMAgnosticDataAPILoopIncrementMessage : public AMAgnosticDataAPIMessage
{
public:
	/// Constructs a "Loop Incremented" message with the given initial values
	AMAgnosticDataAPILoopIncrementMessage(const QString &uniqueID, int nextLoopIncrement);
	/// Destructor.
	virtual ~AMAgnosticDataAPILoopIncrementMessage();

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
	/// Destructor.
	virtual ~AMAgnosticDataAPIDataAvailableMessage();

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
	AMAgnosticDataAPIControlMovedMessage(const QString &uniqueID, const QString &movementType, double movementValue, double movementFeedback);
	/// Destructor.
	virtual ~AMAgnosticDataAPIControlMovedMessage();

	/// Returns the control's movement type as a string. If the value is invalid a string containing "INVALIDMOVEMENTTYPE" is returned
	QString controlMovementType() const;

	/// Returns the control's movement value as a double. If the value is invalid 0 is returned.
	double controlMovementValue() const;

	/// Returns the controls' movement feedback value as a double.  If the value is invalid, 0 is returned.
	double controlMovementFeedback() const;

	/// Sets the control movement type to the given string
	void setControlMovementType(const QString &movementType);

	/// Sets the control movement value to the given double
	void setControlMovementValue(double movementValue);

	/// Sets the control movement feedback value to given double.
	void setControlMovementFeedback(double movementFeedback);
};

class AMAgnositicDataEvent : public QEvent
{
public:
	AMAgnositicDataEvent() : QEvent( (QEvent::Type)AMAgnosticDataAPIDefinitions::MessageEvent), message_(AMAgnosticDataAPIDefinitions::InvalidMessage, "AutoGeneratedKey")
	{}
	virtual ~AMAgnositicDataEvent();

	AMAgnosticDataAPIMessage message_;
};

#include <QObject>

class AMAgnosticDataMessageHandler : public QObject
{
	Q_OBJECT

public:
	AMAgnosticDataMessageHandler(QObject *parent = 0);
	virtual ~AMAgnosticDataMessageHandler();

	virtual void postMessage(const AMAgnosticDataAPIMessage &message) = 0;
};

class AMAgnosticDataMessageQEventHandler : public AMAgnosticDataMessageHandler
{
	Q_OBJECT

public:
	AMAgnosticDataMessageQEventHandler(QObject *parent = 0);
	virtual ~AMAgnosticDataMessageQEventHandler();

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
