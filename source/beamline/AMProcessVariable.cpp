#include "AMProcessVariable.h"
#include <float.h>
#include <QDebug>

///////////////////////////////
// AMProcessVariableHeartbeat
///////////////////////////////

AMProcessVariableHeartbeat* AMProcessVariableHeartbeat::instance_ = 0;

// constructor: initializes channel access, starts the ca_poll timer, and installs us as the global exception handler.
AMProcessVariableHeartbeat::AMProcessVariableHeartbeat() : QObject() {

	qDebug("Starting up channel access...");
	int lastError = ca_context_create(ca_disable_preemptive_callback);
	if(lastError != ECA_NORMAL )
		throw lastError;

	lastError = ca_add_exception_event(PVExceptionCB, 0 );
	if(lastError != ECA_NORMAL )
		throw lastError;

	timerId_ = startTimer( PV_HEARTBEAT_MS );

}

// standard singleton-pattern getInstance() method.
AMProcessVariableHeartbeat* AMProcessVariableHeartbeat::getInstance() {

	if (instance_ == 0)  { // is it the first call?
		instance_ = new AMProcessVariableHeartbeat(); // create sole instance
	}

	return instance_;
}

// the implementation of AMProcessVariableHeartbeat::removePV():
void AMProcessVariableHeartbeat::removePVImplementation(chid c) {

	// unregister this channel:
	map_.remove(int(c));

	// if that was the last one out, tear down Channel Access:
	if(map_.count() == 0) {

		qDebug("Shutting down channel access...");
		killTimer(timerId_);			// stop the ca_poll() timer.
		ca_add_exception_event(0, 0);	// return the default exception handler
		ca_context_destroy();			// shut down Channel Access
		instance_ = 0;					// We are no more...
		deleteLater();					// We're gone.
	}
}

void AMProcessVariableHeartbeat::PVExceptionCB(struct exception_handler_args args) {

	// If there's a specific channel, pass it on to the channel's exception handler:
	if(args.chid && getInstance()->map_.contains(int(args.chid)) ) {

		getInstance()->map_.value(int(args.chid))->exceptionCB(args);

	}

	// otherwise handle it here:
	else {

		char buf[512];
		sprintf ( buf, "AMProcessVariableHeartbeat: Epics exception: %s - with request op=%d data type=%s count=%d", args.ctx, (int)args.op, dbr_type_to_text ( args.type ), (int)args.count );
		qDebug() << buf;
		ca_signal ( args.stat, buf );
	}

}



AMProcessVariable::AMProcessVariable(const QString& pvName, bool autoMonitor, QObject* parent, int timeoutMs) : QObject(parent), shouldBeMonitoring_(autoMonitor) {

	setObjectName("AMProcessVariable_" + pvName);

	// Install convenience signal generators:
	connect(this, SIGNAL(error(int)), this, SLOT(signalForwardOnError(int)));
	connect(this, SIGNAL(connected(bool)), this, SLOT(signalForwardOnConnected(bool)));

	// This PV is not initialized yet:
	initialized_ = false;
	serverType_ = Unconnected;
	ourType_ = Unconnected;

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

	try {

		AMProcessVariableHeartbeat::ensureChannelAccess();

		// attempt to search/connect:
		lastError_ = ca_create_channel (pvName.toAscii().constData(), PVConnectionChangedCBWrapper, this, CA_PRIORITY_DEFAULT, &chid_ );
		if(lastError_ != ECA_NORMAL)
			throw lastError_;

		// This will notice if the search times out:
		QTimer::singleShot(timeoutMs, this, SLOT(onConnectionTimeout()));

		// register ourself to the support class:
		AMProcessVariableHeartbeat::registerPV(chid_, this);
		// qDebug() << QString("AMProcessVariable: Creating AMProcessVariable %1").arg(pvName);

	}

	catch(int s) {

		qDebug() << QString("AMProcessVariable: Error initializing AMProcessVariable: %1: %2").arg(pvName).arg(ca_message(lastError_));
		emit error(s);
	}
}


AMProcessVariable::~AMProcessVariable() {

	// qDebug() << QString("deleting AMProcessVariable %1.").arg(pvName());

	emit disconnected();
	emit connected(false);

	// This is unnecessary... called auto. by ca_clear_channel():
	// ca_clear_subscription(evid_);

	ca_clear_channel(chid_);

	// deregister ourself from the support class:
	AMProcessVariableHeartbeat::removePV(chid_);

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

	// One thing for sure:
	emit error(lastError_ = args.stat);

	qDebug() << QString("AMProcessVariable: EPICS exception: %1\n  Operation: %2\n  Channel: %3\n  Data type: %4\n  Count: %5\n\n  Epics says: %6\n").arg(ca_message(args.stat)).arg(args.op).arg(pvName()).arg(dbr_type_to_text ( args.type )).arg(args.count).arg(args.ctx);
	ca_signal( args.stat, args.ctx );

}

void AMProcessVariable::connectionChangedCB(struct connection_handler_args connArgs) {

	emit connectionStateChanged( ca_state(connArgs.chid) );

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

		emit connected(true);

		// Discover the type of this channel:
		serverType_ = ca_field_type(chid_);

		qDebug() << "Type of channel" << this->pvName() << "is " << serverType_;

		// We simplify all floating-point types to double, all integer types to long, and leave strings as strings and enums as enums:
		ourType_ = serverType2ourType(serverType_);

		// Make sure we have room for storage. Integers and Enums held in data_int_.  FloatingPoints in data_dbl_.
		switch(ourType_) {
		case Integer:
		case Enum:
			data_int_.resize(count());
			break;
		case FloatingPoint:
			data_dbl_.resize(count());
			break;
		case String:
			data_str_.clear();
			break;
		case Unconnected:
			break;// won't happen, but if it does, nothing required.
		}

		// Request some information about the PV (units, control limits, etc.).
		// For enum types, we want the list of string names:
		if(ourType_ == Enum) {
			lastError_ = ca_get_callback(DBR_CTRL_ENUM, chid_, PVControlInfoCBWrapper, this);

			if(lastError_ != ECA_NORMAL) {
				qDebug() << QString("AMProcessVariable: Error while trying to request enum control information: %1: %2").arg(pvName()).arg(ca_message(lastError_));
				emit error(lastError_);
			}
		}
		// otherwise, requesting control information as DBR_CTRL_DOUBLE because this gives the most information that could possibly be available (precision, limits, units)
		else if (ourType_ == Integer || ourType_ == FloatingPoint){
			lastError_ = ca_get_callback(DBR_CTRL_DOUBLE, chid_, PVControlInfoCBWrapper, this);
			if(lastError_ != ECA_NORMAL) {
				qDebug() << QString("AMProcessVariable: Error while trying to request value: %1: %2").arg(pvName()).arg(ca_message(lastError_));
				emit error(lastError_);
			}
		}
		else if(ourType_ == String){
			lastError_ = ca_get_callback(DBR_CTRL_STRING, chid_, PVControlInfoCBWrapper, this);
			if(lastError_ != ECA_NORMAL) {
				qDebug() << QString("AMProcessVariable: Error while trying to request value: %1: %2").arg(pvName()).arg(ca_message(lastError_));
				emit error(lastError_);
			}
		}
		/// \todo What control type to request if type is not any of these?
		/// \bug Currently, if the type is not an enum, integer, floating-point, or string, PV's will not emit initialized().

		// It's useful to automatically-request the value, after we are first connected:
		this->requestValue(count());

		if(shouldBeMonitoring_)
			startMonitoring();
	}
	// Possibility 2: anything but a connection gained:
	else {
		emit connected(false);
		initialized_ = false;
		serverType_ = Unconnected;
		ourType_ = Unconnected;
	}

}


void AMProcessVariable::controlInfoCB(struct event_handler_args eventArgs) {

	struct dbr_ctrl_double* ctrlValue;
	struct dbr_ctrl_enum* enumCtrlValue;

	if( (lastError_ = eventArgs.status) != ECA_NORMAL) {
		qDebug() << QString("Error in control-info-changed callback: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
		return;
	}

	if(eventArgs.dbr == 0)
		return;

	// Handle this based on the type of information we're getting:
	switch(eventArgs.type) {

		// Is this limit-precision-unit information?
	case DBR_CTRL_DOUBLE:
		ctrlValue =  (struct dbr_ctrl_double*)eventArgs.dbr;

		units_ = QString(ctrlValue->units);
		precision_ = ctrlValue->precision;
		upperGraphLimit_ = ctrlValue->upper_disp_limit;
		lowerGraphLimit_ = ctrlValue->lower_disp_limit;
		//If the record's DRV_HIGH and DRV_LOW limits haven't been set, we're receiving them as max(0), min(0).
		// That's a problem, because it pins our minValue() and maxValue() to 0.
		if(ctrlValue->lower_ctrl_limit == 0 && ctrlValue->upper_ctrl_limit == 0) {
			lowerLimit_ = -DBL_MAX;
			upperLimit_ = DBL_MAX;
		}
		else {
			lowerLimit_ = ctrlValue->lower_ctrl_limit;
			upperLimit_ = ctrlValue->upper_ctrl_limit;
		}
		enumStrings_.clear();
		initialized_ = true;
		emit initialized();
		break;

	// Is this enum count/string information?
	case DBR_CTRL_ENUM:
		enumCtrlValue = (struct dbr_ctrl_enum*)eventArgs.dbr;

		units_ = QString("[choice]");
		precision_ = 0;
		lowerGraphLimit_ = lowerLimit_ = 0;
		upperGraphLimit_ = upperLimit_ = enumCtrlValue->no_str - 1;	// limits are inclusive. Check for outside limits with > and < (not >=, <=)
		enumStrings_.clear();
		for(int i=0; i<enumCtrlValue->no_str; i++)
			enumStrings_ << QString(enumCtrlValue->strs[i]);

		initialized_ = true;
		emit initialized();
		break;

	case DBR_CTRL_STRING:
		units_ = QString();
		precision_ = 0;
		lowerLimit_ = -DBL_MAX;
		upperLimit_ = DBL_MAX;
		enumStrings_.clear();
		initialized_ = true;
		emit initialized();
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


	if( (lastError_ = eventArgs.status) != ECA_NORMAL) {
		qDebug() << QString("Error in value-changed callback: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
		return;
	}

	if(eventArgs.dbr == 0)
		return;

	// Handle this based on the type of information we're getting:
	// Note: the onConnectionChanged handler should have got the vector sizes right before we get any of this data.
	switch(eventArgs.type) {

	case FloatingPoint:
		for(int i=0; i<eventArgs.count; i++)
			data_dbl_[i] = ((dbr_double_t*)eventArgs.dbr)[i];
		emit valueChanged(data_dbl_.at(0));
		emit valueChanged(int(data_dbl_.at(0)));
		emit valueChanged();
		break;

	case Integer:
		for(int i=0; i<eventArgs.count; i++)
			data_int_[i] = ((dbr_long_t*)eventArgs.dbr)[i];
		emit valueChanged(data_int_.at(0));
		emit valueChanged(double(data_int_.at(0)));
		emit valueChanged();
		break;

	case Enum:
		for(int i=0; i<eventArgs.count; i++)
			data_int_[i] = ((dbr_enum_t*)eventArgs.dbr)[i];
		emit valueChanged(data_int_.at(0));
		emit valueChanged(double(data_int_.at(0)));
		if(initialized_)
			emit valueChanged(enumStrings_.at(data_int_.at(0)));
		emit valueChanged();
		break;

	case String:
		data_str_.clear();
		for(int i=0; i<eventArgs.count; i++)
			data_str_ << QString( ((dbr_string_t*)eventArgs.dbr)[i] );
		emit valueChanged(data_str_.at(0));
		emit valueChanged();
		break;
	}
}



void AMProcessVariable::putRequestCB(struct event_handler_args eventArgs) {

	emit putRequestReturned(eventArgs.status);

	if(eventArgs.status != ECA_NORMAL) {
		qDebug() << QString("AMProcessVariable: Error in put request: %1: %2").arg(pvName()).arg(ca_message(eventArgs.status));
		emit error(lastError_ = eventArgs.status);
	}

}

void AMProcessVariable::onConnectionTimeout() {

	// If we haven't connected by now:
	if(this->connectionState() != cs_conn) {
		qDebug() << QString("AMProcessVariable: channel connect timed out for %1").arg(pvName());
		emit connectionTimeout();
	}

	// Don't call ca_clear_channel here.  Channel access will keep on trying in the background. (Channel access handles this automatically for us.)  Will emit connected() signal when successful.
}


bool AMProcessVariable::startMonitoring() {

	lastError_ = ca_create_subscription(dataType(), count(), chid_, DBE_VALUE | DBE_LOG | DBE_ALARM, PVValueChangedCBWrapper, this, &evid_ );
	if(lastError_ != ECA_NORMAL) {
		qDebug() << QString("AMProcessVariable: Error starting monitoring: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
		return false;
	}

	return true;
}

void AMProcessVariable::stopMonitoring() {

	ca_clear_subscription(evid_);
}

bool AMProcessVariable::requestValue(int numberOfValues) {

	// Not necessary. Connection status is checked by ca_array_get_callback:
	//if(ca_state(chid_) != cs_conn) {
	//	qDebug(QString("Error requesting value: channel not connected: %1").arg(pvName()));
	//	return false;
	//}

	lastError_ = ca_array_get_callback(dataType(), numberOfValues, chid_, PVValueChangedCBWrapper, this);
	if(lastError_ != ECA_NORMAL) {
		qDebug() << QString("AMProcessVariable: Error while trying to request value: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
		return false;
	}

	return true;
}

void AMProcessVariable::setValue(int value) {

	dbr_long_t setpoint = value;

	if (disablePutCallback_)
		lastError_ = ca_put( DBR_LONG, chid_, &setpoint );
	else
		lastError_ = ca_put_callback( DBR_LONG, chid_, &setpoint, PVPutRequestCBWrapper, this );

	if(lastError_ != ECA_NORMAL) {
		qDebug() << QString("AMProcessVariable: Error while trying to put value: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
	}
}

void AMProcessVariable::setValues(dbr_long_t setpoints[], int num) {

	if (disablePutCallback_)
		lastError_ = ca_array_put( DBR_LONG, num, chid_, setpoints );
	else
		lastError_ = ca_array_put_callback( DBR_LONG, num, chid_, setpoints, PVPutRequestCBWrapper, this );

	if(lastError_ != ECA_NORMAL) {
		qDebug() << QString("AMProcessVariable: Error while trying to put values: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
	}
}

void AMProcessVariable::setValue(double value) {

	dbr_double_t setpoint = value;

	if (disablePutCallback_)
		lastError_ = ca_put( DBR_DOUBLE, chid_, &setpoint );
	else
		lastError_ = ca_put_callback( DBR_DOUBLE, chid_, &setpoint, PVPutRequestCBWrapper, this );

	if(lastError_ != ECA_NORMAL) {
		qDebug() << QString("Error while trying to put AMProcessVariable value: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
	}
}

void AMProcessVariable::setValues(dbr_double_t setpoints[], int num) {

	if (disablePutCallback_)
		lastError_ = ca_array_put( DBR_DOUBLE, num, chid_, setpoints );
	else
		lastError_ = ca_array_put_callback( DBR_DOUBLE, num, chid_, setpoints, PVPutRequestCBWrapper, this );

	if(lastError_ != ECA_NORMAL) {
		qDebug() << QString("Error while trying to put AMProcessVariable values: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
	}
}

void AMProcessVariable::setValue(const QString& value) {

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
}

// TODO: not sure if this is right...
void AMProcessVariable::setValues(const QStringList& setpoints) {

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
		if(initialized_)	// If we have the String name for this enum value:
			return enumStrings_.at(data_int_.at(index));
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



