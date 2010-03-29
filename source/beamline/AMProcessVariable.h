#ifndef ACQMAN_PROCESSVARIABLE_H_
#define ACQMAN_PROCESSVARIABLE_H_

#include <cadef.h>

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QList>
#include <QStringList>
#include <QVector>
#include <QHash>
#include <QTimer>

#define EPICS_CA_CONN_TIMEOUT_MS 1000

///////////////////////////////
// AMProcessVariableHeartbeat
///////////////////////////////

// Singleton support class for AMProcessVariables.  Manages global-context Channel Access business:
// 1) Calls ca_poll on a timer, and
// 2) Maintains a map between chid's and AMProcessVariable objects, so he can route exceptions.

// Automatically creates himself when needed, and deletes himself when no longer necessary...

#define PV_HEARTBEAT_MS 48

class AMProcessVariable;

class AMProcessVariableHeartbeat : public QObject {

	Q_OBJECT

public:

	// AMProcessVariables must call this FIRST in their constructor.
	// Ensures that channel access is initialized, timer is running, etc.
	static void ensureChannelAccess() { getInstance(); }

	// Once they have a chid, they should call this to make sure they receive exceptions routed properly:
	static void registerPV(chid c, AMProcessVariable* pv) { getInstance()->map_.insert(int(c), pv); }

	// AMProcessVariables must call this in their destructor.
	static void removePV(chid c) { getInstance()->removePVImplementation(c); }

	// This is the exception handler:
	static void PVExceptionCB(struct exception_handler_args args);

protected:

	// constructor: starts the ca_poll timer and installs us as the global exception handler.
	AMProcessVariableHeartbeat();

	// standard singleton-pattern getInstance() method.
	static AMProcessVariableHeartbeat* getInstance();

	// the implementation of AMProcessVariableHeartbeat::removePV():
	void removePVImplementation(chid c);

	// Timer event handler: calls ca_poll()
	void timerEvent(QTimerEvent*) { ca_poll(); }



	static AMProcessVariableHeartbeat* instance_;
	QHash<int, AMProcessVariable*> map_;
	int timerId_;

};




class AMProcessVariable : public QObject {

	Q_OBJECT

public:

	/// Process variables (PVs) might represent one of 4 meaningful data types for the programmer.  The type is determined automatically when the PV connects, and available from dataType().  Regardless of the inherent data type, you can set the value from any data type (see setValue(double), setValue(int), setValue(const QString&).
	// Internally, these match the corresponding epics chtype DBF_XXXX values)
	enum PVDataType { Unconnected = TYPENOTCONN, Integer = DBF_LONG, Enum = DBF_ENUM, FloatingPoint = DBF_DOUBLE, String = DBF_STRING };


	friend class AMProcessVariableHeartbeat;

	/// Constructor: pvName is the process variable name.  If autoMonitor = true, we'll start monitoring the channel as soon as we connect.  If you want to be notified if we fail to connect within a certain time, connectionTimeoutMs lets you set the timeout.  Default is one second, defined in EPICS_CA_CONN_TIMEOUT_MS.
	AMProcessVariable(const QString& pvName, bool autoMonitor = 0, QObject* parent = 0, int connectionTimeoutMs = EPICS_CA_CONN_TIMEOUT_MS);

	/// Destructor frees all resources, especially the automatically-managed creation of one AMProcessVariableHeartbeat whenever there are AMProcessVariables in use.
	virtual ~AMProcessVariable();

	/// Some PVs provide a whole array of values.  This is the number of elements in the array.
	unsigned count() const { return ca_element_count(chid_); }

	/// The name of this process variable:
	QString pvName() const { return QString(ca_name(chid_)); }

	/// Provides detailed information on the status of this connection.  Usually isConnected() is all you need.
	enum channel_state connectionState() const { return ca_state(chid_); }

	/// Indicates that a connection is established to the Epics CA server for this Process Variable.
	bool isConnected() const { return ca_state(chid_) == cs_conn; }

	/// Checks read access ability. (Verifies also that we are connected, since reading is impossible if not.)
	bool canRead() const { return isConnected() && ca_read_access(chid_); }
	/// Checks write access ability. (Verifies also that we are connected, since writing is impossible if not.)
	bool canWrite() const { return isConnected() && ca_write_access(chid_); }

	/// Read the most recent value of the PV.
	/*! (The Return value is a double, since this can hold any numeric value. See getInt, getDouble, and getString for other options.)  If you are monitoring (isMonitoring() == true), then the value is as recent as the last monitor notification received from the CA server.  Otherwise, it's as recent as the last time you called requestValue().  requestValue() is also called once when the connection is first established, so if you're using an AMProcessVariable to only read a value once, you don't have to explicitly start monitoring.*/
	double lastValue(unsigned index = 0) const;

	/// Returns the most recent array values of the PV. (Will be empty unless this PV's dataType() is Integer or Enum)
	// This is fast because it doesn't require a memory copy, thanks to Qt's implicit sharing on QVectors and other container types
	QVector<int> lastIntegerValues() const { return data_int_; }
	/// Returns the most recent array values of the PV. (Will be empty unless this PV's dataType() is FloatingPoint)
	QVector<double> lastFloatingPointValues() const { return data_dbl_; }

	/// error reporting: returns the last error code that occurred:
	int lastError() const { return lastError_; }
	/// Returns a string explanation of a particular error code:
	static QString errorString(int errorCode) { return QString(ca_message(errorCode)); }

	// For compatibility with QEpicsConnect:
	////////////////////////////////////

	/// These functions retrieve the most recent value (or for array PVs, the most recent value at a given index). All three are valid regardless of the inherent dataType() of the channel.
	int getInt(unsigned index = 0) const;
	double getDouble(unsigned index = 0) const { return lastValue(index); }
	/// Provides a string representation of the data.  Rather useful for handling many situations: It provides messages if the array index is out of range, or if the process variable isn't connected.  For numeric dataType()s (Integer and FloatingPoint), it returns a string representation of them.  For Enum types, it will return the string name for the option when the string names are available, otherwise it gives a string version of the integer value.
	QString getString(unsigned index = 0) const;

	/// Provided for name-compatibility with QEpicsConnect:
	bool isValid() const { return isConnected(); }
	/// Provided for name-compatibility with QEpicsConnect:
	int getStatus() const { return lastError(); }

public slots:

	/// asynchronous request to read (numberOfValues) values from the server:
	bool requestValue(int numberOfValues = 1);

	/// start monitoring on this channel:
	bool startMonitoring();
	/// stop monitoring this channel
	void stopMonitoring();

	/// Request a new setpoint.  Overloaded for different data types.
	void setValue(int);
	/// Request a new setpoint for 'num' array values.
	void setValues(dbr_long_t[], int num);
	void setValue(double);
	void setValues(dbr_double_t[], int num);
	void setValue(const QString&);
	void setValues(const QStringList& values);

	////////////////////////////////////
public:
	/// These functions provide detailed information about the channel. (The are only valid after the initialized() signal. )
	/// Provides the units for this PV, as known to epics.
	QString units() const { return units_; }

	/// Provides the recommended number of decimal places for displaying this (numeric) PV
	int displayPrecision() const  { return precision_; }

	/// Provides the minimum and maximum (driven) range of this PV, as enforced by EPICS:
	double upperControlLimit() const { return upperLimit_; }
	double lowerControlLimit() const { return lowerLimit_; }
	/// Provides the recommended graphical limits of this PV:
	double upperGraphicalLimit() const { return upperGraphLimit_; }
	double lowerGraphicalLimit() const { return lowerGraphLimit_; }


	/// All PV dataTypes() except for String can always be retrieved as numbers. However, some integer types are used to provide a list of choices. For example, a grating selector might let you choose Grating #1, 2, 3.  For PVs of this nature, isEnum() will be true.  It's just a convenient way of checking that dataType() == AMProcessVariable::Enum.
	bool isEnum() const { return (ourType_ == Enum); }
	/// For Enum dataType()s, sometimes a list of descriptions are provided for each numeric option.  For example, the gratings might be described as "LEG", "MEG", and "HEG".  enumStrings() will give you these titles.
	QStringList enumStrings() const { return enumStrings_; }
	/// Provides the number of choices for an Enum ProcessVariable:
	unsigned enumCount() const { return enumStrings_.count(); }

	// ignoring alarms for now:
	// double upperAlarmValue() const { return ctrlValue_.upper_alarm_limit; }
	// double lowerAlarmValue() const { return ctrlValue_.lower_alarm_limit; }

	/// This indicates the simplified data type that best represents this ProcessVariable. (Either Integer, Enum, FloatingPoint, or String.) Returns Unconnected == -1 if we haven't figured it out yet.
	PVDataType dataType() const { return ourType_; }

signals:
	/// Emits connected(true) when connection is established; connected(false) when lost.
	void connected(bool);
	/// Convenience: same signal without the argument, if you only care about one situation.  connected() goes with connected(true); disconnected() == connected(false);
	void connected();
	void disconnected();

	/// connection status changed. The new state is channel_state. Provides more detail than connected(bool).
	void connectionStateChanged(enum channel_state);

	/// PV failed to connect within the timeout
	void connectionTimeout();

	/// Connection was established, and control information was accessed. A good indicator that everything is ready to go.
	void initialized();

	// TODO. Alarms neglected for now. Never emitted.
	// void alarm(dbr_short_t severity, dbr_short_t reason);

	/// An error occurred... this is the error code:
	void error(int);
	/// Convenience: the same error() signal, with a string explanation
	void error(const QString&);

	/// These signals are emitted when new values arrive. valueChanged() always happens.  valueChanged(int) and valueChanged(double) are emitted for all numeric types.
	void valueChanged();
	void valueChanged(int);	// emitted for all numeric values
	void valueChanged(double); // emitted for all numeric values
	/// for performance reasons, valueChanged(string) is only emitted if the natural type is a string type or enum type.
	void valueChanged(const QString&);

	/// Emitted when a write-request comes back as completed or failed.
	void putRequestReturned(int status);

protected slots:
	/// Emitted when a connection timeout occurs (PV fails to connect)
	void onConnectionTimeout();

	/// these are simply here to generate the error(QString) and connected()/disconnected() signals, from error(int) and connected(bool)
	void signalForwardOnError(int errorCode) { emit error(errorString(errorCode)); }
	void signalForwardOnConnected(bool isConnected) { if(isConnected) emit connected(); else emit disconnected(); }

protected:

	/// Callbacks:
	void connectionChangedCB(struct connection_handler_args connArgs);	///< called when connection status changes. Emits connected()/disconnected().
	void valueChangedCB(struct event_handler_args eventArgs);		///< callled when the value changes.  Emits valueChanged().
	void controlInfoCB(struct event_handler_args eventArgs);		///< callled when control-information arives.  Emits initialized().
	void putRequestCB(struct event_handler_args eventArgs);				///< called when a put request returns. Emits putRequestReturned(int status).
	void exceptionCB(struct exception_handler_args exceptArgs);			///< called when an epics exception occurs.  Emits error(int status).

	/// Static wrapper functions are needed because we can't specify a member function as a callback. (Member functions don't have fixed permanent memory addresses.)  We carry the payload of which instance to call in the puser field of the event_handler_args.
	static void PVConnectionChangedCBWrapper(struct connection_handler_args connArgs);
	static void PVValueChangedCBWrapper(struct event_handler_args eventArgs);
	static void PVControlInfoCBWrapper(struct event_handler_args eventArgs);
	static void PVPutRequestCBWrapper(struct event_handler_args eventArgs);

	/// A convenience function to map an epics type (ex: from ca_field_type) to our simplified types:
	static PVDataType serverType2ourType(chtype serverType);

	/// channel ID for channel access
	chid chid_;
	/// Event ID for subscriptions (monitoring)
	evid evid_;

	// TODO: thread-safe protect with Mutex if using multi-threaded...
	static int channelReferenceCount_;	///< a global reference count used to clean up ca_context_destroy when last AMProcessVariable is destroyed.
	static AMProcessVariableHeartbeat* heartBeat_;	///< we create a single AMProcessVariableHeartbeat to call ca_poll while AMProcessVariables exist.

	/// Request that we start monitoring right away:
	bool shouldBeMonitoring_;
	/// true after the channel connects and we receive the control information:
	bool initialized_;

	/// Last error experienced.
	int lastError_;

	/// Stores control information:
	QString units_;
	int precision_;
	double upperLimit_, lowerLimit_;	// control (driven range) limits
	double upperGraphLimit_, lowerGraphLimit_; // visual graphing range
	QStringList enumStrings_;

	/// This timer used to detect connection timeouts:
	QTimer startupTimer_;

	/// Datatype used by the epics ioc:
	chtype serverType_;
	/// Datatype used by AMProcessVariable's memory storage:
	PVDataType ourType_;

	/// Our actual data storage: (only one of these will be used at a time, once we find out the channel type)
	QVector<double> data_dbl_;
	QVector<int> data_int_;
	QStringList data_str_;

};

#endif /*PROCESSVARIABLE_H_*/
