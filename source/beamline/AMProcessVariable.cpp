/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#include "AMProcessVariable.h"
#include <float.h>
#include <QDebug>

#include <cstdlib>



///////////////////////////////
// AMProcessVariableSupport
///////////////////////////////

AMProcessVariableSupport* AMProcessVariableSupport::instance_ = 0;

// constructor: initializes channel access, starts the ca_poll timer, and installs us as the global exception handler.
AMProcessVariableSupport::AMProcessVariableSupport() : QObject() {

	qRegisterMetaType<AMProcessVariableDoubleVector>();
	qRegisterMetaType<AMProcessVariableIntVector>();

	connect(&flushIOCaller_, SIGNAL(executed()), this, SLOT(executeFlushIO()));

	qDebug("Starting up channel access...");

	putenv("EPICS_CA_MAX_ARRAY_BYTES=" AMPROCESSVARIABLE_MAX_CA_ARRAY_BYTES);

	int lastError = ca_context_create(ca_enable_preemptive_callback);
	if(lastError != ECA_NORMAL )
		throw lastError;

	lastError = ca_add_exception_event(PVExceptionCB, 0 );
	if(lastError != ECA_NORMAL )
		throw lastError;

	timerId_ = startTimer( PV_HEARTBEAT_MS );

}

// standard singleton-pattern getInstance() method.
AMProcessVariableSupport* AMProcessVariableSupport::getInstance() {

	if (instance_ == 0)  { // is it the first call?
		instance_ = new AMProcessVariableSupport(); // create sole instance
	}

	return instance_;
}

// the implementation of AMProcessVariableSupport::removePV():
void AMProcessVariableSupport::removePVImplementation(chid c) {

	// unregister this channel:
	map_.remove(qint64(c));

	// if that was the last one out, tear down Channel Access:
	if(map_.count() == 0) {

		qDebug("Shutting down channel access...");
		// removed: killTimer(timerId_);			// stop the ca_poll() timer.
		ca_add_exception_event(0, 0);	// return the default exception handler
		ca_context_destroy();			// shut down Channel Access
		instance_ = 0;					// We are no more...
		deleteLater();					// We're gone.
	}
}

void AMProcessVariableSupport::PVExceptionCB(struct exception_handler_args args) {

	// If there's a specific channel, pass it on to the channel's exception handler:
	if(args.chid && getInstance()->map_.contains(qint64(args.chid)) ) {

		getInstance()->map_.value(qint64(args.chid))->exceptionCB(args);

	}

	// otherwise handle it here:
	else {

		char buf[512];
		sprintf ( buf, "AMProcessVariableSupport: Epics exception: %s - with request op=%d data type=%s count=%d", args.ctx, (int)args.op, dbr_type_to_text ( args.type ), (int)args.count );
		qDebug() << buf;
		ca_signal ( args.stat, buf );
	}

}



AMProcessVariable::AMProcessVariable(const QString& pvName, bool autoMonitor, QObject* parent, int timeoutMs) : QObject(parent), shouldBeMonitoring_(autoMonitor) {

	setObjectName("AMProcessVariable_" + pvName);

	// connect signals and slots we use to pass data safely back from callback functions called in other threads:
	connect(this, SIGNAL(internal_error(int)), this, SLOT(internal_onError(int)));
	connect(this, SIGNAL(internal_connectionStateChanged(bool,int,int,int)), this, SLOT(internal_onConnectionStateChanged(bool,int,int,int)));
	connect(this, SIGNAL(internal_controlInfoChanged(int,QString,int,double,double,double,double,QStringList)), this, SLOT(internal_onControlInfoChanged(int,QString,int,double,double,double,double,QStringList)));
	connect(this, SIGNAL(internal_floatingPointValueChanged(AMProcessVariableDoubleVector)), this, SLOT(internal_onFloatingPointValueChanged(AMProcessVariableDoubleVector)));
	connect(this, SIGNAL(internal_integerValueChanged(AMProcessVariableIntVector)), this, SLOT(internal_onIntegerValueChanged(AMProcessVariableIntVector)));
	connect(this, SIGNAL(internal_enumValueChanged(AMProcessVariableIntVector)), this, SLOT(internal_onEnumValueChanged(AMProcessVariableIntVector)));
	connect(this, SIGNAL(internal_stringValueChanged(QStringList)), this,  SLOT(internal_onStringValueChanged(QStringList)));


	// Install convenience signal generators: (these create the error(QString message) and connected() and disconnected() signals.
	connect(this, SIGNAL(error(int)), this, SLOT(signalForwardOnError(int)));
	connect(this, SIGNAL(connected(bool)), this, SLOT(signalForwardOnConnected(bool)));

	// This PV is not initialized yet:
	initialized_ = false;
	hasValues_ = false;
	serverType_ = Unconnected;
	ourType_ = Unconnected;

	lastReadReady_ = false;
	lastWriteReady_ = false;
	connect(this, SIGNAL(connected()), this, SLOT(checkReadWriteReady()));
	connect(this, SIGNAL(initialized()), this, SLOT(checkReadWriteReady()));
	connect(this, SIGNAL(hasValuesChanged(bool)), this, SLOT(checkReadWriteReady()));

	count_ = 0;

	// Initialize control information:
	units_ = "?";
	precision_ = 0;
	// Limits are initialized to the widest possible range at the start. Is this safe? Don't know what else to use, until we find out from the PV itself.
	lowerLimit_ = -DBL_MAX;
	upperLimit_ = DBL_MAX;
	upperGraphLimit_ = DBL_MAX;
	lowerGraphLimit_ = -DBL_MAX;

	// Sets disablePutCallback to false so that setValue uses ca_put_callback() by default.
	disablePutCallback_ = false;

	chid_ = 0;
	channelCreated_ = true;

	try {

		AMProcessVariableSupport::ensureChannelAccess();

		// attempt to search/connect:
		lastError_ = ca_create_channel (pvName.toAscii().constData(), PVConnectionChangedCBWrapper, this, CA_PRIORITY_DEFAULT, &chid_ );
		if(lastError_ != ECA_NORMAL)
			throw lastError_;

		AMProcessVariableSupport::flushIO();

		// This will notice if the search times out:
		QTimer::singleShot(timeoutMs, this, SLOT(onConnectionTimeout()));

		// register ourself to the support class:
		AMProcessVariableSupport::registerPV(chid_, this);
		// qDebug() << QString("AMProcessVariable: Creating AMProcessVariable %1").arg(pvName);

	}

	catch(int s) {

		qDebug() << QString("AMProcessVariable: Error initializing AMProcessVariable for process variable named '%1'. Reason: %2").arg(pvName).arg(ca_message(lastError_));
		channelCreated_ = false;
		emit error(s);
	}
}


AMProcessVariable::~AMProcessVariable() {

	// qDebug() << QString("deleting AMProcessVariable %1.").arg(pvName());

	emit disconnected();
	emit connected(false);

	// This is unnecessary... called auto. by ca_clear_channel():
	// ca_clear_subscription(evid_);

	if(channelCreated_) {
		ca_clear_channel(chid_);

		// deregister ourself from the support class:
		AMProcessVariableSupport::removePV(chid_);
	}

}

// Wrapper functions to deliver the callbacks to the class instances:
////////////////////
void AMProcessVariable::PVConnectionChangedCBWrapper(struct connection_handler_args connArgs) {

	// dig the instance pointer out of the puser field of the chid (inside connArgs)
	AMProcessVariable* myPV = reinterpret_cast<AMProcessVariable*>( ca_puser(connArgs.chid) );
	if(myPV)
		myPV->connectionChangedCB(connArgs);

}

void AMProcessVariable::PVValueChangedCBWrapper(struct event_handler_args eventArgs) {

	// dig the instance pointer out of the puser field of the chid (inside connArgs)
	AMProcessVariable* myPV = reinterpret_cast<AMProcessVariable*>( ca_puser(eventArgs.chid) );
	if(myPV)
		myPV->valueChangedCB(eventArgs);

}

void AMProcessVariable::PVControlInfoCBWrapper(struct event_handler_args eventArgs) {

	// dig the instance pointer out of the puser field of the chid (inside connArgs)
	AMProcessVariable* myPV = reinterpret_cast<AMProcessVariable*>( ca_puser(eventArgs.chid) );
	if(myPV)
		myPV->controlInfoCB(eventArgs);

}

void AMProcessVariable::PVPutRequestCBWrapper(struct event_handler_args eventArgs) {

	// dig the instance pointer out of the puser field of the chid (inside connArgs)
	AMProcessVariable* myPV = reinterpret_cast<AMProcessVariable*>( ca_puser(eventArgs.chid) );
	if(myPV)
		myPV->putRequestCB(eventArgs);

}
/////////////////////


// Callbacks for events:
/////////////////////
void AMProcessVariable::exceptionCB(struct exception_handler_args args) {



	emit internal_error(args.stat);	// will notify eventually with error() signals

	qDebug() << QString("AMProcessVariable: EPICS exception: %1\n  Operation: %2\n  Channel: %3\n  Data type: %4\n  Count: %5\n\n  Epics says: %6\n").arg(ca_message(args.stat)).arg(args.op).arg(pvName()).arg(dbr_type_to_text ( args.type )).arg(args.count).arg(args.ctx);
	ca_signal( args.stat, args.ctx );

}

void AMProcessVariable::connectionChangedCB(struct connection_handler_args connArgs) {


	int lastError;
	chtype serverType;
	PVDataType ourType;

	//
	//  cs_ - `channel state'
	//
	//  cs_never_conn       valid chid, IOC not found
	//  cs_prev_conn        valid chid, IOC was found, but unavailable
	//  cs_conn             valid chid, IOC was found, still available
	//  cs_closed           channel deleted by user
	//
	//

	// Possibility 1: Connection gained:
	if( ca_state(connArgs.chid) == cs_conn && connArgs.op == CA_OP_CONN_UP) {

		// Discover the type of this channel:
		serverType = ca_field_type(chid_);

		// We simplify all floating-point types to double, all integer types to long, and leave strings as strings and enums as enums:
		ourType = serverType2ourType(serverType);

		// Make sure we have room for storage. Integers and Enums held in data_int_internal_.  FloatingPoints in data_dbl_internal_. (NOTE: this storage is just a performance optimization, and only accessed by the callback functions within channel-access thread-space. It prevents these functions from having to allocate storage each time new values arrive. These arrays may be accessed from various channel-access threads at any point in time (although channel-access guarantees that no more than one premptive callback will be made simultaneously), and therefore should not be read or written from the main thread.
		/*
		switch(ourType) {
		case Integer:
		case Enum:
			data_int_internal_.resize(ca_element_count(chid_));
			break;
		case FloatingPoint:
			data_dbl_internal_.resize(ca_element_count(chid_));
			break;
		case String:
			data_str_internal_.clear();
			break;
		case Unconnected:
			break;// won't happen, but if it does, nothing required.
		}*/

		// Request some information about the PV (units, control limits, etc.).
		// For enum types, we want the list of string names:
		if(ourType == Enum) {
			lastError = ca_get_callback(DBR_CTRL_ENUM, chid_, PVControlInfoCBWrapper, this);

			if(lastError != ECA_NORMAL) {
				qDebug() << QString("AMProcessVariable: Error while trying to request enum control information: %1: %2").arg(pvName()).arg(ca_message(lastError));
				emit internal_error(lastError);
			}
		}
		// otherwise, requesting control information as DBR_CTRL_DOUBLE because this gives the most information that could possibly be available (precision, limits, units)
		else if (ourType == Integer || ourType == FloatingPoint){
			lastError = ca_get_callback(DBR_CTRL_DOUBLE, chid_, PVControlInfoCBWrapper, this);
			if(lastError != ECA_NORMAL) {
				qDebug() << QString("AMProcessVariable: Error while trying to request value: %1: %2").arg(pvName()).arg(ca_message(lastError));
				emit internal_error(lastError);
			}
		}
		else if(ourType == String){
			lastError = ca_get_callback(DBR_CTRL_STRING, chid_, PVControlInfoCBWrapper, this);
			if(lastError != ECA_NORMAL) {
				qDebug() << QString("AMProcessVariable: Error while trying to request value: %1: %2").arg(pvName()).arg(ca_message(lastError));
				emit internal_error(lastError);
			}
		}
		/// \todo What control type to request if type is not any of these?
		/// \bug Currently, if the type is not an enum, integer, floating-point, or string, PV's will not emit initialized().

		// It's useful to automatically-request the value, after we are first connected:
		lastError = ca_array_get_callback(ourType, ca_element_count(chid_), chid_, PVValueChangedCBWrapper, this);
		if(lastError != ECA_NORMAL) {
			qDebug() << QString("AMProcessVariable: Error while trying to request value: %1: %2").arg(pvName()).arg(ca_message(lastError));
			emit internal_error(lastError);
		}

		if(shouldBeMonitoring_) {
			lastError = ca_create_subscription(ourType, ca_element_count(chid_), chid_, DBE_VALUE | DBE_LOG | DBE_ALARM, PVValueChangedCBWrapper, this, &evid_ );
			if(lastError != ECA_NORMAL) {
				qDebug() << QString("AMProcessVariable: Error starting monitoring: %1: %2").arg(pvName()).arg(ca_message(lastError));
				emit internal_error(lastError);
			}
		}

		// AMProcessVariableSupport::flushIO() may not be thread-safe... Use ca_flush_io directly instead.
		ca_flush_io();

		emit internal_connectionStateChanged(true, ca_element_count(chid_), serverType, ourType);
	}
	// Possibility 2: anything but a connection gained:
	else {
		emit internal_connectionStateChanged(false, 0, 0, 0);
	}

	emit connectionStateChanged( ca_state(connArgs.chid) );

}


void AMProcessVariable::controlInfoCB(struct event_handler_args eventArgs) {


	int lastError;

	QStringList enumStrings;

	struct dbr_ctrl_double* ctrlValue;
	struct dbr_ctrl_enum* enumCtrlValue;

	if( (lastError = eventArgs.status) != ECA_NORMAL) {
		qDebug() << QString("Error in control-info-changed callback: %1: %2").arg(pvName()).arg(ca_message(lastError));
		emit internal_error(lastError);
		return;
	}

	if(eventArgs.dbr == 0)
		return;

	// Handle this based on the type of information we're getting:
	switch(eventArgs.type) {

		// Is this limit-precision-unit information?
	case DBR_CTRL_DOUBLE:
		ctrlValue =  (struct dbr_ctrl_double*)eventArgs.dbr;
		emit internal_controlInfoChanged(DBR_CTRL_DOUBLE,
										 QString(ctrlValue->units),
										 ctrlValue->precision,
										 ctrlValue->upper_disp_limit,
										 ctrlValue->lower_disp_limit,
										 ctrlValue->upper_ctrl_limit,
										 ctrlValue->lower_ctrl_limit,
										 QStringList());
		break;

		// Is this enum count/string information?
	case DBR_CTRL_ENUM:
		enumCtrlValue = (struct dbr_ctrl_enum*)eventArgs.dbr;
		for(int i=0; i<enumCtrlValue->no_str; i++)
			enumStrings << QString(enumCtrlValue->strs[i]);
		emit internal_controlInfoChanged(DBR_CTRL_ENUM,
										 "[choice]",
										 0,
										 enumCtrlValue->no_str - 1, 0,
										 enumCtrlValue->no_str - 1, 0,
										 enumStrings);
		break;

	case DBR_CTRL_STRING:
		emit internal_controlInfoChanged(DBR_CTRL_STRING,
										 QString(),
										 0,
										 DBL_MAX,	-DBL_MAX,
										 DBL_MAX, -DBL_MAX,
										 QStringList());
		break;
	}
}

// eventArgs:
//void            *usr;   user argument supplied with request
//chanId          chid;   channel id
//long            type;   the type of the item returned
//long            count;  the element count of the item returned
//const void      *dbr;   a pointer to the item returned
//int             status; ECA_XXX status of the requested op from the server
//

void AMProcessVariable::valueChangedCB(struct event_handler_args eventArgs) {


	int lastError;
	if( (lastError = eventArgs.status) != ECA_NORMAL) {
		qDebug() << QString("Error in value-changed callback: %1: %2").arg(pvName()).arg(ca_message(lastError));
		emit internal_error(lastError);
		return;
	}

	if(eventArgs.dbr == 0)
		return;

	// Handle this based on the type of information we're getting:
	// Note: the onConnectionChanged handler should have got the vector sizes right before we get any of this data.
	switch(eventArgs.type) {

	case FloatingPoint: {
			AMProcessVariableDoubleVector rv(eventArgs.count);
			for(int i=0; i<eventArgs.count; i++)
				rv[i] = ((dbr_double_t*)eventArgs.dbr)[i];
			emit internal_floatingPointValueChanged(rv);
			break;
		}

	case Integer: {
			AMProcessVariableIntVector rv(eventArgs.count);
			for(int i=0; i<eventArgs.count; i++)
				rv[i] = ((dbr_long_t*)eventArgs.dbr)[i];
			emit internal_integerValueChanged(rv);
			break;
		}

	case Enum: {
			AMProcessVariableIntVector rv(eventArgs.count);
			for(int i=0; i<eventArgs.count; i++)
				rv[i] = ((dbr_enum_t*)eventArgs.dbr)[i];
			emit internal_enumValueChanged(rv);
			break;
		}

	case String: {
			QStringList rv;
			for(int i=0; i<eventArgs.count; i++)
				rv << QString( ((dbr_string_t*)eventArgs.dbr)[i] );
			emit internal_stringValueChanged(rv);
			break;
		}
	}
}



void AMProcessVariable::putRequestCB(struct event_handler_args eventArgs) {

	if(eventArgs.status != ECA_NORMAL) {


		qDebug() << QString("AMProcessVariable: Error in put request: %1: %2").arg(pvName()).arg(ca_message(eventArgs.status));
		emit internal_error(eventArgs.status);
	}

	emit putRequestReturned(eventArgs.status);
}

void AMProcessVariable::internal_onConnectionStateChanged(bool isConnected, int count, int serverType, int ourType) {

	if(isConnected) {
		count_ = count;
		serverType_ = serverType;
		ourType_ = (PVDataType)ourType;
	}

	else {
		initialized_ = false;
		if(hasValues_)
			emit hasValuesChanged(false);
		hasValues_ = false;
	}

	emit connected(isConnected);
}


void AMProcessVariable::internal_onControlInfoChanged(int controlInfoType, QString units, int precision, double upperGraphLimit, double lowerGraphLimit, double upperLimit, double lowerLimit, QStringList enumStrings) {

	Q_UNUSED(controlInfoType)

	units_ = units;
	precision_ = precision;
	upperGraphLimit_ = upperGraphLimit;
	lowerGraphLimit_ = lowerGraphLimit;
	upperLimit_ = upperLimit;
	lowerLimit_ = lowerLimit;
	enumStrings_ = enumStrings;

	//If the record's DRV_HIGH and DRV_LOW limits haven't been set, we're receiving them as max(0), min(0).
	// That's a problem, because it pins our minValue() and maxValue() to 0.
	if(lowerLimit_ == 0 && upperLimit_ == 0) {
		lowerLimit_ = -DBL_MAX;
		upperLimit_ = DBL_MAX;
	}
	else if(lowerLimit_ > upperLimit_){
		//In this case we're pinned again
		lowerLimit_ = -DBL_MAX;
		upperLimit_ = DBL_MAX;
	}

	//If the record's OPR_HIGH and OPR_LOW limits haven't been set, we're receiving them as max(0), min(0).
	// That's a problem, because it pins our minValue() and maxValue() to 0 for readonly.
	if(lowerGraphLimit_ == 0 && upperGraphLimit_ == 0) {
		lowerGraphLimit_ = -DBL_MAX;
		upperGraphLimit_ = DBL_MAX;
	}
	else if(lowerGraphLimit_ > upperGraphLimit_){
		//In this case we're pinned again
		lowerGraphLimit_ = -DBL_MAX;
		upperGraphLimit_ = DBL_MAX;
	}

	initialized_ = true;
	emit initialized();
}

void AMProcessVariable::internal_onFloatingPointValueChanged(AMProcessVariableDoubleVector doubleData) {
	bool hasChanged = false;
	if(!hasValues_)
		hasChanged = true;
	hasValues_ = true;
	data_dbl_ = doubleData;
	if(hasChanged)
		emit hasValuesChanged(true);
	emit valueChanged(data_dbl_.at(0));
	emit valueChanged(int(data_dbl_.at(0)));
	emit valueChanged();
}

void AMProcessVariable::internal_onIntegerValueChanged(AMProcessVariableIntVector intData) {
	bool hasChanged = false;
	if(!hasValues_)
		hasChanged = true;
	hasValues_ = true;
	data_int_ = intData;
	if(hasChanged)
		emit hasValuesChanged(true);
	emit valueChanged(data_int_.at(0));
	emit valueChanged(double(data_int_.at(0)));
	emit valueChanged();
}

void AMProcessVariable::internal_onEnumValueChanged(AMProcessVariableIntVector enumData) {
	bool hasChanged = false;
	if(!hasValues_)
		hasChanged = true;
	hasValues_ = true;
	data_int_ = enumData;
	int newValue = data_int_.at(0);
	if(hasChanged)
		emit hasValuesChanged(true);
	emit valueChanged(newValue);
	emit valueChanged(double(newValue));
	if(initialized_) {
		// check to make sure that the enum value is within the allowed enum range
		// Some bad IOCs might send an enum value that is not within their set of enum options.
		if((unsigned)newValue < (unsigned)enumStrings_.count())
			emit valueChanged(enumStrings_.at(newValue));
		else
			emit valueChanged(QString("[enum value out of range]"));
	}
	emit valueChanged();
}


void AMProcessVariable::internal_onStringValueChanged(QStringList stringData) {
	bool hasChanged = false;
	if(!hasValues_)
		hasChanged = true;
	hasValues_ = true;
	data_str_ = stringData;
	if(hasChanged)
		emit hasValuesChanged(true);
	emit valueChanged(data_str_.at(0));
	emit valueChanged();
}


void AMProcessVariable::onConnectionTimeout() {



	// If we haven't connected by now:
	if(this->connectionState() != cs_conn) {
		qDebug() << QString("AMProcessVariable: channel connect timed out for %1").arg(pvName());
		emit connectionTimeout();
	}

	// Don't call ca_clear_channel here.  Channel access will keep on trying in the background. (Channel access handles this automatically for us.)  Will emit connected() signal when successful.
}

void AMProcessVariable::checkReadWriteReady(){
	// We aren't readReady, but the only problem is ca_read_access failed
	if(!readReady() && isConnected() && isInitialized() && hasValues()){
		emit error(AMPROCESSVARIABLE_CANNOT_READ);
		emit error("Lacking read access to PV");
	}
	// We aren't writeReady, but the only problem is ca_write_access failed
	if(!writeReady() && readReady()){
		emit error(AMPROCESSVARIABLE_CANNOT_WRITE);
		emit error("Lacking write access to PV");
	}
	if(lastReadReady_ != readReady()){
		lastReadReady_ = readReady();
		emit readReadyChanged(lastReadReady_);
	}
	if(lastWriteReady_ != writeReady()){
		lastWriteReady_ = writeReady();
		emit writeReadyChanged(lastWriteReady_);
	}
}


bool AMProcessVariable::startMonitoring() {

	lastError_ = ca_create_subscription(ourType_, ca_element_count(chid_), chid_, DBE_VALUE | DBE_LOG | DBE_ALARM, PVValueChangedCBWrapper, this, &evid_ );
	if(lastError_ != ECA_NORMAL) {
		qDebug() << QString("AMProcessVariable: Error starting monitoring: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
		return false;
	}

	AMProcessVariableSupport::flushIO();

	return true;
}

void AMProcessVariable::stopMonitoring() {

	ca_clear_subscription(evid_);
	AMProcessVariableSupport::flushIO();
}

bool AMProcessVariable::requestValue(int numberOfValues) {

	//just for lastError_

	// Not necessary. Connection status is checked by ca_array_get_callback:
	//if(ca_state(chid_) != cs_conn) {
	//	qDebug(QString("Error requesting value: channel not connected: %1").arg(pvName()));
	//	return false;
	//}

	lastError_ = ca_array_get_callback(ourType_, numberOfValues, chid_, PVValueChangedCBWrapper, this);
	if(lastError_ != ECA_NORMAL) {
		qDebug() << QString("AMProcessVariable: Error while trying to request value: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
		return false;
	}

	AMProcessVariableSupport::flushIO();

	return true;
}

void AMProcessVariable::setValue(int value) {

	//just for lastError_

	dbr_long_t setpoint = value;

	if (disablePutCallback_)
		lastError_ = ca_put( DBR_LONG, chid_, &setpoint );
	else
		lastError_ = ca_put_callback( DBR_LONG, chid_, &setpoint, PVPutRequestCBWrapper, this );

	if(lastError_ != ECA_NORMAL) {
		qDebug() << QString("AMProcessVariable: Error while trying to put value: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
	}

	AMProcessVariableSupport::flushIO();
}

void AMProcessVariable::setValues(dbr_long_t setpoints[], int num) {

	//just for lastError_

	if (disablePutCallback_)
		lastError_ = ca_array_put( DBR_LONG, num, chid_, setpoints );
	else
		lastError_ = ca_array_put_callback( DBR_LONG, num, chid_, setpoints, PVPutRequestCBWrapper, this );

	if(lastError_ != ECA_NORMAL) {
		qDebug() << QString("AMProcessVariable: Error while trying to put values: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
	}

	AMProcessVariableSupport::flushIO();
}

void AMProcessVariable::setValue(double value) {

	//just for lastError_

	dbr_double_t setpoint = value;

	if (disablePutCallback_)
		lastError_ = ca_put( DBR_DOUBLE, chid_, &setpoint );
	else
		lastError_ = ca_put_callback( DBR_DOUBLE, chid_, &setpoint, PVPutRequestCBWrapper, this );

	if(lastError_ != ECA_NORMAL) {
		qDebug() << QString("Error while trying to put AMProcessVariable value: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
	}

	AMProcessVariableSupport::flushIO();
}

void AMProcessVariable::setValues(dbr_double_t setpoints[], int num) {

	//just for lastError_

	if (disablePutCallback_)
		lastError_ = ca_array_put( DBR_DOUBLE, num, chid_, setpoints );
	else
		lastError_ = ca_array_put_callback( DBR_DOUBLE, num, chid_, setpoints, PVPutRequestCBWrapper, this );

	if(lastError_ != ECA_NORMAL) {
		qDebug() << QString("Error while trying to put AMProcessVariable values: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
	}

	AMProcessVariableSupport::flushIO();
}

void AMProcessVariable::setValue(const QString& value) {

	//just for lastError_

	dbr_string_t setpoint;
	QByteArray d1 = value.toAscii();
	strcpy(setpoint, d1.constData());

	if (disablePutCallback_)
		lastError_ = ca_put( DBR_STRING, chid_, setpoint );
	else
		lastError_ = ca_put_callback( DBR_STRING, chid_, setpoint, PVPutRequestCBWrapper, this );

	if(lastError_ != ECA_NORMAL) {
		qDebug() << QString("Error while trying to put AMProcessVariable value: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
	}

	AMProcessVariableSupport::flushIO();
}

// TODO: not sure if this is right...
void AMProcessVariable::setValues(const QStringList& setpoints) {

	//just for lastError_

	QList<QByteArray> asciiData;	// will hold the ascii form of our strings
	const char** stringArray = new const char*[setpoints.size()];		// an array of strings (array of char*... ie: char**) which ca_array_put_callback requires.

	for(int i=0; i<setpoints.size(); i++) {
		asciiData.append( setpoints[i].toAscii() );
		stringArray[i] = asciiData[i].constData();
	}

	if (disablePutCallback_)
		lastError_ = ca_array_put( DBR_STRING, setpoints.size(), chid_, stringArray );
	else
		lastError_ = ca_array_put_callback( DBR_STRING, setpoints.size(), chid_, stringArray, PVPutRequestCBWrapper, this );

	if(lastError_ != ECA_NORMAL) {
		qDebug() << QString("Error while trying to put AMProcessVariable values: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
	}
	// TODO: check that ca_array_put_callback doesn't require this left in memory after this function goes out of scope?
	delete[] stringArray;

	AMProcessVariableSupport::flushIO();

}


double AMProcessVariable::lastValue(unsigned index) const {



	// Depending on our type, we need to know where to look and how to convert:
	switch(ourType_) {

	case FloatingPoint:
		if(index >= (unsigned)data_dbl_.count())
			return -1.0;
		return data_dbl_.at(index);

	case Integer:
	case Enum:
		if(index >= (unsigned)data_int_.count())
			return -1.0;
		return (double)data_int_.at(index);

	case String:
		if(index >= (unsigned)data_str_.count())
			return -1.0;
		return data_str_.at(index).toDouble();

		// default case, including Unconnected:
	default:
		return -1.0;
	}
}

int AMProcessVariable::binIntegerValues(int lowIndex, int highIndex) const{
	QVector<int> lastInts = lastIntegerValues();
	int lIndex = lowIndex;
	int hIndex = highIndex;
	int rVal = 0;
	if(hIndex < lIndex)
		return rVal;
	if(lIndex < 0)
		lIndex = 0;
	if(hIndex > lastInts.count())
		hIndex = lastInts.count();
	for(int x = lIndex; x < hIndex; x++)
		rVal += lastInts.at(x);
	return rVal;
}

double AMProcessVariable::binFloatingPointValues(int lowIndex, int highIndex) const{
	QVector<double> lastDoubles = lastFloatingPointValues();
	int lIndex = lowIndex;
	int hIndex = highIndex;
	double rVal = 0.0;
	if(hIndex < lIndex)
		return rVal;
	if(lIndex < 0)
		lIndex = 0;
	if(hIndex > lastDoubles.count())
		hIndex = lastDoubles.count();
	for(int x = lIndex; x < hIndex; x++)
		rVal += lastDoubles.at(x);
	return rVal;
}

// double AMProcessVariable::getDouble() is just a synonym for lastValue().

int AMProcessVariable::getInt(unsigned index) const {



	// Depending on our type, we need to know where to look and how to convert:
	switch(ourType_) {

	case FloatingPoint:
		if(index >= (unsigned)data_dbl_.count())
			return -1;
		return (int)data_dbl_.at(index);

	case Integer:
	case Enum:
		if(index >= (unsigned)data_int_.count())
			return -1;
		return data_int_.at(index);

	case String:
		if(index >= (unsigned)data_str_.count())
			return -1;
		return data_str_.at(index).toInt();

		// default case, including Unconnected:
	default:
		return -1;
	}
}


QString AMProcessVariable::getString(unsigned index) const {



	// Depending on our type, we need to know where to look and how to convert:
	switch(ourType_) {

	case FloatingPoint:
		if(index >= (unsigned)data_dbl_.count())
			return "[array index out of range]";
		return QString("%1").arg(data_dbl_.at(index));

	case Integer:
		if(index >= (unsigned)data_int_.count())
			return "[array index out of range]";
		return QString("%1").arg(data_int_.at(index));

	case Enum:
		if(index >= (unsigned)data_int_.count())
			return "[array index out of range]";
		if(initialized_) {
			int intValue = data_int_.at(index);
			// check to make sure that the enum value is within the allowed enum range
			// Some bad IOCs might send an enum value that is not within their set of enum options. Dude!?
			if((unsigned)intValue < (unsigned)enumStrings_.count())
				return enumStrings_.at(intValue);
			else
				return QString("[enum value out of range]");
		}
		else	// otherwise we just return the number, in string form:
			return QString("%1").arg(data_int_.at(index));


	case String:
		if(index >= (unsigned)data_str_.count())
			return "[array index out of range]";
		return data_str_.at(index);

		// default case, including Unconnected:
	default:
		return "[PV not connected]";
	}
}

// This a convenience function that we can call to figure out our simplified type based on an epics ca_field_type(). Possibilities are:
/*
#define DBF_STRING	0
#define	DBF_INT		1
#define	DBF_SHORT	1
#define	DBF_FLOAT	2
#define	DBF_ENUM	3
#define	DBF_CHAR	4
#define	DBF_LONG	5
#define	DBF_DOUBLE	6

//
//
//DBR_CHAR  	dbr_char_t  	8 bit character
//DBR_SHORT 	dbr_short_t 	16 bit integer
//DBR_ENUM 	dbr_enum_t 	16 bit unsigned integer
//DBR_LONG 	dbr_long_t 	32 bit signed integer
//DBR_FLOAT 	dbr_float_t 	32 bit IEEE floating point
//DBR_DOUBLE 	dbr_double_t 	64 bit IEEE floating point
//DBR_STRING 	dbr_string_t 	40 character string
//
//
  */
AMProcessVariable::PVDataType AMProcessVariable::serverType2ourType(chtype serverType) {

	switch(serverType) {
	case DBF_STRING:
		return String;
	case DBF_SHORT:
	case DBF_CHAR:
	case DBF_LONG:
		return Integer;
	case DBF_ENUM:
		return Enum;
	case DBF_FLOAT:
	case DBF_DOUBLE:
		return FloatingPoint;
	}
	return FloatingPoint;
}



