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

        setObjectName("AMProcessVariable_"+pvName);

	// Install convenience signal generators:
	connect(this, SIGNAL(error(int)), this, SLOT(signalForwardOnError(int)));
	connect(this, SIGNAL(connected(bool)), this, SLOT(signalForwardOnConnected(bool)));

	// Initialize information in ctrlValue_:
	strcpy(ctrlValue_.units, "?");
	// Limits are initialized to the widest possible range at the start. Is this safe? Don't know what else to use, until we find out.
	ctrlValue_.lower_alarm_limit = ctrlValue_.lower_ctrl_limit = -DBL_MAX;
	ctrlValue_.upper_alarm_limit = ctrlValue_.upper_ctrl_limit = DBL_MAX;
	// TODO: enable other fields of ctrlValue_ for people that want it.

	try {

                AMProcessVariableHeartbeat::ensureChannelAccess();
		
		// attempt to search/connect:
		lastError_ = ca_create_channel (pvName.toAscii().constData(), PVConnectionChangedCBWrapper, this, CA_PRIORITY_DEFAULT, &chid_ );
		if(lastError_ != ECA_NORMAL)
			throw lastError_;

		// This will notice if the search times out:
		connect(&startupTimer_, SIGNAL(timeout()), this, SLOT(onConnectionTimeout()));
		startupTimer_.start(timeoutMs);
		
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
         // Missing... What state is it when IOC not found, connect times out?
         //

	if( ca_state(connArgs.chid) == cs_conn && connArgs.op == CA_OP_CONN_UP) {
		emit connected(true);

		// Cancel the connection-timeout timer. We are connected.
		startupTimer_.stop();
		disconnect(&startupTimer_, SIGNAL(timeout()), this, SLOT(onConnectionTimeout()));

		// Request some information about the PV (units, control limits, etc.). Requesting as DBR_CTRL_DOUBLE because this gives the most information.
		lastError_ = ca_get_callback(DBR_CTRL_DOUBLE, chid_, PVValueChangedCBWrapper, this);
		if(lastError_ != ECA_NORMAL) {
                        qDebug() << QString("AMProcessVariable: Error while trying to request value: %1: %2").arg(pvName()).arg(ca_message(lastError_));
			emit error(lastError_);
		}

		// It's useful to automatically-request the value, after we are first connected:
		this->requestValue(1);

		if(shouldBeMonitoring_)
			startMonitoring();
	}
	else {
		emit connected(false);
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
		startupTimer_.stop();
		emit connectionTimeout();
	}

	// TODO: should we ca_clear_channel here and give up permanently? Or keep trying?
}

///////////////////


bool AMProcessVariable::startMonitoring() {
	
        // Not necessary. Connection status is checked by ca_create_subs:
        //if( ca_state(chid_) != cs_conn) {
        //	qDebug(QString("Error starting AMProcessVariable monitoring: channel not connected: %1").arg(pvName());
        //	return false;
        //}
		
	lastError_ = ca_create_subscription(dataType(), numElements(), chid_, DBE_VALUE | DBE_LOG | DBE_ALARM, PVValueChangedCBWrapper, this, &evid_ );
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

bool AMProcessVariable::requestValue(int num) {
	
        // Not necessary. Connection status is checked by ca_array_get_callback:
        //if(ca_state(chid_) != cs_conn) {
        //	qDebug(QString("Error requesting value: channel not connected: %1").arg(pvName()));
        //	return false;
        //}
	
	lastError_ = ca_array_get_callback(dataType(), num, chid_, PVValueChangedCBWrapper, this);
	if(lastError_ != ECA_NORMAL) {
                qDebug() << QString("AMProcessVariable: Error while trying to request value: %1: %2").arg(pvName()).arg(ca_message(lastError_));
    	emit error(lastError_);
    	return false;
	}	
	
	return true;
}

void AMProcessVariable::setValue(int value) {

	dbr_long_t setpoint = value;

	lastError_ = ca_put_callback( DBR_LONG, chid_, &setpoint, PVPutRequestCBWrapper, this );
	if(lastError_ != ECA_NORMAL) {
                qDebug() << QString("AMProcessVariable: Error while trying to put value: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
	}
}

void AMProcessVariable::setValues(dbr_long_t setpoints[], int num) {

	lastError_ = ca_array_put_callback( DBR_LONG, num, chid_, setpoints, PVPutRequestCBWrapper, this );
	if(lastError_ != ECA_NORMAL) {
                qDebug() << QString("AMProcessVariable: Error while trying to put values: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
	}
}

void AMProcessVariable::setValue(double value) {

	dbr_double_t setpoint = value;

	lastError_ = ca_put_callback( DBR_DOUBLE, chid_, &setpoint, PVPutRequestCBWrapper, this );
	if(lastError_ != ECA_NORMAL) {
                qDebug() << QString("Error while trying to put AMProcessVariable value: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
	}
}

void AMProcessVariable::setValues(dbr_double_t setpoints[], int num) {

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

	lastError_ = ca_array_put_callback( DBR_STRING, setpoints.size(), chid_, stringArray, PVPutRequestCBWrapper, this );
	if(lastError_ != ECA_NORMAL) {
                qDebug() << QString("Error while trying to put AMProcessVariable values: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
	}

}

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

///////////////////////////////////
// AMIntProcessVariable
///////////////////////////////////

AMIntProcessVariable::AMIntProcessVariable(const QString& pvName, bool autoMonitor, QObject* parent, int connectionTimeoutMs) : AMProcessVariable(pvName, autoMonitor, parent, connectionTimeoutMs) {

	// TODO: Any specific initialization?
	
	values_.insert(0, -1);	// make sure we have one entry in the array of values

	// Forward valueChanged(int) signal as valueChanged().
	connect(this, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged()));

}	

void AMIntProcessVariable::valueChangedCB(struct event_handler_args eventArgs) {
	if( (lastError_ = eventArgs.status) != ECA_NORMAL) {
		qDebug() << QString("Error in value-changed callback: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
		return;
	}
	
	if(eventArgs.dbr == 0)
		return;

	// Is this control information coming back?
	if(eventArgs.type == DBR_CTRL_DOUBLE) {
		memcpy(&ctrlValue_, eventArgs.dbr, sizeof(ctrlValue_));
		// TODO: Think about this more. If the record's DRV_HIGH and DRV_LOW limits haven't been set, we're receiving them as max(0), min(0).
		// That's a problem, because it pins our minValue() and maxValue() to 0.
		if(ctrlValue_.lower_ctrl_limit == 0 && ctrlValue_.upper_alarm_limit == 0) {
			ctrlValue_.lower_alarm_limit = ctrlValue_.lower_ctrl_limit = -DBL_MAX;
			ctrlValue_.upper_alarm_limit = ctrlValue_.upper_ctrl_limit = DBL_MAX;
		}
		emit initialized();
	}

	// Is this values coming back?
	if(eventArgs.type == dataType() ) {

		values_.clear();
		for(int i=0; i<eventArgs.count; i++) {

			values_.append( ((dbr_long_t*)eventArgs.dbr)[i] );

		}

		emit valueChanged(values_[0]);
	}
	
}


///////////////////////////////////
// AMDoubleProcessVariable
///////////////////////////////////

AMDoubleProcessVariable::AMDoubleProcessVariable(const QString& pvName, bool autoMonitor, QObject* parent, int connectionTimeoutMs) : AMProcessVariable(pvName, autoMonitor, parent, connectionTimeoutMs) {

	// TODO: Any specific initialization?
	
	values_.insert(0, -1.0);	// make sure we have one entry in the array of values

	connect(this, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged()));
}	

// evenArgs:
//void            *usr;   user argument supplied with request
//chanId          chid;   channel id
//long            type;   the type of the item returned
//long            count;  the element count of the item returned
//const void      *dbr;   a pointer to the item returned
//int             status; ECA_XXX status of the requested op from the server
//

void AMDoubleProcessVariable::valueChangedCB(struct event_handler_args eventArgs) {
	
	if( (lastError_ = eventArgs.status) != ECA_NORMAL) {
		qDebug() << QString("Error within value-changed callback: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
		return;
	}
	
	if(eventArgs.dbr == 0)
		return;

	// Is this control information coming back?
	if(eventArgs.type == DBR_CTRL_DOUBLE) {
		memcpy(&ctrlValue_, eventArgs.dbr, sizeof(ctrlValue_));
		// TODO: Think about this more. If the record's DRV_HIGH and DRV_LOW limits haven't been set, we're receiving them as max(0), min(0).
		// That's a problem, because it pins our minValue() and maxValue() to 0.
		if(ctrlValue_.lower_ctrl_limit == 0 && ctrlValue_.upper_alarm_limit == 0) {
			ctrlValue_.lower_alarm_limit = ctrlValue_.lower_ctrl_limit = -DBL_MAX;
			ctrlValue_.upper_alarm_limit = ctrlValue_.upper_ctrl_limit = DBL_MAX;
		}
		emit initialized();
	}

	// Is this values coming back?
	if(eventArgs.type == dataType() ) {
		values_.clear();
		for(int i=0; i<eventArgs.count; i++) {

			values_.append( ((dbr_double_t*)eventArgs.dbr)[i] );

		}

		emit valueChanged(values_[0]);
	}
	
}


///////////////////////////////////
// AMStringProcessVariable
///////////////////////////////////

AMStringProcessVariable::AMStringProcessVariable(const QString& pvName, bool autoMonitor, QObject* parent, int connectionTimeoutMs) : AMProcessVariable(pvName, autoMonitor, parent, connectionTimeoutMs) {

	// TODO: Any specific initialization?
	
	values_.insert(0, QString() );	// make sure we have one entry in the array of values

	connect(this, SIGNAL(valueChanged(QString)), this, SIGNAL(valueChanged()));
}	

	
	
void AMStringProcessVariable::valueChangedCB(struct event_handler_args eventArgs) {
	
	if( (lastError_ = eventArgs.status) != ECA_NORMAL) {
		qDebug() << QString("Error in value-changed callback: %1: %2").arg(pvName()).arg(ca_message(lastError_));
		emit error(lastError_);
		return;
	}
	
	if(eventArgs.dbr == 0)
		return;
	
	// Is this control information coming back?
	if(eventArgs.type == DBR_CTRL_DOUBLE) {
		memcpy(&ctrlValue_, eventArgs.dbr, sizeof(ctrlValue_));
		// TODO: Think about this more. If the record's DRV_HIGH and DRV_LOW limits haven't been set, we're receiving them as max(0), min(0).
		// That's a problem, because it pins our minValue() and maxValue() to 0.
		if(ctrlValue_.lower_ctrl_limit == 0 && ctrlValue_.upper_alarm_limit == 0) {
			ctrlValue_.lower_alarm_limit = ctrlValue_.lower_ctrl_limit = -DBL_MAX;
			ctrlValue_.upper_alarm_limit = ctrlValue_.upper_ctrl_limit = DBL_MAX;
		}
		emit initialized();
	}

	// Is this values coming back?
	if(eventArgs.type == dataType() ) {
		values_.clear();
		for(int i=0; i<eventArgs.count; i++) {

			values_.append( QString( ((dbr_string_t*)eventArgs.dbr)[i] ) );

		}

		emit valueChanged(values_[0]);
	}
	
}
