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


#include "AMProcessVariablePrivate.h"
#include "AMProcessVariable.h"
#include "util/AMErrorMonitor.h"

#include <float.h>

#include <cstdlib>
#include <stdio.h>


///////////////////////////////
// AMProcessVariableSupport
///////////////////////////////

AMProcessVariableSupport* AMProcessVariableSupport::instance_ = 0;

// constructor: initializes channel access, starts the ca_poll timer, and installs us as the global exception handler.
AMProcessVariableSupport::AMProcessVariableSupport() : QObject() {
	qRegisterMetaType<AMProcessVariableDoubleVector>();
	qRegisterMetaType<AMProcessVariableIntVector>();

	connect(&flushIOCaller_, SIGNAL(executed()), this, SLOT(executeFlushIO()));
	connect(&timeoutErrorFunctionCall_, SIGNAL(executed()), this, SLOT(executeTimeoutError()));

	AMErrorMon::information(this, AMPROCESSVARIABLESUPPORT_STARTING_CHANNEL_ACCESS, "Starting up channel access...");


	setenv(QString("EPICS_CA_MAX_ARRAY_BYTES").toAscii().data(), QString("%1").arg(AMPROCESSVARIABLE_MAX_CA_ARRAY_BYTES).toAscii().data(), 1);

	int lastError = ca_context_create(ca_enable_preemptive_callback);
	if(lastError != ECA_NORMAL )
		throw lastError;

	lastError = ca_add_exception_event(PVExceptionCB, 0 );
	if(lastError != ECA_NORMAL )
		throw lastError;

	timerId_ = startTimer( PV_HEARTBEAT_MS );

}

AMProcessVariableSupport::~AMProcessVariableSupport()
{
	ca_add_exception_event(0, 0);	// return the default exception handler
	ca_context_destroy();			// shut down Channel Access
}

void AMProcessVariableSupport::shutdownChannelAccess()
{
	instance_->deleteLater();
	instance_ = 0;
}

// standard singleton-pattern get-instance method.
AMProcessVariableSupport* AMProcessVariableSupport::s() {

	if (instance_ == 0)  { // is it the first call?
		instance_ = new AMProcessVariableSupport(); // create sole instance
	}

	return instance_;
}

// the implementation of AMProcessVariableSupport::removePV():
void AMProcessVariableSupport::removePVImplementation(chid c) {

	// Grab processvariableprivate
	AMProcessVariablePrivate * onePrivate = chid2Private_.value(qint64(c));

	// unregister this channel:
	chid2Private_.remove(qint64(c));

	// grab pv name
	QString pvName = pvName2Private_.key(onePrivate);
	pvName2Private_.remove(pvName);
}

AMProcessVariablePrivate* AMProcessVariableSupport::getPrivateForPVNameImplementation(const QString &pvName)
{
	QHash<QString, AMProcessVariablePrivate*>::const_iterator i = pvName2Private_.find(pvName);
	if(i != pvName2Private_.constEnd())
		return i.value();	// have one already!

	AMProcessVariablePrivate* d = new AMProcessVariablePrivate(pvName);
	pvName2Private_.insert(pvName, d);
	return d;

}


void AMProcessVariableSupport::PVExceptionCB(struct exception_handler_args args) {

	// If there's a specific channel, pass it on to the channel's exception handler:
	if(args.chid && s()->chid2Private_.contains(qint64(args.chid)) ) {

		s()->chid2Private_.value(qint64(args.chid))->exceptionCB(args);

	}

	// otherwise handle it here:
	else {

		char buf[512];
		sprintf ( buf, "AMProcessVariableSupport: Epics exception: %s - with request op=%d data type=%s count=%d", args.ctx, (int)args.op, dbr_type_to_text ( args.type ), (int)args.count );
		AMErrorMon::debug(0, AMPROCESSVARIABLESUPPORT_EPICS_EXCEPTION_PVEXCEPTIONCB, buf);
		ca_signal ( args.stat, buf );
	}

}

void AMProcessVariableSupport::reportTimeoutError(QString pvName){
	s()->timeoutPVNames_.append(pvName);
	s()->timeoutErrorFunctionCall_.runLater(2500);
}

void AMProcessVariableSupport::executeTimeoutError(){
	// This seems kinda stupid, but if we get a lot of these alerts going up the system tray icon can't keep up
	//  so we make sure that we put out no more than 6 actual alerts (they might have a lot of pvs in them if there was
	//  no connection on startup ... but that's fine)
	int pvsPerAlert = s()->timeoutPVNames_.count()/6;
	if(pvsPerAlert < 5)
		pvsPerAlert = 5;
	QString pvsInThisAlert;
	int thisAlertCounter;
	while(s()->timeoutPVNames_.count() > 0){
		pvsInThisAlert.clear();
		thisAlertCounter = 0;
		while( (s()->timeoutPVNames_.count() > 0) && (thisAlertCounter < pvsPerAlert) ){
			pvsInThisAlert.append(QString("\n%1").arg(s()->timeoutPVNames_.takeFirst()));
			thisAlertCounter++;
		}
		AMErrorMon::alert(this, AMPROCESSVARIABLE_CONNECTION_TIMED_OUT, QString("AMProcessVariable: channel connect timed out for %1").arg(pvsInThisAlert));
	}
}

AMProcessVariablePrivate::AMProcessVariablePrivate(const QString& pvName) : QObject() {

	shouldBeMonitoring_ = false;
	setObjectName("AMProcessVariablePrivate_" + pvName);

	// connect signals and slots we use to pass data safely back from callback functions called in other threads:
	connect(this, SIGNAL(internal_error(int)), this, SLOT(internal_onError(int)));
	connect(this, SIGNAL(internal_connectionStateChanged(bool,int,int,int)), this, SLOT(internal_onConnectionStateChanged(bool,int,int,int)));
	connect(this, SIGNAL(internal_controlInfoChanged(int,QString,int,double,double,double,double,QStringList)), this, SLOT(internal_onControlInfoChanged(int,QString,int,double,double,double,double,QStringList)));
	connect(this, SIGNAL(internal_floatingPointValueChanged(AMProcessVariableDoubleVector)), this, SLOT(internal_onFloatingPointValueChanged(AMProcessVariableDoubleVector)));
	connect(this, SIGNAL(internal_integerValueChanged(AMProcessVariableIntVector)), this, SLOT(internal_onIntegerValueChanged(AMProcessVariableIntVector)));
	connect(this, SIGNAL(internal_enumValueChanged(AMProcessVariableIntVector)), this, SLOT(internal_onEnumValueChanged(AMProcessVariableIntVector)));
	connect(this, SIGNAL(internal_stringValueChanged(QStringList)), this,  SLOT(internal_onStringValueChanged(QStringList)));
	connect(this, SIGNAL(internal_alarmChanged(int,int)), this, SLOT(internal_onAlarmChanged(int,int)));


	// Install convenience signal generators: (these create the error(QString message) and connected() and disconnected() signals.
	connect(this, SIGNAL(error(int)), this, SLOT(signalForwardOnError(int)));
	connect(this, SIGNAL(connected(bool)), this, SLOT(signalForwardOnConnected(bool)));

	// This PV is not initialized yet:
	initialized_ = false;
	hasValues_ = false;
	serverType_ = PVDataType::Unconnected;
	ourType_ = PVDataType::Unconnected;
	ourAlarmType_ = DBR_STS_DOUBLE;

	lastReadReady_ = false;
	lastWriteReady_ = false;
	connect(this, SIGNAL(connected()), this, SLOT(checkReadWriteReady()));
	connect(this, SIGNAL(connected(bool)), this, SLOT(checkReadWriteReady()));
	connect(this, SIGNAL(initialized()), this, SLOT(checkReadWriteReady()));
	connect(this, SIGNAL(hasValuesChanged(bool)), this, SLOT(checkReadWriteReady()));

	count_ = 0;

	// Initialize control information:
	units_ = "?";
	precision_ = 3;
	// Limits are initialized to the widest possible range at the start. Is this safe? Don't know what else to use, until we find out from the PV itself.
	lowerLimit_ = -DBL_MAX;
	upperLimit_ = DBL_MAX;
	upperGraphLimit_ = DBL_MAX;
	lowerGraphLimit_ = -DBL_MAX;

	// Use ca_put() instead of ca_put_callback() by default.
	putCallbackEnabled_ = false;

	chid_ = 0;
	evid_ = 0;
	alarmEvid_ = 0;

	alarmStatus_ = 0;
	alarmSeverity_ = 0;

	channelCreated_ = true;
	try {

		AMProcessVariableSupport::ensureChannelAccess();

		// attempt to search/connect to channel:
		lastError_ = ca_create_channel (pvName.toAscii().constData(), PVConnectionChangedCBWrapper, this, CA_PRIORITY_DEFAULT, &chid_ );
		if(lastError_ != ECA_NORMAL)
			throw lastError_;

		AMProcessVariableSupport::flushIO();

		// register ourself to the support class:
		AMProcessVariableSupport::registerPV(chid_, this);
	}

	catch(int s) {

		AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_ERROR_INITIALIZING_PRIVATE, QString("AMProcessVariablePrivate: Error initializing AMProcessVariablePrivate for process variable named '%1'. Reason: %2").arg(pvName).arg(ca_message(lastError_)) );
		channelCreated_ = false;
		emit error(s);
	}
}


AMProcessVariablePrivate::~AMProcessVariablePrivate() {
	// This is unnecessary... called auto. by ca_clear_channel():
	// ca_clear_subscription(evid_);

	if(channelCreated_) {
		ca_clear_channel(chid_);

		// deregister ourself from the support class:
		AMProcessVariableSupport::removePV(chid_);
	}
}

void AMProcessVariablePrivate::attachProcessVariable(AMProcessVariable *pv)
{
	if(attachedProcessVariables_.contains(pv)) {
		AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_CANNOT_ATTACH_ALREADY_ATTACHED, "AMProcessVariablePrivate: Attempted to attach an AMProcessVariable that is already attached.");
		return;
	}

	attachedProcessVariables_ << pv;

	// Forward all signals:
	connect(this, SIGNAL(connected(bool)), pv, SIGNAL(connected(bool)));
	connect(this, SIGNAL(connected()), pv, SIGNAL(connected()));
	connect(this, SIGNAL(disconnected()), pv, SIGNAL(disconnected()));
	connect(this, SIGNAL(connectionStateChanged(int)), pv, SIGNAL(connectionStateChanged(int)));
	connect(this, SIGNAL(initialized()), pv, SIGNAL(initialized()));
	connect(this, SIGNAL(hasValuesChanged(bool)), pv, SIGNAL(hasValuesChanged(bool)));

	connect(this, SIGNAL(readReadyChanged(bool)), pv, SIGNAL(readReadyChanged(bool)));
	connect(this, SIGNAL(writeReadyChanged(bool)), pv, SIGNAL(writeReadyChanged(bool)));

	connect(this, SIGNAL(alarmChanged(int,int)), pv, SIGNAL(alarmChanged(int,int)));

	connect(this, SIGNAL(error(int)), pv, SIGNAL(error(int)));
	connect(this, SIGNAL(error(QString)), pv, SIGNAL(error(QString)));

	connect(this, SIGNAL(valueChanged()), pv, SIGNAL(valueChanged()));
	connect(this, SIGNAL(valueChanged(int)), pv, SIGNAL(valueChanged(int)));
	connect(this, SIGNAL(valueChanged(double)), pv, SIGNAL(valueChanged(double)));
	connect(this, SIGNAL(valueChanged(QString)), pv, SIGNAL(valueChanged(QString)));

	connect(this, SIGNAL(putRequestReturned(int)), pv, SIGNAL(putRequestReturned(int)));

	// If this PV wants to be monitored, this might change whether we should be monitoring.
	reviewMonitoring();

}

void AMProcessVariablePrivate::detachProcessVariable(AMProcessVariable *pv)
{
	if(!attachedProcessVariables_.contains(pv)) {
		AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_CANNOT_DETACH_NOT_ATTACHED, "AMProcessVariablePrivate: Attempted to detach an AMProcessVariable that is not attached.");
		return;
	}

	disconnect(this, 0, pv, 0);
	attachedProcessVariables_.remove(pv);
	// If this PV wanted to be monitored, this might change whether we should be monitoring.
	reviewMonitoring();

	if(attachedProcessVariables_.isEmpty())
		this->deleteLater();	 // we can do this (carefully), since it is the last thing we do, and nothing will ever use us anymore.
}


// Wrapper functions to deliver the callbacks to the class instances:
////////////////////
void AMProcessVariablePrivate::PVConnectionChangedCBWrapper(struct connection_handler_args connArgs) {

	// dig the instance pointer out of the puser field of the chid (inside connArgs)
	AMProcessVariablePrivate* myPV = reinterpret_cast<AMProcessVariablePrivate*>( ca_puser(connArgs.chid) );
	if(myPV)
		myPV->connectionChangedCB(connArgs);

}

void AMProcessVariablePrivate::PVValueChangedCBWrapper(struct event_handler_args eventArgs) {

	// dig the instance pointer out of the puser field of the chid (inside connArgs)
	AMProcessVariablePrivate* myPV = reinterpret_cast<AMProcessVariablePrivate*>( ca_puser(eventArgs.chid) );
	if(myPV)
		myPV->valueChangedCB(eventArgs);

}

void AMProcessVariablePrivate::PVAlarmChangedCBWrapper(event_handler_args eventArgs) {
	// dig the instance pointer out of the puser field of the chid (inside connArgs)
	AMProcessVariablePrivate* myPV = reinterpret_cast<AMProcessVariablePrivate*>( ca_puser(eventArgs.chid) );
	if(myPV)
		myPV->alarmChangedCB(eventArgs);
}

void AMProcessVariablePrivate::PVControlInfoCBWrapper(struct event_handler_args eventArgs) {

	// dig the instance pointer out of the puser field of the chid (inside connArgs)
	AMProcessVariablePrivate* myPV = reinterpret_cast<AMProcessVariablePrivate*>( ca_puser(eventArgs.chid) );
	if(myPV)
		myPV->controlInfoCB(eventArgs);

}

void AMProcessVariablePrivate::PVPutRequestCBWrapper(struct event_handler_args eventArgs) {

	// dig the instance pointer out of the puser field of the chid (inside connArgs)
	AMProcessVariablePrivate* myPV = reinterpret_cast<AMProcessVariablePrivate*>( ca_puser(eventArgs.chid) );
	if(myPV)
		myPV->putRequestCB(eventArgs);

}
/////////////////////


// Callbacks for events:
/////////////////////
void AMProcessVariablePrivate::exceptionCB(struct exception_handler_args args) {



	emit internal_error(args.stat);	// will notify eventually with error() signals

	AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_EPICS_EXCEPTION_EXCEPTIONCB, QString("AMProcessVariable: EPICS exception: %1\n  Operation: %2\n  Channel: %3\n  Data type: %4\n  Count: %5\n\n  Epics says: %6\n").arg(ca_message(args.stat)).arg(args.op).arg(pvName()).arg(dbr_type_to_text ( args.type )).arg(args.count).arg(args.ctx));
	ca_signal( args.stat, args.ctx );

}

void AMProcessVariablePrivate::connectionChangedCB(struct connection_handler_args connArgs) {


	int lastError;
	chtype serverType;
	PVDataType::Type ourType;

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
		ourType = AMProcessVariable::serverType2ourType(serverType);

		// Request some information about the PV (units, control limits, etc.).
		// For enum types, we want the list of string names:
		if(ourType == PVDataType::Enum) {
			lastError = ca_get_callback(DBR_CTRL_ENUM, chid_, PVControlInfoCBWrapper, this);

			if(lastError != ECA_NORMAL) {
				AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_ERROR_WHILE_REQUESTING_ENUM_INFO, QString("AMProcessVariable: Error while trying to request enum control information: %1: %2").arg(pvName()).arg(ca_message(lastError)));
				emit internal_error(lastError);
			}
		}
		// otherwise, requesting control information as DBR_CTRL_DOUBLE because this gives the most information that could possibly be available (precision, limits, units)
		else if (ourType == PVDataType::Integer || ourType == PVDataType::FloatingPoint){
			lastError = ca_get_callback(DBR_CTRL_DOUBLE, chid_, PVControlInfoCBWrapper, this);
			if(lastError != ECA_NORMAL) {
				AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_ERROR_WHILE_REQUESTING_INT_VALUE, QString("AMProcessVariable: Error while trying to request value: %1: %2").arg(pvName()).arg(ca_message(lastError)));
				emit internal_error(lastError);
			}
		}
		else if(ourType == PVDataType::String){
			lastError = ca_get_callback(DBR_CTRL_STRING, chid_, PVControlInfoCBWrapper, this);
			if(lastError != ECA_NORMAL) {
				AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_ERROR_WHILE_REQUESTING_STRING_VALUE, QString("AMProcessVariable: Error while trying to request value: %1: %2").arg(pvName()).arg(ca_message(lastError)));
				emit internal_error(lastError);
			}
		}
		/// \todo What control type to request if type is not any of these?
		/// \bug Currently, if the type is not an enum, integer, floating-point, or string, PV's will not emit initialized().

		// It's useful to automatically-request the value, after we are first connected:
		lastError = ca_array_get_callback(ourType, ca_element_count(chid_), chid_, PVValueChangedCBWrapper, this);
		if(lastError != ECA_NORMAL) {
			AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_ERROR_WHILE_REQUESTING_VALUE_GENERAL, QString("AMProcessVariable: Error while trying to request value: %1: %2").arg(pvName()).arg(ca_message(lastError)));
			emit internal_error(lastError);
		}

		// all connections will monitor the alarm status, even if they don't want to monitor the values
		lastError = ca_create_subscription(AMProcessVariable::serverType2StatusType(serverType), 1, chid_, DBE_ALARM, PVAlarmChangedCBWrapper, this, &alarmEvid_);
		if(lastError != ECA_NORMAL) {
			AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_ERROR_WHILE_SUBSCRIBING_TO_ALARMS, QString("AMProcessVariable: Error while trying to subscribe to alarms: %1: %2").arg(pvName()).arg(ca_message(lastError)));
			emit internal_error(lastError);
		}

		// start monitoring values, if we're supposed to be.
		if(shouldBeMonitoring_) {
			lastError = ca_create_subscription(ourType, ca_element_count(chid_), chid_, DBE_VALUE | DBE_LOG | DBE_ALARM, PVValueChangedCBWrapper, this, &evid_ );
			if(lastError != ECA_NORMAL) {
				AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_ERROR_WHILE_STARTING_MONITORING, QString("AMProcessVariable: Error starting monitoring: %1: %2").arg(pvName()).arg(ca_message(lastError)));
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


void AMProcessVariablePrivate::controlInfoCB(struct event_handler_args eventArgs) {


	int lastError;

	QStringList enumStrings;

	struct dbr_ctrl_double* ctrlValue;
	struct dbr_ctrl_enum* enumCtrlValue;

	if( (lastError = eventArgs.status) != ECA_NORMAL) {
		AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_ERROR_IN_CONTROL_INFO_CHANGED_CB, QString("Error in control-info-changed callback: %1: %2").arg(pvName()).arg(ca_message(lastError)));
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

/*
eventArgs:
void            *usr;   user argument supplied with request
chanId          chid;   channel id
long            type;   the type of the item returned
long            count;  the element count of the item returned
const void      *dbr;   a pointer to the item returned
int             status; ECA_XXX status of the requested op from the server

*/

void AMProcessVariablePrivate::valueChangedCB(struct event_handler_args eventArgs) {


	int lastError;
	if( (lastError = eventArgs.status) != ECA_NORMAL) {
		AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_ERROR_IN_VALUE_CHANGED_CB, QString("Error in value-changed callback: %1: %2").arg(pvName()).arg(ca_message(lastError)));
		emit internal_error(lastError);
		return;
	}

	if(eventArgs.dbr == 0)
		return;

	// Handle this based on the type of information we're getting:
	// Note: the onConnectionChanged handler should have got the vector sizes right before we get any of this data.
	switch(eventArgs.type) {

	case PVDataType::FloatingPoint: {
			AMProcessVariableDoubleVector rv(eventArgs.count);
			for(int i=0; i<eventArgs.count; i++)
				rv[i] = ((dbr_double_t*)eventArgs.dbr)[i];

			emit internal_floatingPointValueChanged(rv);
			break;
		}

	case PVDataType::Integer: {
			AMProcessVariableIntVector rv(eventArgs.count);
			for(int i=0; i<eventArgs.count; i++)
				rv[i] = ((dbr_long_t*)eventArgs.dbr)[i];
			emit internal_integerValueChanged(rv);
			break;
		}

	case PVDataType::Enum: {
			AMProcessVariableIntVector rv(eventArgs.count);
			for(int i=0; i<eventArgs.count; i++)
				rv[i] = ((dbr_enum_t*)eventArgs.dbr)[i];
			emit internal_enumValueChanged(rv);
			break;
		}

	case PVDataType::String: {
			QStringList rv;
			for(int i=0; i<eventArgs.count; i++)
				rv << QString( ((dbr_string_t*)eventArgs.dbr)[i] );
			emit internal_stringValueChanged(rv);
			break;
		}
	}
}


void AMProcessVariablePrivate::alarmChangedCB(struct event_handler_args eventArgs) {


	int lastError;
	if( (lastError = eventArgs.status) != ECA_NORMAL) {
		AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_ERROR_IN_ALARM, QString("Error in alarm-changed callback: %1: %2").arg(pvName()).arg(ca_message(lastError)));
		emit internal_error(lastError);
		return;
	}

	if(eventArgs.dbr == 0)
		return;

	// Handle this based on the type of information we're getting:
	switch(eventArgs.type) {

	case DBR_STS_DOUBLE: {
			dbr_sts_double* dbr = (dbr_sts_double*)eventArgs.dbr;
			emit internal_alarmChanged(dbr->status, dbr->severity);
			break;
		}

	case DBR_STS_LONG: {
			dbr_sts_long* dbr = (dbr_sts_long*)eventArgs.dbr;
			emit internal_alarmChanged(dbr->status, dbr->severity);
			break;
		}

	case DBR_STS_ENUM: {
			dbr_sts_enum* dbr = (dbr_sts_enum*)eventArgs.dbr;
			emit internal_alarmChanged(dbr->status, dbr->severity);
			break;
		}

	case DBR_STS_STRING: {
			dbr_sts_string* dbr = (dbr_sts_string*)eventArgs.dbr;
			emit internal_alarmChanged(dbr->status, dbr->severity);
			break;
		}
	}
}


void AMProcessVariablePrivate::putRequestCB(struct event_handler_args eventArgs) {

	if(eventArgs.status != ECA_NORMAL) {


		AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_ERROR_IN_PUT_REQUEST, QString("AMProcessVariable: Error in put request: %1: %2").arg(pvName()).arg(ca_message(eventArgs.status)));
		emit internal_error(eventArgs.status);
	}

	emit putRequestReturned(eventArgs.status);
}

void AMProcessVariablePrivate::internal_onConnectionStateChanged(bool isConnected, int count, int serverType, int ourType) {

	if(isConnected) {
		count_ = count;
		serverType_ = serverType;
		ourType_ = (PVDataType::Type)ourType;
		ourAlarmType_ = AMProcessVariable::serverType2StatusType(serverType);
	}

	else {
		initialized_ = false;
		if(hasValues_)
			emit hasValuesChanged(false);
		hasValues_ = false;
	}

	emit connected(isConnected);
}


void AMProcessVariablePrivate::internal_onControlInfoChanged(int controlInfoType, QString units, int precision, double upperGraphLimit, double lowerGraphLimit, double upperLimit, double lowerLimit, QStringList enumStrings) {

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

void AMProcessVariablePrivate::internal_onFloatingPointValueChanged(AMProcessVariableDoubleVector doubleData) {
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

void AMProcessVariablePrivate::internal_onIntegerValueChanged(AMProcessVariableIntVector intData) {
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

void AMProcessVariablePrivate::internal_onEnumValueChanged(AMProcessVariableIntVector enumData) {
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


void AMProcessVariablePrivate::internal_onStringValueChanged(QStringList stringData) {
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

void AMProcessVariablePrivate::internal_onAlarmChanged(int status, int severity) {
	bool hasChanged = (status != alarmStatus_ || severity != alarmSeverity_);
	alarmStatus_ = status;
	alarmSeverity_ = severity;
	if(hasChanged)
		emit alarmChanged(alarmStatus_, alarmSeverity_);
}


void AMProcessVariablePrivate::checkReadWriteReady(){
	// We aren't readReady, but the only problem is ca_read_access failed
	if(!readReady() && isConnected() && isInitialized() && hasValues()){
		emit error(AMPROCESSVARIABLE_CANNOT_READ);
	}
	// We aren't writeReady, but the only problem is ca_write_access failed
	if(!writeReady() && readReady()){
		emit error(AMPROCESSVARIABLE_CANNOT_WRITE);
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


void AMProcessVariablePrivate::startMonitoring() {

	if(!channelCreated_ || !isConnected()) {
		AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_ERROR_WHILE_STARTING_MONITOR_NOT_CONNECTED, QString("AMProcessVariable: Error starting monitoring PV '%1' because it is not connected yet.").arg(pvName()));
	}

	lastError_ = ca_create_subscription(ourType_, ca_element_count(chid_), chid_, DBE_VALUE | DBE_LOG | DBE_ALARM, PVValueChangedCBWrapper, this, &evid_ );
	if(lastError_ != ECA_NORMAL) {
		AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_ERROR_WHILE_STARTING_MONITOR_GENERAL, QString("AMProcessVariable: Error starting monitoring: %1: %2").arg(pvName()).arg(ca_message(lastError_)));
		emit error(lastError_);
		return;
	}

	AMProcessVariableSupport::flushIO();
}

void AMProcessVariablePrivate::stopMonitoring() {

	if(!channelCreated_ || !isConnected()) {
		return;
	}

	if(evid_ == 0) {
		AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_CANNOT_STOP_MONITORING_NEVER_STARTED, QString("AMProcessVariable: Cannot stop monitoring %1, because it hasn't started monitoring yet.").arg(pvName()));
		return;
	}

	ca_clear_subscription(evid_);
	AMProcessVariableSupport::flushIO();
	evid_ = 0;
}

void AMProcessVariablePrivate::reviewMonitoring()
{
	bool shouldBeMonitoring = false;
	foreach(AMProcessVariable* pv, attachedProcessVariables_) {
		if(pv->shouldBeMonitoring_) {
			shouldBeMonitoring = true;
			break;
		}
	}

	shouldBeMonitoring_ = shouldBeMonitoring;

	// do we need to start? If connected, we can now.  If not connected, we'll start automatically when we do connect.
	if(shouldBeMonitoring_ && !isMonitoring() && channelCreated_ && isConnected())
		startMonitoring();
	// do we need to stop?
	else if(!shouldBeMonitoring_ && isMonitoring())
		stopMonitoring();
}


bool AMProcessVariablePrivate::requestValue(int numberOfValues) {

	lastError_ = ca_array_get_callback(ourType_, numberOfValues, chid_, PVValueChangedCBWrapper, this);
	if(lastError_ != ECA_NORMAL) {
		AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_ERROR_WHILE_REQUESTING_VALUE, QString("AMProcessVariable: Error while trying to request value: %1: %2").arg(pvName()).arg(ca_message(lastError_)));
		emit error(lastError_);
		return false;
	}

	AMProcessVariableSupport::flushIO();

	return true;
}

void AMProcessVariablePrivate::setValue(int value) {

	dbr_long_t setpoint = value;

	if (!putCallbackEnabled_)
		lastError_ = ca_put( DBR_LONG, chid_, &setpoint );
	else
		lastError_ = ca_put_callback( DBR_LONG, chid_, &setpoint, PVPutRequestCBWrapper, this );

	if(lastError_ != ECA_NORMAL) {
		AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_ERROR_WHILE_PUTTING_VALUE_INTEGER, QString("AMProcessVariable: Error while trying to put value: %1: %2").arg(pvName()).arg(ca_message(lastError_)));
		emit error(lastError_);
	}

	AMProcessVariableSupport::flushIO();
}

void AMProcessVariablePrivate::setValues(dbr_long_t setpoints[], int num) {

	if (!putCallbackEnabled_)
		lastError_ = ca_array_put( DBR_LONG, num, chid_, setpoints );
	else
		lastError_ = ca_array_put_callback( DBR_LONG, num, chid_, setpoints, PVPutRequestCBWrapper, this );

	if(lastError_ != ECA_NORMAL) {
		AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_ERROR_WHILE_PUTTING_VALUES_INTERGER, QString("AMProcessVariable: Error while trying to put values: %1: %2").arg(pvName()).arg(ca_message(lastError_)));
		emit error(lastError_);
	}

	AMProcessVariableSupport::flushIO();
}

void AMProcessVariablePrivate::setValue(double value) {

	dbr_double_t setpoint = value;

	if (!putCallbackEnabled_)
		lastError_ = ca_put( DBR_DOUBLE, chid_, &setpoint );
	else
		lastError_ = ca_put_callback( DBR_DOUBLE, chid_, &setpoint, PVPutRequestCBWrapper, this );

	if(lastError_ != ECA_NORMAL) {
		AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_ERROR_WHILE_PUTTING_VALUE_DOUBLE, QString("AMProcessVariable: Error while trying to put value: %1: %2").arg(pvName()).arg(ca_message(lastError_)));
		emit error(lastError_);
	}

	AMProcessVariableSupport::flushIO();
}

void AMProcessVariablePrivate::setValues(dbr_double_t setpoints[], int num) {

	if (!putCallbackEnabled_)
		lastError_ = ca_array_put( DBR_DOUBLE, num, chid_, setpoints );
	else
		lastError_ = ca_array_put_callback( DBR_DOUBLE, num, chid_, setpoints, PVPutRequestCBWrapper, this );

	if(lastError_ != ECA_NORMAL) {
		AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_ERROR_WHILE_PUTTING_VALUES_DOUBLE, QString("AMProcessVariable: Error while trying to put values: %1: %2").arg(pvName()).arg(ca_message(lastError_)));
		emit error(lastError_);
	}

	AMProcessVariableSupport::flushIO();
}

void AMProcessVariablePrivate::setValue(const QString& value) {

	dbr_string_t setpoint;
	QByteArray d1 = value.toAscii();
	strcpy(setpoint, d1.constData());

	if (!putCallbackEnabled_)
		lastError_ = ca_put( DBR_STRING, chid_, setpoint );
	else
		lastError_ = ca_put_callback( DBR_STRING, chid_, setpoint, PVPutRequestCBWrapper, this );

	if(lastError_ != ECA_NORMAL) {
		AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_ERROR_WHILE_PUTTING_VALUE_STRING, QString("AMProcessVariable: Error while trying to put value: %1: %2").arg(pvName()).arg(ca_message(lastError_)));
		emit error(lastError_);
	}

	AMProcessVariableSupport::flushIO();
}

void AMProcessVariablePrivate::setValues(const QStringList& setpoints) {


	QList<QByteArray> asciiData;	// will hold the ascii form of our strings
	const char** stringArray = new const char*[setpoints.size()];		// an array of strings (array of char*... ie: char**) which ca_array_put_callback requires.

	for(int i=0; i<setpoints.size(); i++) {
		asciiData.append( setpoints[i].toAscii() );
		stringArray[i] = asciiData[i].constData();
	}

	if (!putCallbackEnabled_)
		lastError_ = ca_array_put( DBR_STRING, setpoints.size(), chid_, stringArray );
	else
		lastError_ = ca_array_put_callback( DBR_STRING, setpoints.size(), chid_, stringArray, PVPutRequestCBWrapper, this );

	if(lastError_ != ECA_NORMAL) {
		AMErrorMon::debug(this, AMPROCESSVARIABLESUPPORT_ERROR_WHILE_PUTTING_VALUES_STRING, QString("AMProcessVariable: Error while trying to put values: %1: %2").arg(pvName()).arg(ca_message(lastError_)));
		emit error(lastError_);
	}
	delete[] stringArray;

	AMProcessVariableSupport::flushIO();

}


double AMProcessVariablePrivate::lastValue(unsigned index) const {

	// Depending on our type, we need to know where to look and how to convert:
	switch(ourType_) {

	case PVDataType::FloatingPoint:
		if(index >= (unsigned)data_dbl_.count())
			return -1.0;
		return data_dbl_.at(index);

	case PVDataType::Integer:
	case PVDataType::Enum:
		if(index >= (unsigned)data_int_.count())
			return -1.0;
		return (double)data_int_.at(index);

	case PVDataType::String:
		if(index >= (unsigned)data_str_.count())
			return -1.0;
		return data_str_.at(index).toDouble();

		// default case, including Unconnected:
	default:
		return -1.0;
	}
}



int AMProcessVariablePrivate::getInt(unsigned index) const {



	// Depending on our type, we need to know where to look and how to convert:
	switch(ourType_) {

	case PVDataType::FloatingPoint:
		if(index >= (unsigned)data_dbl_.count())
			return -1;
		return (int)data_dbl_.at(index);

	case PVDataType::Integer:
	case PVDataType::Enum:
		if(index >= (unsigned)data_int_.count())
			return -1;
		return data_int_.at(index);

	case PVDataType::String:
		if(index >= (unsigned)data_str_.count())
			return -1;
		return data_str_.at(index).toInt();

		// default case, including Unconnected:
	default:
		return -1;
	}
}


QString AMProcessVariablePrivate::getString(unsigned index) const {



	// Depending on our type, we need to know where to look and how to convert:
	switch(ourType_) {

	case PVDataType::FloatingPoint:
		if(index >= (unsigned)data_dbl_.count())
			return "[array index out of range]";
		return QString("%1").arg(data_dbl_.at(index));

	case PVDataType::Integer:
		if(index >= (unsigned)data_int_.count())
			return "[array index out of range]";
		return QString("%1").arg(data_int_.at(index));

	case PVDataType::Enum:
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


	case PVDataType::String:
		if(index >= (unsigned)data_str_.count())
			return "[array index out of range]";
		return data_str_.at(index);

		// default case, including Unconnected:
	default:
		return "[PV not connected]";
	}
}

QString AMProcessVariablePrivate::errorString(int errorCode)
{
	switch(errorCode){
	case AMPROCESSVARIABLE_CANNOT_READ:
		return "Lacking read access permission to Process Variable.";
	case AMPROCESSVARIABLE_CANNOT_WRITE:
		return "Lacking write access permission to Process Variable.";
	default:
		 return QString(ca_message(errorCode));
	}
}

