#ifndef ACQMAN_PROCESSVARIABLE_H_
#define ACQMAN_PROCESSVARIABLE_H_

#include <cadef.h>

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QList>
#include <QStringList>
#include <QHash>
#include <QTimer>

#define EPICS_CA_CONN_TIMEOUT_MS 1000

///////////////////////////////
// ProcessVariableHeartbeat
///////////////////////////////

// Singleton support class for ProcessVariables.  Manages global-context Channel Access business:
// 1) Calls ca_poll on a timer, and
// 2) Maintains a map between chid's and ProcessVariable objects, so he can route exceptions.

// Automatically creates himself when needed, and deletes himself when no longer necessary...

#define PV_HEARTBEAT_MS 48

class ProcessVariable;

class ProcessVariableHeartbeat : public QObject {
	
	Q_OBJECT
	
public:
	
	// ProcessVariables must call this FIRST in their constructor.
	// Ensures that channel access is initialized, timer is running, etc.
	static void ensureChannelAccess() { getInstance(); }
	
	// Once they have a chid, they should call this to make sure they receive exceptions routed properly:
	static void registerPV(chid c, ProcessVariable* pv) { getInstance()->map_.insert(int(c), pv); }
	
	// ProcessVariables must call this in their destructor.
	static void removePV(chid c) { getInstance()->removePVImplementation(c); }
	
	// This is the exception handler: 
	static void PVExceptionCB(struct exception_handler_args args);
	
protected:

	// constructor: starts the ca_poll timer and installs us as the global exception handler.
	ProcessVariableHeartbeat();
	
	// standard singleton-pattern getInstance() method.
	static ProcessVariableHeartbeat* getInstance();
	
	// the implementation of ProcessVariableHeartbeat::removePV():
	void removePVImplementation(chid c);
	
	// Timer event handler: calls ca_poll()
	void timerEvent(QTimerEvent*) { ca_poll(); }
	
	

	static ProcessVariableHeartbeat* instance_;
	QHash<int, ProcessVariable*> map_;
	int timerId_;

};




class ProcessVariable : public QObject {
	
	Q_OBJECT
	
public:
	friend class ProcessVariableHeartbeat;

	ProcessVariable(const QString& pvName, bool autoMonitor = 0, QObject* parent = 0, int connectionTimeoutMs = EPICS_CA_CONN_TIMEOUT_MS);
	virtual ~ProcessVariable();	
	
	// access functions:
	unsigned numElements() { return ca_element_count(chid_); }
	QString pvName() { return QString(ca_name(chid_)); }
	enum channel_state connectionState() { return ca_state(chid_); }
	bool isConnected() { return ca_state(chid_) == cs_conn; }
	
	// check read/write access ability (verifies also that we are connected, since reading is impossible if not)
	bool canRead() { return isConnected() && ca_read_access(chid_); }
	bool canWrite() { return isConnected() && ca_write_access(chid_); }
	
	
	// error reporting:
	int lastError() { return lastError_; }
	static QString errorString(int errorCode) { return QString(ca_message(errorCode)); }
	
	// asynchronous request to read the value:
	bool requestValue(int num = 1);
	
	// start / stop monitoring:
	bool startMonitoring();
	void stopMonitoring();

	// For compatibility with QEpicsConnect:
	////////////////////////////////////

	virtual int getInt(unsigned idx = 0) = 0;
	virtual double getDouble(unsigned idx = 0) = 0;
	virtual QString getString(unsigned idx = 0) = 0;

	bool isValid() { return isConnected(); }
	int getStatus() { return lastError(); }

public slots:
	virtual void setValue(int);
	virtual void setValues(dbr_long_t[], int num);
	virtual void setValue(double);
	virtual void setValues(dbr_double_t[], int num);
	virtual void setValue(const QString&);
	virtual void setValues(const QStringList& values);

	////////////////////////////////////

	// More information: (Only valid after initialized() signal. )
	QString units() { return QString(ctrlValue_.units); }
	double displayPrecision() { return ctrlValue_.precision; }
	double upperAlarmValue() { return ctrlValue_.upper_alarm_limit; }
	double lowerAlarmValue() { return ctrlValue_.lower_alarm_limit; }
	double upperControlLimit() { return ctrlValue_.upper_ctrl_limit; }
	double lowerControlLimit() { return ctrlValue_.lower_ctrl_limit; }

signals:
	void connected(bool);
		// Convenience:
		void connected();
		void disconnected();

	void connectionStateChanged(enum channel_state);
	void connectionTimeout();
	void initialized();			// connection was established, and control information was accessed. A good indicator that everything is ready to go.

	void alarm(dbr_short_t severity, dbr_short_t reason);	// TODO
	void error(int);
		// Convenience:
		void error(const QString&);

	void valueChanged();

	void putRequestReturned(int status);

protected slots:
	virtual void onConnectionTimeout();

	// these are simply here to generate the error(QString) and connected()/disconnected() signals, from error(int) and connected(bool)
	void signalForwardOnError(int errorCode) { emit error(errorString(errorCode)); }
	void signalForwardOnConnected(bool isConnected) { if(isConnected) emit connected(); else emit disconnected(); }

protected:

	// Callbacks:
	virtual void connectionChangedCB(struct connection_handler_args connArgs);	// called when connection status changes. Emits connected()/disconnected().
	virtual void valueChangedCB(struct event_handler_args eventArgs) = 0;		// callled when the value changes.  Emits valueChanged(T).
	virtual void putRequestCB(struct event_handler_args eventArgs);				// called when a put request returns. Emits putRequestReturned(int status).
	virtual void exceptionCB(struct exception_handler_args exceptArgs);			// called when an epics exception occurs.  Emits error(int status).

	static void PVConnectionChangedCBWrapper(struct connection_handler_args connArgs);
	static void PVValueChangedCBWrapper(struct event_handler_args eventArgs);
	static void PVPutRequestCBWrapper(struct event_handler_args eventArgs);

	chid chid_;	// channel ID
	evid evid_;	// event ID
	
	// TODO: thread-safe protect with Mutex
	static int channelReferenceCount_;	// a global reference count used to clean up ca_context_destroy when last ProcessVariable is destroyed.
	static ProcessVariableHeartbeat* heartBeat_;	// we create a single ProcessVariableHeartbeat to call ca_poll while ProcessVariables exist.
		
	bool shouldBeMonitoring_;
	
	int lastError_;
	struct dbr_ctrl_double ctrlValue_;
	
	virtual chtype dataType() = 0;
	QTimer startupTimer_;
	
};




// Implementations of ProcessVariables:

///////////////////////////////////
// IntProcessVariable
///////////////////////////////////

class IntProcessVariable : public ProcessVariable {

	Q_OBJECT
	
public:
	IntProcessVariable(const QString& pvName, bool autoMonitor = 0, QObject* parent = 0, int connectionTimeoutMs = EPICS_CA_CONN_TIMEOUT_MS);
	
	// read stored value (values... if array)
	int lastValue() { return values_[0]; }
	QList<int>& lastValues() { return values_; }

	virtual int getInt(unsigned idx = 0) { if( idx >= numElements() ) return 0; else return values_[idx]; }
	virtual double getDouble(unsigned idx = 0) { if( idx >= numElements() ) return 0; else return values_[idx]; }
	virtual QString getString(unsigned idx = 0) { if( idx >= numElements() ) return QString(""); else return QString("%1").arg(values_[idx]); }

	
protected:
	QList<int> values_;
	
	 virtual chtype dataType() { return DBR_LONG; }

	 virtual void valueChangedCB(struct event_handler_args eventArgs);			// called when the value changes
	
signals:
	void valueChanged(int newValue);		// emitted whenever the value is updated. For arrays, argument is just the first value.
	
};

///////////////////////////////////
// DoubleProcessVariable
///////////////////////////////////

class DoubleProcessVariable : public ProcessVariable {

	Q_OBJECT
	
public:
	DoubleProcessVariable(const QString& pvName, bool autoMonitor = 0, QObject* parent = 0, int connectionTimeoutMs = EPICS_CA_CONN_TIMEOUT_MS);
	
	// read stored value (values... if array)
	double lastValue() { return values_[0]; }
	QList<double>& lastValues() { return values_; }

	virtual int getInt(unsigned idx = 0) { if( idx >= numElements() ) return 0; else return values_[idx]; }
	virtual double getDouble(unsigned idx = 0) { if( idx >= numElements() ) return 0; else return values_[idx]; }
	virtual QString getString(unsigned idx = 0) { if( idx >= numElements() ) return QString(""); else return QString("%1").arg(values_[idx]); }

	
protected:
	QList<double> values_;
	
	 virtual chtype dataType() { return DBR_DOUBLE; }

	virtual void valueChangedCB(struct event_handler_args eventArgs);			// called when the value changes
	
signals:
	void valueChanged(double newValue);		// emitted whenever the value is updated. For arrays, argument is just the first value.
	
};

///////////////////////////////////
// StringProcessVariable
///////////////////////////////////

class StringProcessVariable : public ProcessVariable {

	Q_OBJECT
	
public:
	StringProcessVariable(const QString& pvName, bool autoMonitor = 0, QObject* parent = 0, int connectionTimeoutMs = EPICS_CA_CONN_TIMEOUT_MS);
	
	// read stored value (values... if array)
	QString lastValue() { return values_[0]; }
	QList<QString>& lastValues() { return values_; }


	virtual int getInt(unsigned idx = 0) { if( idx >= numElements() ) return 0; else return values_.at(idx).toInt(); }
	virtual double getDouble(unsigned idx = 0) { if( idx >= numElements() ) return 0; else return values_.at(idx).toDouble(); }
	virtual QString getString(unsigned idx = 0) { if( idx >= numElements() ) return QString(""); else return values_.at(idx); }
	
protected:
	QStringList values_;

	
	 virtual chtype dataType() { return DBR_STRING; }

	 virtual void valueChangedCB(struct event_handler_args eventArgs);			// called when the value changes
	
signals:
	void valueChanged(const QString& newValue);		// emitted whenever the value is updated. For arrays, argument is just the first value.
	
};


#endif /*PROCESSVARIABLE_H_*/
