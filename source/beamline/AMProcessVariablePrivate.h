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


#ifndef AMPROCESSVARIABLEPRIVATE_H
#define AMPROCESSVARIABLEPRIVATE_H


#include <cadef.h>

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QList>
#include <QStringList>
#include <QVector>
#include <QHash>
#include <QSet>
#include <QTimer>
#include <QMetaType>

#include "util/AMDeferredFunctionCall.h"

#define AMPROCESSVARIABLESUPPORT_STARTING_CHANNEL_ACCESS 113001
#define AMPROCESSVARIABLESUPPORT_SHUTTING_DOWN_CHANNEL_ACCESS_MESSAGE 113002
#define AMPROCESSVARIABLESUPPORT_EPICS_EXCEPTION_PVEXCEPTIONCB 113002
#define AMPROCESSVARIABLESUPPORT_ERROR_INITIALIZING_PRIVATE 113002
#define AMPROCESSVARIABLESUPPORT_CANNOT_ATTACH_ALREADY_ATTACHED 113002
#define AMPROCESSVARIABLESUPPORT_CANNOT_DETACH_NOT_ATTACHED 113002
#define AMPROCESSVARIABLESUPPORT_EPICS_EXCEPTION_EXCEPTIONCB 113002
#define AMPROCESSVARIABLESUPPORT_ERROR_WHILE_REQUESTING_ENUM_INFO 113002
#define AMPROCESSVARIABLESUPPORT_ERROR_WHILE_REQUESTING_INT_VALUE 113002
#define AMPROCESSVARIABLESUPPORT_ERROR_WHILE_REQUESTING_STRING_VALUE 113002
#define AMPROCESSVARIABLESUPPORT_ERROR_WHILE_REQUESTING_VALUE_GENERAL 113002
#define AMPROCESSVARIABLESUPPORT_ERROR_WHILE_SUBSCRIBING_TO_ALARMS 113002
#define AMPROCESSVARIABLESUPPORT_ERROR_WHILE_STARTING_MONITORING 113002
#define AMPROCESSVARIABLESUPPORT_ERROR_IN_CONTROL_INFO_CHANGED_CB 113002
#define AMPROCESSVARIABLESUPPORT_ERROR_IN_VALUE_CHANGED_CB 113002
#define AMPROCESSVARIABLESUPPORT_ERROR_IN_ALARM 113002
#define AMPROCESSVARIABLESUPPORT_ERROR_IN_PUT_REQUEST 113002
#define AMPROCESSVARIABLESUPPORT_ERROR_WHILE_STARTING_MONITOR_NOT_CONNECTED 113002
#define AMPROCESSVARIABLESUPPORT_ERROR_WHILE_STARTING_MONITOR_GENERAL 113002
#define AMPROCESSVARIABLESUPPORT_CANNOT_STOP_MONITORING_NEVER_STARTED 113002
#define AMPROCESSVARIABLESUPPORT_ERROR_WHILE_REQUESTING_VALUE 113002
#define AMPROCESSVARIABLESUPPORT_ERROR_WHILE_PUTTING_VALUE_INTEGER 113002
#define AMPROCESSVARIABLESUPPORT_ERROR_WHILE_PUTTING_VALUES_INTERGER 113002
#define AMPROCESSVARIABLESUPPORT_ERROR_WHILE_PUTTING_VALUE_DOUBLE 113002
#define AMPROCESSVARIABLESUPPORT_ERROR_WHILE_PUTTING_VALUES_DOUBLE 113002
#define AMPROCESSVARIABLESUPPORT_ERROR_WHILE_PUTTING_VALUE_STRING 113002
#define AMPROCESSVARIABLESUPPORT_ERROR_WHILE_PUTTING_VALUES_STRING 113002


/// Qt does not register QVector<int> and QVector<double> with qRegisterMetaType(), so we'll need to do this to use them in queued signal-slot connections.
typedef QVector<double> AMProcessVariableDoubleVector;
typedef QVector<int> AMProcessVariableIntVector;

Q_DECLARE_METATYPE(AMProcessVariableDoubleVector)
Q_DECLARE_METATYPE(AMProcessVariableIntVector)


///////////////////////////////
// AMProcessVariableSupport
///////////////////////////////

/// Process additional epics channel-access events every PV_HEARTBEAT_MS milliseconds
/*! Now that we're using the channel-access library's preemptive callback, we don't need to do this very quickly. However, some events (for ex: notifications of servers connecting or re-connecting) seem to be only delivered when requesting ca_poll.  We call ca_poll() every PV_HEARTBEAT_MS. */
#define PV_HEARTBEAT_MS 998

class AMProcessVariablePrivate;
class AMProcessVariable;

/// This class provides support to all AMProcessVariables, by setting up a channel-access context and handling exception delivery. You should never need to use it directly.
/*!
 Singleton support class for AMProcessVariable.  Manages global-context Channel Access business:
 -# Sets up the channel access environment
 -# Maintains a map between chid's and AMProcessVariablePrivate objects, to route exceptions.
 -# Maintains a map between PV names and AMProcessVariablePrivate instances, so that we can share one AMProcessVariablePrivate connection over several AMProcessVariables that all refer to the same PV.
 -# Calls ca_poll() every PV_HEARTBEAT_MS. This is not really necessary to do quickly because we are using the pre-emptive callback system, but it seems to be necessary for receiving notification of certain events (like channel access servers becoming available, etc.)

 Automatically creates itself when needed, and deletes itself when all channel access connections have been deleted.
  */
class AMProcessVariableSupport : public QObject {

	Q_OBJECT

public:

	/// Ensures that channel access environment is initialized. AMProcessVariables must call this FIRST in their constructor.
	static void ensureChannelAccess() { s(); }

	/// Once AMProcessVariablePrivate have a chid, they should call this to make sure they receive exceptions routed properly:
	static void registerPV(chid c, AMProcessVariablePrivate* pv) { s()->chid2Private_.insert(qint64(c), pv); }

	/// AMProcessVariablePrivates must call this in their destructor to de-register.
	static void removePV(chid c) { s()->removePVImplementation(c); }

	/// AMProcessVariable calls this to retrieve the AMProcessVariablePrivate for its connection instance. If there is no such connection yet, it will be created.
	static AMProcessVariablePrivate* getPrivateForPVName(const QString& pvName) { return s()->getPrivateForPVNameImplementation(pvName); }

	/// This is the global epics channel-access exception handler:
	static void PVExceptionCB(struct exception_handler_args args);

	/// Call this function to flush Channel-access requests to the network.
	/*! Because we are using the preemptive callback mode of the channel access library, we're not calling ca_poll on a timer anymore. This means that we must call ca_flush_io() after each network request if we want it go out immediately (otherwise it will be buffered until the output buffer is full).  However, calling ca_flush_io() after EACH (put, subscription request, etc.) is network-expensive, and good EPICS practices encourage you to buffer as much as possible.  This function will ensure that ca_flush_io() is called as soon as program control returns to the Qt event loop. Multiple calls to this function within one event loop cycle will result in a single ca_flush_io(), achieving the desired efficiency.
   */
	static void flushIO() {
		s()->flushIOCaller_.schedule();
	}

	/// Call this function to report that a timeout occured and AMProcessVariableSupport will schedule a deferred call to broadcast the error monitor
	static void reportTimeoutError(QString pvName);

	/// Destroys the namespace by ensuring the channel access is shut down properly.
	static void shutdownChannelAccess();

protected slots:
	/// Executes one call to ca_flush_io() for all the flushIO() requests that happened during the past event loop.
	void executeFlushIO() {
		ca_flush_io();
	}

	/// Handles the list of AMErrorMon timeout alerts
	void executeTimeoutError();

protected:

	/// constructor: sets up the channel access environement and installs us as the global exception handler.
	AMProcessVariableSupport();
	/// Destructor.  Shuts down channel access.
	virtual ~AMProcessVariableSupport();

	/// singleton class. Use the s() method to access.
	static AMProcessVariableSupport* s();

	/// Member function to implement AMProcessVariableSupport::removePV():
	void removePVImplementation(chid c);
	/// Member function to implement AMProcessVariableSupport::getPrivateForPVName().
	AMProcessVariablePrivate* getPrivateForPVNameImplementation(const QString& pvName);

	/// Timer event handler: calls ca_poll() every PV_HEARTBEAT_MS ms.
	void timerEvent(QTimerEvent*) { ca_poll(); }

	/// singleton class instance variable
	static AMProcessVariableSupport* instance_;
	/// Mapping from \c chid channel-id's to AMProcessVariablePrivate instances
	QHash<int, AMProcessVariablePrivate*> chid2Private_;
	/// Mapping from process variable names to AMProcessVariablePrivate instances
	QHash<QString, AMProcessVariablePrivate*> pvName2Private_;

	/// ID of the ca_poll timer
	int timerId_;
	/// This schedules and combines requests to flush channel-access IO a maximum of once every event loop.
	AMDeferredFunctionCall flushIOCaller_;

	/// This deferred call runs after 2.5 seconds to throw out the error monitors regarding PV connection timeout. This can be quite expensive if there's no connectivity on startup
	AMDeferredFunctionCall timeoutErrorFunctionCall_;
	/// Holds the list of pv names to report as timed out
	QStringList timeoutPVNames_;
};

namespace PVDataType {
/// Process variables (PVs) might represent one of 4 meaningful data types for the programmer.  The type is determined automatically when the PV connects, and available from dataType().  Regardless of the inherent data type, you can set the value from any data type (see setValue(double), setValue(int), setValue(const QString&).
/*! Internally, these match the corresponding epics \c chtype DBF_XXXX values defined in db_access.h)
  */
enum Type { Unconnected = TYPENOTCONN, ///< Type of PV's before they are connected. We don't know!
				  Integer = DBF_LONG,	///< Simplifies all integer types (short, long, char) to this
				  Enum = DBF_ENUM,		///< Represents integer type having discrete options/states.
				  FloatingPoint = DBF_DOUBLE, ///< Simplifies all floating-point types (double and float) to this
				  String = DBF_STRING		///< Used for records that can only be accessed as a string.
				};
}

/// Used by AMProcessVariable, this class encapsulates a connection to an EPICS channel-access Process Variable.  You should never need to use it directly; it exists to share a single channel-access connection between AMProcessVariable instances that all refer to the same underlying PV.
class AMProcessVariablePrivate : public QObject {

	Q_OBJECT

public:

	friend class AMProcessVariableSupport;

	/// Construct a connection for process variable named \c pvName.
	AMProcessVariablePrivate(const QString& pvName);

	/// Attach an AMProcessVariable \c pv to this instance. This will connect all the signals to be forwarded, and remember that we are "in use" by this AMProcessVariable.
	void attachProcessVariable(AMProcessVariable* pv);
	/// Un-attach an AMProcessVariable \c pv from this instance. This will disconnect all the forwarded signals, and delete this instance if it's no longer required (ie: has no other AMProcessVariables attached.)
	void detachProcessVariable(AMProcessVariable* pv);

	/// Destructor frees all resources, including releasing AMProcessVariableSupport when there are no more PVs in use.
	virtual ~AMProcessVariablePrivate();

	/// Some PVs provide a whole array of values.  This is the number of elements in the array.
	unsigned count() const { return count_; }

	/// The name of this process variable:
	QString pvName() const { return QString(ca_name(chid_)); } // ca_ functions are thread-safe by design; does not require locker

	/// Provides detailed information on the status of this connection.  Usually isConnected() is all you need, but this returns one of the channel_state enum values defined in <epics base>/include/cadef.h: {cs_never_conn = 0, cs_prev_conn, cs_conn, cs_closed}
	int connectionState() const {  return int(ca_state(chid_)); }

	/// Indicates that a connection is established to the Epics CA server for this Process Variable.
	bool isConnected() const {  return ca_state(chid_) == cs_conn; }
	/// Indicates that a connection was established to the Epics CA server, and we managed to download control information (meta information) for this Process Variable.
	bool isInitialized() const {  return initialized_; }
	/// Returns true if the connection has a monitoring subscription to receive value changes.
	bool isMonitoring() const { return (evid_ != 0); }
	/// Indicates that we've received the actual values for this PV at some point in history. (Note that isConnected() will be true as soon as a connection to the CA server is established, but we won't have the value yet when connected() gets emitted.)  valueChanged() will be emitted when the first value is received, but in case you're not watching that, you can call hasValues() to check if this has already happened.
	bool hasValues() const {  return hasValues_; }

	/// Checks read access permission. (Verifies also that we are connected, since reading is impossible if not.)
	bool canRead() const {  return isConnected() && ca_read_access(chid_); }
	/// Checks write access permission. (Verifies also that we are connected, since writing is impossible if not.)
	bool canWrite() const {  return isConnected() && ca_write_access(chid_); }

	/// Checks that we are completely ready to read (we are connected(), initialized(), have received at least one set of values, and we have read access permission)
	bool readReady() const { return canRead() && isInitialized() && hasValues(); }
	/// Check that we are completely readdy to write(we are connected(), initialized(), have received at least one set of values, and we have both read and write access permission)
	bool writeReady() const { return readReady() && canWrite(); }


	/// Returns true if the AMProcessVariable has been configured to use ca_put_callback() for PV puts, instead of ca_put().   [ca_put() is the default.]   \see enablePutCallback().
	bool putCallbackEnabled() const { return putCallbackEnabled_; }

	/// Read the most recent value of the PV.
	/*! (The Return value is a double, since this can hold any numeric value. See getInt, getDouble, and getString for other options.)  If you are monitoring (isMonitoring() == true), then the value is as recent as the last monitor notification received from the CA server.  Otherwise, it's as recent as the last time you called requestValue().  requestValue() is also called once when the connection is first established, so if you're using an AMProcessVariable to only read a value once, you don't have to explicitly start monitoring.*/
	double lastValue(unsigned index = 0) const;

	/// Returns the most recent array values of the PV. (Will be empty unless this PV's dataType() is Integer or Enum)
	/*! This is fast because it doesn't require a memory copy, thanks to Qt's implicit sharing on QVectors and other container types.*/
	QVector<int> lastIntegerValues() const {  return data_int_; }

	/// Returns the most recent array values of the PV. (Will be empty unless this PV's dataType() is FloatingPoint)
	/*! This is fast because it doesn't require a memory copy, thanks to Qt's implicit sharing on QVectors and other container types.*/
	QVector<double> lastFloatingPointValues() const {  return data_dbl_; }


	/// Returns the PV's Alarm Status.  All Process Variables automatically monitor changes to the alarm status and severity.
	/*! The initial value is 0 = NO_ALARM. Alarm status values are defined in <epics base>/include/alarm.h.

	\see alarmChanged()*/
	int alarmStatus() const { return alarmStatus_; }
	/// Returns the PV's Alarm Severity.  All Process Variables automatically monitor changes to the alarm status and severity.
	/*! The initial severity is 0 = NO_ALARM; others are Minor (1), Major (2), and Invalid (3). Alarm severity values are defined in <epics base>/include/alarm.h

	 \see alarmChanged()*/
	int alarmSeverity() const { return alarmSeverity_; }



	/// error reporting: returns the last error code that occurred:
	int lastError() const {  return lastError_; }
	/// Returns a string explanation of a particular error code:
	static QString errorString(int errorCode);

	/// \name Functions provided for backwards compatibility with QEpicsConnect:
	/// These functions retrieve the most recent value (or for array PVs, the most recent value at a given index).
	/*! All three are valid regardless of the inherent dataType() of the channel.  If the native datatype() is String, getInt() will attempt to convert the string to an integer.
  */
	//@{

	/// return the most recent value at \c index, as an integer.
	/*! This is valid regardless of the inherent dataType() of the channel.  If the native datatype() is String, getInt() will attempt to convert the string to an integer.
  If the index is out of range, will return -1.
  */
	int getInt(unsigned index = 0) const;

	/// return the most recent value at \c index, as a double.

	/*! This is valid regardless of the inherent dataType() of the channel.  If the native datatype() is String, getDouble() will attempt to convert the string to a double.
  If the index is out of range, will return -1.
  This is just a synonym for lastValue(int index).
  */
	double getDouble(unsigned index = 0) const { return lastValue(index); }

	/// Provides a string representation of the data at \c index.
	/*! Rather useful for handling many situations:

 - It provides messages if the array index is out of range, or if the process variable isn't connected.
 - For numeric dataType()s (Integer and FloatingPoint), it returns a string representation of them.
 - For Enum types, it will return the string name for the option when the string names are available, otherwise it gives a string version of the integer value.
 */
	QString getString(unsigned index = 0) const;

	/// Provided for name-compatibility with QEpicsConnect. Synonym for isConnected().
	bool isValid() const { return isConnected(); }
	/// Provided for name-compatibility with QEpicsConnect. Synonym for lastError().
	int getStatus() const { return lastError(); }
	//@}

	/// \name Control Group Information
	/// These functions provide detailed information about the channel. (The are only meaningful after the initialized() signal.)
	//@{
	/// Provides the units for this PV, as known to epics.
	QString units() const {  return units_; }

	/// Provides the recommended number of decimal places for displaying this (numeric) PV
	int displayPrecision() const  {  return precision_; }

	/// Provides the minimum and maximum (driven) range of this PV, as enforced by EPICS:
	double upperControlLimit() const {  return upperLimit_; }
	double lowerControlLimit() const {  return lowerLimit_; }
	/// Provides the recommended graphical limits of this PV:
	double upperGraphicalLimit() const {  return upperGraphLimit_; }
	double lowerGraphicalLimit() const {  return lowerGraphLimit_; }


	/// All PV dataTypes() except for String can always be retrieved as numbers. However, some integer types are used to provide a list of choices. For example, a grating selector might let you choose Grating #1, 2, 3.  For PVs of this nature, isEnum() will be true.  It's just a convenient way of checking that dataType() == AMProcessVariable::Enum.
	bool isEnum() const {  return (ourType_ == PVDataType::Enum); }
	/// For Enum dataType()s, sometimes a list of descriptions are provided for each numeric option.  For example, the gratings might be described as "LEG", "MEG", and "HEG".  enumStrings() will give you these titles.
	QStringList enumStrings() const {  return enumStrings_; }
	/// Provides the number of choices for an Enum ProcessVariable:
	unsigned enumCount() const {  return enumStrings_.count(); }
	//@}

	// ignoring alarms for now:
	// double upperAlarmValue() const { return ctrlValue_.upper_alarm_limit; }
	// double lowerAlarmValue() const { return ctrlValue_.lower_alarm_limit; }

	/// This indicates the simplified data type that best represents this ProcessVariable.
	/*! (Either Integer, Enum, FloatingPoint, or String.)

  Returns Unconnected == -1 if we haven't figured it out yet.
  */
	PVDataType::Type dataType() const {  return ourType_; }


public slots:

	/// asynchronous request to read (numberOfValues) values from the server:
	bool requestValue(int numberOfValues = 1);

	/// start monitoring on this channel. This should only be called when the channel is connected.
	void startMonitoring();
	/// stop monitoring this channel.
	void stopMonitoring();
	/// Check out whether our attached AMProcessVariables want monitoring, and set shouldBeMonitoring_ accordingly.  If connected and shouldBeMonitoring_ but is not, calls startMonitoring(). If monitoring and should not be, calls stopMonitoring().
	void reviewMonitoring();

	void setValue(int);
	void setValues(dbr_long_t[], int num);
	void setValue(double);
	void setValues(dbr_double_t[], int num);
	void setValue(const QString&);
	void setValues(const QStringList& values);

	/*! This function changes whether the the PV sets values through using ca_put() or ca_put_callback().  Generally ca_put_callback() is preferred since it returns debug messages after all process requests.  However, some of the more exotic record types in EPICS do not handle the ca_put_callback() effectively which causes the IOC to delay writing the value for a very long time (seconds per value).  An except from Jeff Hill about the differences between ca_put() and ca_put_callback():

  Description (IOC Database Specific)
  A ca put request causes the record to process if the record's SCAN field is set to passive, and the field being written has it's process passive attribute set to true. If such a record is already processing when a put request is initiated the specified field is written immediately, and the record is scheduled to process again as soon as it finishes processing. Earlier instances of multiple put requests initiated while the record is being processing may be discarded, but the last put request initiated is always written and processed.

  A ca put callback request causes the record to process if the record's SCAN field is set to passive, and the field being written has it's process passive attribute set to true. For such a record, the user's put callback function is not called until after the record, and any records that the record links to, finish processing. If such a record is already processing when a put callback request is initiated the put callback request is postponed until the record, and any records it links to, finish processing.
  If the record's SCAN field is not set to passive, or the field being written has it's process passive attribute set to false then the ca put or ca put callback request cause the specified field to be immediately written, but they do not cause the record to be processed.

  Setting this value to true will cause the PV to use ca_put_callback() instead of ca_put().
   */
	void enablePutCallback(bool putCallbackEnabled) { putCallbackEnabled_ = putCallbackEnabled; }

	////////////////////////////////////

signals:
	/// Emits connected(true) when connection is established; connected(false) when lost.
	void connected(bool);
	/// Convenience: same signal without the argument. Implies connected(true)
	void connected();
	/// Implies connected(false);
	void disconnected();


	/// connection status changed. The new state is \c channelState, which corresponds to the channel_state enum defined in <epics base>/include/cadef.h: {cs_never_conn = 0, cs_prev_conn, cs_conn, cs_closed}.   Provides more detail than connected(bool).
	void connectionStateChanged(int channelState);


	/// Connection was established, and control information was accessed. However, we may not have received the PV's current values yet.
	void initialized();
	/// Has just obtained the PV's values.
	void hasValuesChanged(bool);

	/// Signals changes in readReady().
	void readReadyChanged(bool);
	/// Signals changes in writeRead().
	void writeReadyChanged(bool);

	/// Signals when the alarmStatus() or alarmSeverity() changes.
	void alarmChanged(int status, int severity);

	/// An error occurred... this is the error code:
	void error(int);
	/// For convenience: the error() signal, but with a string explanation from errorString().
	void error(const QString&);

	/// \name Value Monitoring
	/// These signals are emitted when new values arrive.
	/*! valueChanged() always happens. valueChanged(int) and valueChanged(double) are emitted for all numeric types. For performance reasons, valueChanged(string) is only emitted if the natural type is a string type or enum type.
   */
	//@{
	void valueChanged();	///< emitted whenever a new value arrives
	void valueChanged(int);	///< emitted for all numeric values
	void valueChanged(double); ///< emitted for all numeric values
	/// for performance reasons, valueChanged(string) is only emitted if the natural type is a string type or enum type.
	void valueChanged(const QString&);
	//@}

	/// Emitted when a write-request comes back as completed or failed. Only available if enablePutCallback() has been set to true.
	void putRequestReturned(int status);

protected slots:

	/// Helper function to determine the status of readReady and writeReady: watches connected(), initialized(), and hasValuesChanged()
	void checkReadWriteReady();

	/// these are simply here to generate the error(QString) signals, from error(int)
	void signalForwardOnError(int errorCode) { emit error(errorString(errorCode)); }
	/// these are simply here to generate the connected()/disconnected() signals, from connected(bool)
	void signalForwardOnConnected(bool isConnected) { if(isConnected) emit connected(); else emit disconnected(); }

	// The following slots are used internally for thread-safe passing of values out of the callback functions (which my be called from other channel-access threads)
	void internal_onError(int lastError) {
		emit error(lastError_ = lastError);
	}
	void internal_onConnectionStateChanged(bool connected, int count, int serverType, int ourType);
	void internal_onControlInfoChanged(int controlInfoType, QString units, int precision, double upperGraphLimit, double lowerGraphLimit, double upperLimit, double lowerLimit, QStringList enumStrings);
	void internal_onFloatingPointValueChanged(AMProcessVariableDoubleVector doubleData);
	void internal_onIntegerValueChanged(AMProcessVariableIntVector intData);
	void internal_onEnumValueChanged(AMProcessVariableIntVector enumData);
	void internal_onStringValueChanged(QStringList stringData);
	void internal_onAlarmChanged(int status, int severity);

signals:
	// The following signals are used internally for thread-safe passing of values out of the callback functions (which my be called from other channel-access threads)
	void internal_error(int lastError);
	void internal_connectionStateChanged(bool connected, int count, int serverType, int ourType);
	void internal_controlInfoChanged(int controlInfoType, QString units, int precision, double upperGraphLimit, double lowerGraphLimit, double upperLimit, double lowerLimit, QStringList enumStrings);
	void internal_floatingPointValueChanged(AMProcessVariableDoubleVector doubleData);
	void internal_integerValueChanged(AMProcessVariableIntVector intData);
	void internal_enumValueChanged(AMProcessVariableIntVector enumData);
	void internal_stringValueChanged(QStringList stringData);
	void internal_alarmChanged(int status, int severity);

protected:

	/// \name Callbacks:
	/// These member functions are called by the channel access library (at any time, from any thread) to notify ProcessVariables of new data or connection-state changes.
	//@{
	void connectionChangedCB(struct connection_handler_args connArgs);	///< called when connection status changes. Emits connected()/disconnected().
	void valueChangedCB(struct event_handler_args eventArgs);		///< callled when the value changes.  Emits valueChanged().
	void alarmChangedCB(struct event_handler_args eventArgs);		///< callled when the alarm status changes.  Emits alarmChanged().
	void controlInfoCB(struct event_handler_args eventArgs);		///< callled when control-information arives.  Emits initialized().
	void putRequestCB(struct event_handler_args eventArgs);				///< called when a put request returns. Emits putRequestReturned(int status).
	void exceptionCB(struct exception_handler_args exceptArgs);			///< called when an epics exception occurs.  Emits error(int status).
	//@}

	/// \name Static wrapper functions
	/// These are needed because we can't specify a member function as a callback.
	/// (Member functions don't have fixed permanent memory addresses.)  We carry the payload of which instance to call in the puser field of the event_handler_args.)

	//@{
	/// on changes to connection status
	static void PVConnectionChangedCBWrapper(struct connection_handler_args connArgs);
	/// on receiving monitors or value updates
	static void PVValueChangedCBWrapper(struct event_handler_args eventArgs);
	/// on receiving alarm status changes
	static void PVAlarmChangedCBWrapper(struct event_handler_args eventArgs);
	/// on receiving control group information
	static void PVControlInfoCBWrapper(struct event_handler_args eventArgs);
	/// on receiving confirmation of put requests
	static void PVPutRequestCBWrapper(struct event_handler_args eventArgs);
	//@}


	/// Number of array elements (for array PVs)
	int count_;

	/// channel ID for channel access
	chid chid_;
	/// Event ID for subscriptions (monitoring values)
	evid evid_;
	/// Event ID for alarm subscription
	evid alarmEvid_;

	/// Request that we start monitoring as soon as we connect. (Set by main thread; read from epics connection callback thread, hence volatile.)
	volatile bool shouldBeMonitoring_;
	/// true after the channel connects and we receive the control information:
	bool initialized_;
	/// true after we receive the first value response
	bool hasValues_;
	/// last value for readReady, for watching for actual changes
	bool lastReadReady_;
	/// last value for writeReady, for watching for actual changes
	bool lastWriteReady_;

	/// Last error experienced.
	int lastError_;

	/// Sets up whether the PV should use ca_put() or ca_put_callback().  If true will use ca_put() as the calling function, other will use ca_put_callback().
	bool putCallbackEnabled_;

	/// \name Control Group Storage
	/// Storage of information on the PV's type and description.
	//@{
	QString units_;
	int precision_;
	double upperLimit_, lowerLimit_;	///< control (driven range) limits
	double upperGraphLimit_, lowerGraphLimit_; ///< visual graphing range
	QStringList enumStrings_;
	//@}

	/// Datatype used by the Epics IOC:
	chtype serverType_;
	/// Datatype used by AMProcessVariablePrivate's memory storage:
	PVDataType::Type ourType_;
	/// Datatype used to retrieve value+status structures corresponding to our type.
	chtype ourAlarmType_;

	//@{
	/// Our actual (read-accessible) data storage: (only one of these will be used at a time, once we find out the channel type)
	QVector<double> data_dbl_;
	QVector<int> data_int_;
	QStringList data_str_;
	//@}

	/// True almost always. False if the initial attempt to create the channel-access channel failed.
	bool channelCreated_;

	/// This is the set of attached AMProcessVariable instances that are using our connection
	QSet<AMProcessVariable*> attachedProcessVariables_;

	/// Records the PV's alarm status and alarm severity.
	int alarmStatus_, alarmSeverity_;
};

#endif // AMPROCESSVARIABLEPRIVATE_H
