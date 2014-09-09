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


#ifndef AM_PROCESSVARIABLE_H_
#define AM_PROCESSVARIABLE_H_

#include "beamline/AMProcessVariablePrivate.h"

#define AMPROCESSVARIABLE_CONNECTION_TIMED_OUT 112001

/**
  \addtogroup control
  @{
  */


/// This defines the default value for a channel-access search connection timeout, in milliseconds.  If a connection takes longer than this to establish, we'll keep on trying, but we'll issue the connectionTimeout() signal.
#define EPICS_CA_CONN_TIMEOUT_MS 3000

/// We use to set the EPICS_CA_MAX_ARRAY_BYTES environment variable before starting up channel access.  It's necessary to allow the transfer of large arrays as EPICS channel access data.  Here we set it to (2^12)*(2^12)*8, or enough room for a 4096 x 4096 2D image with doubles stored at each point.  If that's not enough for you, feel free to increase it.

#define AMPROCESSVARIABLE_MAX_CA_ARRAY_BYTES "134217728"

/// We use these to return additional errors from AMProcessVariable. These are in addition to the ones EPICS already defines in the range of 0 to 60 in caerr.h.
#define AMPROCESSVARIABLE_CANNOT_READ  270000
#define AMPROCESSVARIABLE_CANNOT_WRITE 270001


/// This class is used to connect to an Epics Process Variable (PV) using the Channel Access protocol.  The PV values can be read and written, and signals are provided for monitoring and alarm events.  Control information in the form of units, enum strings, etc. is also provided.
/*!
<b>Note on multiple AMProcessVariables that refer to the same PV</b>

If you create two or more AMProcessVariable instances that refer to the same PV, for network performance they will transparently share a single underlying Channel Access connection. One important caveat, for now, is that if ANY of those process variables want monitoring enabled, ALL of them will be effectively monitored, and emit valueChanged() updates.  Calling stopMonitoring() may do nothing, if another connection to the same PV wants monitoring enabled.*/
class AMProcessVariable : public QObject {
	Q_OBJECT
public:

	/// A function to map an Epics type (ex: from ca_field_type) to our simplified types in PVDataType::Type:
	static PVDataType::Type serverType2ourType(chtype serverType);
	/// A function to map an Epics type (ex: from ca_field_type) to the data structure we use for retrieving status (alarm) values: dbr_sts_<ourType>
	static chtype serverType2StatusType(chtype serverType);

	/// Constructor
	/*! \param pvName The process variable channel-access name.
  \param monitor If monitor = true, we'll start monitoring the channel for value changes as soon as we connect.
  \param connectionTimeoutMs Sets the timeout if you want to be notified if we fail to connect within a certain period.  Default is one second, defined in EPICS_CA_CONN_TIMEOUT_MS.
  \param parent QObject parent class pointer
  */
	AMProcessVariable(const QString& pvName, bool monitor = false, QObject* parent = 0, int connectionTimeoutMs = EPICS_CA_CONN_TIMEOUT_MS);

	/// Destructor
	/*! Notifies our AMProcessVariablePrivate that we don't need it anymore */
	virtual ~AMProcessVariable();

	// Basic PV Status Information:
	//////////////////////////////////

	/// Some PVs provide a whole array of values.  This is the number of elements in the array.
	unsigned count() const { return d_->count(); }

	/// The name of this process variable:
	QString pvName() const { return d_->pvName(); }

	/// Provides detailed information on the status of this connection.  Usually isConnected() is all you need, but this returns one of the values defined in the channel_state enum in <epics base>/include/cadef.h: {cs_never_conn = 0, cs_prev_conn, cs_conn, cs_closed}
	int connectionState() const { return d_->connectionState(); }
	/// Indicates that a connection is established to the Epics CA server for this Process Variable.
	bool isConnected() const { return d_->isConnected(); }
	/// Indicates that a connection was established to the Epics CA server, and we managed to download control information (units, limits, enum strings, etc.) for this Process Variable. Does not mean that we've retrieved the values yet.
	bool isInitialized() const { return d_->isInitialized(); }
	/// Returns true if the connection has a monitoring subscription to receive value changes. \see startMonitoring(), startMonitoring().
	bool isMonitoring() const { return d_->isMonitoring(); }
	/// Indicates that we've received the actual values for this PV at some point in history. (Note that isConnected() will be true as soon as a connection to the CA server is established, but we won't have the value yet when connected() gets emitted.)  valueChanged() will be emitted when the first value is received, but in case you're not watching that, you can call hasValues() to check if this has already happened.
	bool hasValues() const { return d_->hasValues(); }

	/// Checks read access permission. (Verifies also that we are connected, since reading is impossible if not.)
	bool canRead() const { return d_->canRead(); }
	/// Checks write access permission. (Verifies also that we are connected, since writing is impossible if not.)
	bool canWrite() const { return d_->canWrite(); }

	/// Checks that we are completely ready to read (we are connected, initialized, have received at least one set of values, and we have read access)
	bool readReady() const { return d_->readReady(); }
	/// Check that we are completely readdy to write(we are connected, initialized, have received at least one set of values, and we have both read and write access)
	bool writeReady() const { return d_->writeReady(); }

	/// Returns true if the AMProcessVariable has been configured to use ca_put_callback() for PV puts, instead of ca_put().   [ca_put() is the default.]   \see enablePutCallback().
	bool putCallbackEnabled() const { return d_->putCallbackEnabled(); }


	// Retrieving PV Values in various formats
	////////////////////////////////////////////

	/// Read the most recent value of the PV.
	/*! (The Return value is a double, since this can hold any numeric value. See getInt, getDouble, and getString for other options.)  If you are monitoring (isMonitoring() == true), then the value is as recent as the last monitor notification received from the CA server.  Otherwise, it's as recent as the last time you called requestValue().  requestValue() is also called once when the connection is first established, so if you're using an AMProcessVariable to only read a value once, you don't have to explicitly start monitoring.*/
	double lastValue(unsigned index = 0) const { return d_->lastValue(index); }

	/// Returns the most recent array values of the PV. (Will be empty unless this PV's dataType() is Integer or Enum)
	/*! This is fast because it doesn't require a memory copy, thanks to Qt's implicit sharing on QVectors and other container types.*/
	QVector<int> lastIntegerValues() const { return d_->lastIntegerValues(); }


	/// Returns the most recent array values of the PV. (Will be empty unless this PV's dataType() is FloatingPoint)
	/*! This is fast because it doesn't require a memory copy, thanks to Qt's implicit sharing on QVectors and other container types.*/
	QVector<double> lastFloatingPointValues() const { return d_->lastFloatingPointValues(); }


	/// Returns the PV's Alarm Status.  All Process Variables automatically monitor changes to the alarm status and severity.
	/*! The initial value is 0 = NO_ALARM. Alarm status values are defined in <epics base>/include/alarm.h.

	\see alarmChanged()*/
	int alarmStatus() const { return d_->alarmStatus(); }
	/// Returns the PV's Alarm Severity.  All Process Variables automatically monitor changes to the alarm status and severity.
	/*! The initial severity is 0 = NO_ALARM; others are Minor (1), Major (2), and Invalid (3). Alarm severity values are defined in <epics base>/include/alarm.h

	 \see alarmChanged()*/
	int alarmSeverity() const { return d_->alarmSeverity(); }


	/// \name Functions provided for backwards compatibility with QEpicsConnect:
	/// These functions retrieve the most recent value (or for array PVs, the most recent value at a given index).
	/*! All three are valid regardless of the inherent dataType() of the channel.  If the native datatype() is String, getInt() will attempt to convert the string to an integer.
  */
	//@{

	/// return the most recent value at \c index, as an integer.
	/*! This is valid regardless of the inherent dataType() of the channel.  If the native datatype() is String, getInt() will attempt to convert the string to an integer.
  If the index is out of range, will return -1.
  */
	int getInt(unsigned index = 0) const { return d_->getInt(index); }

	/// return the most recent value at \c index, as a double.
	/*! This is valid regardless of the inherent dataType() of the channel.  If the native datatype() is String, getDouble() will attempt to convert the string to a double.
  If the index is out of range, will return -1.
  This is just a synonym for lastValue(int index).
  */
	double getDouble(unsigned index = 0) const { return lastValue(index); }

	/// Provides a string representation of the data at \c index.
	/*! Rather useful for handling many situations:
 - It provides messages if the array index is out of range, or if the process variable isn't connected.
 - For numeric dataType()s (Integer and FloatingPoint), it returns a string representation.
 - For Enum types, it will return the string name for the option [when the string names are available, otherwise it gives a string version of the integer value].
 */
	QString getString(unsigned index = 0) const { return d_->getString(index); }

	/// Provided for name-compatibility with QEpicsConnect. Synonym for isConnected().
	bool isValid() const { return isConnected(); }
	/// Provided for name-compatibility with QEpicsConnect. Synonym for lastError().
	int getStatus() const { return lastError(); }
	//@}

	// Functions for simplifying array PVs into signal numbers
	//////////////////////////////////////////////////////////
	/// For array PVs, returns the sum of the values from lowIndex to highIndex (inclusive). If the lowIndex or highIndex is out of range, any valid subrange is returned
	int binIntegerValues(int lowIndex, int highIndex) const;
	/// For array PVs, returns the sum of the values from lowIndex to highIndex (inclusive). If the lowIndex or highIndex is out of range, any valid subrange is returned
	double binFloatingPointValues(int lowIndex, int highIndex) const;


	// Error Reporting
	/////////////////////

	/// error reporting: returns the last error code that occurred:
	int lastError() const { return d_->lastError(); }
	/// Returns a string explanation of a particular error code:
	static QString errorString(int errorCode) { return AMProcessVariablePrivate::errorString(errorCode); }


	// Control Group Information
	//////////////////////////////

	/// \name Control Group Information
	/// These functions provide detailed information about the channel. (The are only meaningful after the initialized() signal.)
	//@{
	/// Provides the units for this PV, as known to epics.
	QString units() const { return d_->units(); }

	/// Provides the recommended number of decimal places for displaying this (numeric) PV
	int displayPrecision() const { return d_->displayPrecision(); }

	/// Provides the minimum and maximum (driven) range of this PV, as enforced by EPICS:
	double upperControlLimit() const { return d_->upperControlLimit(); }
	/// Provides the minimum and maximum (driven) range of this PV, as enforced by EPICS:
	double lowerControlLimit() const { return d_->lowerControlLimit(); }
	/// Provides the recommended graphical limits of this PV:
	double upperGraphicalLimit() const { return d_->upperGraphicalLimit(); }
	/// Provides the recommended graphical limits of this PV:
	double lowerGraphicalLimit() const { return d_->lowerGraphicalLimit(); }


	/// All PV dataTypes() except for String can always be retrieved as numbers. However, some integer types are used to provide a list of choices. For example, a grating selector might let you choose "LEG", "MEG", "HEG", etc.  For PVs of this nature, isEnum() will be true.  This is just a convenient way of checking that dataType() == AMProcessVariable::Enum.
	bool isEnum() const { return d_->isEnum(); }
	/// For Enum dataType()s, sometimes a list of descriptions are provided for each numeric option.  For example, the gratings might be described as "LEG", "MEG", and "HEG".  enumStrings() will give you these titles.
	QStringList enumStrings() const { return d_->enumStrings(); }
	/// Returns the number of string choices for an Enum ProcessVariable:
	unsigned enumCount() const { return d_->enumCount(); }
	//@}

	/*
	 ignoring alarms for now:
	 double upperAlarmValue() const { return ctrlValue_.upper_alarm_limit; }
	 double lowerAlarmValue() const { return ctrlValue_.lower_alarm_limit; }
	 */


	/// This indicates the simplified data type that best represents this ProcessVariable.
	/*! (Either Integer, Enum, FloatingPoint, or String.)

  Returns Unconnected == -1 if we haven't figured it out yet.
  */
	PVDataType::Type dataType() const { return d_->dataType(); }


public slots:
	/// enable / disable the process the record (If DISV=DISA, then the record will be disabled).
	void enableProcessRecord() { d_->enableProcessRecord(); }
	void disableProcessRecord() { d_->disableProcessRecord(); }

	/// asynchronous request to read \c numberOfValues values from the server:
	bool requestValue(int numberOfValues = 1) { return d_->requestValue(numberOfValues); }

	/// start monitoring on this channel. If the channel is not yet connected, it will start monitoring when it does connect.
	void startMonitoring() { shouldBeMonitoring_ = true; d_->reviewMonitoring(); }
	/// stop monitoring this channel. \note If any other AMProcessVariables instances exist which connect to the same underlying PV, and they want monitoring enabled, then monitoring will continue. This is a restriction due to our automatic sharing of channel-access connections for network performance.
	void stopMonitoring() { shouldBeMonitoring_ = false; d_->reviewMonitoring(); }

	/// Request a new setpoint.
	void setValue(int i) { d_->setValue(i); }
	/// Request a new setpoint of 'num' integer values.
	void setValues(dbr_long_t values[], int num) { d_->setValues(values, num); }
	/// Request a new setpoint.
	void setValue(double d) { d_->setValue(d); }
	/// Request a new setpoint of \c num double values.
	void setValues(dbr_double_t values[], int num) { d_->setValues(values, num); }
	/// Request a new setpoint.
	void setValue(const QString& s) { d_->setValue(s); }
	/// Request a new setpoint of an array of string values.
	void setValues(const QStringList& values) { d_->setValues(values); }


	/// Configure whether the PV uses ca_put() or ca_put_callback() to send put requests.  ca_put() is the default.
	/*! Channel access provides two mechanisms for sending put requests: ca_put() and ca_put_callback().  ca_put() simply sends a put request and does no further work; generally we can assume the put request will arrive at the IOC if the channel is connected().

	  ca_put_callback() provides a more robust method, in which the IOC will respond after the record has been processed with the new value, triggering a callback in the client that sent the request. This is the most reliable way to ensure that a put request has been delivered and acted upon.  However, the ca_put_callback() mechanism triggers a change in IOC record processing from using dbPut to dbPutNotify; this becomes significant whenever put requests are being sent faster than the IOC can process the corresponding record [and any linked records that need to be processed along with the modified record].

- If ca_put() is sent while a record is still being processed (PACT=1), a re-processing of that record is scheduled to do a dbPut with the most recent value. If more ca_put()s are received before the re-processing happens, the IOC simply updates what value it will use when the re-processing finally does happens.
- If ca_put_callback() is sent while a record is still being processed, a queue is created of upcoming values to put, and all of these puts need to be processed individually. Additional ca_put_callback()s will append to this queue. Therefore, if ca_put_callback() is used faster than the record can be processed, a queue of ever-increasing length builds up. This can compounded if there are chains of linked records that all need to be processed for each put.

In summary, during record processing, ca_put_callback() requests are queued, whereas ca_put() requests are simply cached. The result is that the record processing load on the IOC is much lower when using ca_put().

Ref: http://www.aps.anl.gov/epics/base/R3-14/12-docs/AppDevGuide.pdf Section 5.10, 5.11

ca_put_callback() would be preferred for the communication reliability if offers; however, there is no way to separate this communication behaviour from the record-processing behaviour.  We've found that some IOC applications and device drivers cannot process records fast enough when using ca_put_callback(), leading to bouts of "constipation" that can last for many seconds, or indefinitely, while the queue is processed. Therefore, we use ca_put() by default.

Applications that want to make use of the extra delivery checking provided by ca_put_callback() can enable it using this function.


<b>Reference:</b> An excerpt from Jeff Hill about the differences between ca_put() and ca_put_callback():

<i>  A ca put request causes the record to process if the record's SCAN field is set to passive, and the field being written has it's process passive attribute set to true. If such a record is already processing when a put request is initiated the specified field is written immediately, and the record is scheduled to process again as soon as it finishes processing. Earlier instances of multiple put requests initiated while the record is being processing may be discarded, but the last put request initiated is always written and processed.

  A ca put callback request causes the record to process if the record's SCAN field is set to passive, and the field being written has it's process passive attribute set to true. For such a record, the user's put callback function is not called until after the record, and any records that the record links to, finish processing. If such a record is already processing when a put callback request is initiated the put callback request is postponed until the record, and any records it links to, finish processing.
  If the record's SCAN field is not set to passive, or the field being written has it's process passive attribute set to false then the ca put or ca put callback request cause the specified field to be immediately written, but they do not cause the record to be processed.
  </i>
   */
	void enablePutCallback(bool putCallbackEnabled) { d_->enablePutCallback(putCallbackEnabled); }

	////////////////////////////////////


signals:
	/// Emits connected(true) when connection is established, connected(false) when lost.
	void connected(bool);
	/// Convenience: same signal without the argument. Implies connected(true)
	void connected();
	/// Implies connected(false)
	void disconnected();
	/// Connection status changed. The new state is \c channelState, which corresponds to the values defined in the channel_state enum in <epics base>/include/cadef.h:{cs_never_conn = 0, cs_prev_conn, cs_conn, cs_closed}. Provides more detail than connected(bool).
	void connectionStateChanged(int channelState);

	/// PV failed to connect within the timeout.
	void connectionTimeout();

	/// Connection was established, and control information was retrieved. After initialized() is emitted, we'll know the data type, units, limits, enum strings, etc.  However, we may not have retrieved the actual values yet...
	void initialized();
	/// Signals changes in hasValues(). \see hasValues().
	void hasValuesChanged(bool);

	/// Signals changes in readReady(). \see readReady().
	void readReadyChanged(bool);
	/// Signals changes in writeRead(). \see writeRead().
	void writeReadyChanged(bool);

	/// Signals changes in alarmStatus() or alarmSeverity().
	void alarmChanged(int status, int severity);

	/// An error occurred... this is the error code.
	void error(int);
	/// For convenience: the error() signal, but with a string explanation from errorString()
	void error(const QString&);

	/// \name Value Monitoring
	/// These signals are emitted when new values arrive.
	/*! valueChanged() always happens. valueChanged(int) and valueChanged(double) are emitted for all numeric types. For performance reasons, valueChanged(string) is only emitted if the natural type is a string type or enum type.
   */
	//@{
	void valueChanged();	///< emitted whenever a new value arrives
	void valueChanged(int);	///< emitted for all numeric values
	void valueChanged(double); ///< emitted for all numeric values
	/// For performance reasons, valueChanged(string) is only emitted if the natural type is a string type or enum type.
	void valueChanged(const QString&);
	//@}

	/// Emitted when a write-request comes back as completed or failed. Only available if enablePutCallback() has been set to true.
	void putRequestReturned(int status);

protected slots:
	/// Runs when the connection timer expires, to check if we are connected yet.
	void onConnectionTimeout();


protected:
	/// Private instance of a channel access connection, possibly shared with other AMProcessVariables that refer to the same underlying PV.
	AMProcessVariablePrivate* d_;
	/// Flag to indicate that this PV wants to receive monitor notifications when the value changes.
	bool shouldBeMonitoring_;

	friend class AMProcessVariablePrivate;
};

/**
  \addtogroup control
  @}
  */

#endif /*AM_PROCESSVARIABLE_H_*/
