/*
 * Copyright 2007 Canadian Light Source, Inc. All Rights Reserved.
 */

#include "acqBaseOutput.h"
#include <string.h>
#include <stdio.h>
#include <cstdlib>

#include <QString>
#include <QDebug>

static eventDataHandler_t defaultAcqBaseOutput = {
				NULL,
				NULL,
	pvlist_cb:		( int(*)(void *, int, char**)) acqBaseOutput::nullFunction,
	start_cb:		acqBaseOutput::nullFunction,
	pause_cb:		acqBaseOutput::nullFunction,
	resume_cb:		acqBaseOutput::nullFunction,
	stop_cb:		acqBaseOutput::nullFunction,
	shutdown_cb:		acqBaseOutput::nullFunction,

	startpass_cb:		(int(*)(void *,int)) acqBaseOutput::nullFunction,
	endpass_cb:		acqBaseOutput::nullFunction,
	startrepeat_cb:		(int(*)(void *,int)) acqBaseOutput::nullFunction,
	endrepeat_cb:		(int(*)(void *,int)) acqBaseOutput::nullFunction,
	nextOutput_cb:		acqBaseOutput::nullFunction,
	eventName_cb:		acqBaseOutput::default_eventName,
	eventFlags_cb:		acqBaseOutput::default_eventFlags,
	pvName_cb:		acqBaseOutput::default_pvName,
	pvDescription_cb:	acqBaseOutput::default_pvDescription,
	pvRange_cb:		acqBaseOutput::default_pvRange,
	startRecord_cb:		(int(*)(void *,int)) acqBaseOutput::nullFunction,
	endRecord_cb:		(int(*)(void *,int)) acqBaseOutput::nullFunction,
	pvValue_cb:		acqBaseOutput::default_pvValue,
	pvDataType_cb:		acqBaseOutput::default_pvDataType,
	pvState_cb:		acqBaseOutput::default_pvState,
	pvFlags_cb:		acqBaseOutput::default_pvFlags,
	comment_cb:		(int(*)(void *, const char*)) acqBaseOutput::nullFunction,
	getHandlerSignal_cb:	(int(*)(void *, void *, unsigned int, const void *)) acqBaseOutput::nullFunction,

	statusMessage_cb:	acqBaseOutput::default_statusMessage,
	sendHandlerSignal_cb:	(int(*)(void *, void *, unsigned int, const void*)) acqBaseOutput::nullFunction
};

// no-op: used when a function is not applicable
int acqBaseOutput::nullFunction(void *unused)
{
	return 0;	// success indication: we successfully did nothing
}

acqBaseOutput::acqBaseOutput()
	: key(NULL), debug(0), bsp(NULL), handler(defaultAcqBaseOutput)
{
	defProperty("Debug");
}

acqBaseOutput::~acqBaseOutput()
{
}

//
// returns the resulting string length on success, -1 on failure
int
acqBaseOutput::value_to_string(const void *value, int dbType, const char * format, char *result, int result_max )
{
	switch( dbType)
	{
	case DBF_STRING:
		strncpy( result, (char *)value, result_max);
		break;
	case DBF_ENUM:	// ENUM support is difficult if the value may be passed through multiple
			// processes before being handled. It is the responsibility of the inheriting class
			// to determine how to convert enums to strings, probably with support from the
			// calling program.
	case DBF_SHORT:
		snprintf(result, result_max, format?format:"%d", *(short *)value);
		break;
	case DBF_FLOAT:
		snprintf(result, result_max, format?format:"%g", *(float *)value);
		break;
	case DBF_CHAR:
		snprintf(result, result_max, format?format:"%d", *(char *)value);
		break;
	case DBF_LONG:{
		//snprintf(result, result_max, format?format:"%ld", *(long *)value);
		snprintf(result, result_max, format?format:"%d", *(int *)value);
		break;
	}
	case DBF_DOUBLE:
		snprintf(result, result_max, format?format:"%g", *(double *)value);
		break;
	default:
		handlerSignal(acqBaseOutput_BadType);
		return -1;
	}

	return strlen(result);
}

// returns success or failure, result is in 'result'
int
acqBaseOutput::value_to_double(const void *value, int dbType, double &result)
{
	switch ( dbType)
	{
	case DBF_STRING:
		result = atof( (char *)value);
		break;
	case DBF_ENUM:
	case DBF_SHORT:
		result = *(short *)value;
		break;
	case DBF_FLOAT:
		result = *(float *)value;
		break;
	case DBF_CHAR:
		result = *(char *)value;
		break;
	case DBF_LONG:
		result = *(long *)value;
		break;
	case DBF_DOUBLE:
		result = *(double *)value;
		break;
	default:
		handlerSignal(acqBaseOutput_BadType);
		return -1;
	}
	return 0;
}

int
acqBaseOutput::value_to_int( const void *value, int dataType, int &result)
{
	switch ( dataType)
	{
	case DBF_STRING:
		result = atoi( (char *)value);
		break;
	case DBF_ENUM:
	case DBF_SHORT:
		result = *(short *)value;
		break;
	case DBF_FLOAT:
		{
		float r;
		r = *(float *)value;
		if( r > 0)
			result = (int)(r+0.5);
		else
			result = (int)(r-0.5);
		}
		break;
	case DBF_CHAR:
		result = *(char *)value;
		break;
	case DBF_LONG:
		result = *(long *)value;
		break;
	case DBF_DOUBLE:
		{
		double r;
		r = *(double *)value;
		if( r > 0)
			result = (int)(r+0.5);
		else
			result = (int)(r-0.5);
		}
		break;
	default:
		handlerSignal(acqBaseOutput_BadType);
		return -1;
	}
	return 0;
}

eventDataHandler_t *
acqBaseOutput_getHandler(acqKey_t key)
{
	acqBaseOutput *to = (acqBaseOutput *) key;
	return &to->handler;
}

acqOutputEvent_t *acqBaseOutput::find_event_name(const char *name)
{
	acqOutputEvent_t *aoep;

	for(aoep=key; aoep; aoep=(aoep->next==key?NULL:aoep->next) )
		if( strcmp(aoep->eventName, name) == 0)
			return aoep;
	if( debug) printf("find_event_name(%s) fails\n", name);
	handlerSignal(acqBaseOutput_BadEvent);
	return NULL;
}
acqOutputEvent_t *acqBaseOutput::find_event_number( int id)
{
	acqOutputEvent_t *aoep;

	for(aoep=key; aoep; aoep=(aoep->next==key?NULL:aoep->next) )
	{
		if( debug) printf("find_event_number/compare (%u,%u)\n", aoep->eventNo, id);
		if( aoep->eventNo == id)
			return aoep;
	}
	if( debug) printf("find_event_number(%d) fails\n", id);
	handlerSignal(acqBaseOutput_BadEvent);
	return NULL;
}
int acqBaseOutput::pvIndex(const char * eventName, const char *pvName)
{
	acqOutputEvent_t *aoep;
	aoep = find_event_name(eventName);
	if( aoep == NULL)
		return -1;
	return pvIndex( aoep, pvName);
}

int acqBaseOutput::pvIndex(acqOutputEvent_t *event, const char * pvName)
{
	if( event == NULL)
		return -1;

	acqOutputColumn_t *colp;

	for(int colnum=0; colnum < event->nColumn; colnum++)
	{
		colp = &event->column[colnum];
		if( colp->realName && strcmp(colp->realName,pvName) == 0)
			return colnum;
		if( colp->columnName && strcmp(colp->columnName, pvName) == 0)
			return colnum;
	}
	return -1;
}


acqOutputEvent_t *acqBaseOutput::new_event()
{
	acqOutputEvent_t *aoep;
	aoep = (acqOutputEvent_t *) calloc(1, sizeof (acqOutputEvent_t));
	if( key == NULL)
	{
		key = aoep;
		aoep->next = aoep->prev = aoep;
		return aoep;
	}
	aoep->next = key;
	aoep->prev = key->prev;
	aoep->next->prev = aoep;
	aoep->prev->next = aoep;
	return aoep;
}
void acqBaseOutput::destroy_event(  acqOutputEvent_t *event)
{
	if( event == NULL)
		return;
	if( event->next == event)
		key = NULL;
	else
	{
		event->prev->next = event->next;
		event->next->prev = event->prev;
		if( key == event)
			key = event->next;
	}
	free(event);
}
/* creates a new output event */
int acqBaseOutput::default_eventName(acqKey_t key, int eventno, const char *name)
{
	acqOutputEvent_t *eventp;
	acqBaseOutput *basep = (acqBaseOutput *)key;
	if( basep->debug) printf("default_eventName(%p, %d, %s)\n", key, eventno, name);
	if( (eventp = basep->find_event_number( eventno)) == NULL)
		eventp = basep->new_event();
	eventp->eventNo = eventno;
	if( eventp->eventName)
		free(eventp->eventName);
	eventp->eventName = strdup(name);
	return 0;
}
/* adds the flags to the numbered event */
int acqBaseOutput::default_eventFlags(acqKey_t key, int eventno, const char *flags)
{
	acqOutputEvent_t *eventp;
	acqBaseOutput *basep = (acqBaseOutput *)key;
	if( basep->debug) printf("default_eventFlags(%p, %d, %s)\n", key, eventno, flags);
	if( (eventp = basep->find_event_number(eventno)) == NULL)
		return -1;
	if( eventp->outputFlags )
		free( eventp->outputFlags);
	eventp->outputFlags = strdup (flags);
	return 0;
}
/* creates a new output column */
int acqBaseOutput::default_pvName( acqKey_t key, int eventno, int pvno, const char *name)
{
	acqBaseOutput *basep = (acqBaseOutput *)key;
	acqOutputEvent_t *eventp;
	acqOutputColumn_t *col;
	if( basep->debug) printf("default_pvName(key:%p eventno:%d pvno:%d name:%s)\n", key, eventno, pvno, name);
	if( pvno < 0 || (eventp = basep->find_event_number(eventno)) == NULL)
		return -1;
	if( basep->debug) printf("default_pvName eventp=%p\n", eventp);
	if( eventp->nColumn <= pvno)
	{
		int i;
		eventp->column = (acqOutputColumn_t *)realloc( eventp->column, (pvno+1)*sizeof (acqOutputColumn_t));
		for( i=eventp->nColumn; i <= pvno; i++)
		{
			col = &(eventp->column[i]);
			col->columnID = -1;
			col->description = NULL;
			col->columnName = NULL;
			col->realName = NULL;
			col->columnFlags = NULL;
			col->dataSize = 0;
			col->columnType = 0;
			col->columnData = NULL;
			col->private_data = NULL;
			col->valid = 0;
			col->severity = 0;
		}
		eventp->nColumn = pvno+1;
	}
	col = &(eventp->column[pvno]);
	if( col->columnName)
		free(col->columnName);
	col->columnName = strdup( name);
	col->columnID = pvno;
	col->dataCount = 1;
	return 0;
}
/* set the descriptive name to use for a column (or range of columns?) */
int acqBaseOutput::default_pvDescription( acqKey_t key, int eventno, int pvno, const char *description)
{
	acqBaseOutput *basep = (acqBaseOutput *)key;
	acqOutputEvent_t *eventp;
	acqOutputColumn_t *col;
	if( basep->debug) printf("default_pvDescription(key:%p eventno:%d pvno:%d desc:%s)\n", key, eventno, pvno, description);
	if( pvno < 0 || (eventp = basep->find_event_number(eventno)) == NULL)
		return -1;
	col = &(eventp->column[pvno]);
	if( col->description )
		free(col->description);
	col->description = description?strdup (description):NULL;
	return 0;
}
int acqBaseOutput::default_pvRange(acqKey_t key, int eventno, int pvno, int count)
{
	acqBaseOutput *basep = (acqBaseOutput *)key;
	acqOutputEvent_t *eventp;
	acqOutputColumn_t *col;
	if( basep->debug) printf("default_pvRange(key:%p eventno:%d pvno:%d count:%d)\n", key, eventno, pvno, count);
	if( pvno < 0 || (eventp = basep->find_event_number(eventno)) == NULL)
		return -1;
	col = &(eventp->column[pvno]);
	col->dataCount = count;
	return 0;
}
/* gets a new value for an output column */
int acqBaseOutput::default_pvValue(acqKey_t key, int eventno, int pvno, const void *value, int count)
{
	acqBaseOutput *basep = (acqBaseOutput *)key;
	acqOutputEvent_t *eventp;
	acqOutputColumn_t *col;
	if( basep->debug) printf("default_pvValue(key:%p eventno:%d pvno:%d value:%p count:%d)\n", key, eventno, pvno, value, count);
	if( pvno < 0 || (eventp = basep->find_event_number(eventno)) == NULL)
		return -1;
	if( eventp->nColumn <= pvno)
		return -1;
	col = &(eventp->column[pvno]);
#if 1
	/* don't copy the data: leave a pointer to it, and the caller must decide
	 * if the data needs to be saved between calls
	 */
	col->columnData = value;
#else
	if( count > col->dataCount)
		count = col->dataCount==0?1:col->dataCount;
	count *= col->dataSize;
	memcpy( col->columnData, value, count);
#endif
	return 0;
}

/* sets the PV connection state */
int acqBaseOutput::default_pvState(acqKey_t key, int eventno, int pvno, int severity, int valid)
{
	acqBaseOutput *basep = (acqBaseOutput *)key;
	acqOutputEvent_t *eventp;
	if( basep->debug) printf("default_pvState(key:%p eventno:%d pvno:%d severity:%d valid:%d)\n", key, eventno, pvno, severity, valid);
	eventp = basep->find_event_number(eventno);
	if( eventp == NULL)
		return -1;
	if (pvno >= eventp->nColumn)
		return -1;
	eventp->column[pvno].valid = valid;
	eventp->column[pvno].severity = severity;
	return 0;
}

/* sets the data type of the output column */
int acqBaseOutput::default_pvDataType(acqKey_t key, int eventno, int pvno, int dataType, int maxCount)
{
	acqBaseOutput *basep = (acqBaseOutput *)key;
	acqOutputEvent_t *eventp;
	if( basep->debug) printf("default_pvDataType(%p, %d, %d, %d, %d)\n", key, eventno, pvno, dataType, maxCount);
	eventp = basep->find_event_number(eventno);
	if( eventp == NULL)
		return -1;
	if (pvno >= eventp->nColumn)
		return -1;
	eventp->column[pvno].columnType = dataType;
	eventp->column[pvno].dataCount = maxCount;
	switch( dataType)
	{
	case DBF_STRING:
		eventp->column[pvno].dataSize = 40;
		break;
	case DBF_CHAR:
		eventp->column[pvno].dataSize = sizeof (char);
		break;
	case DBF_SHORT:
		eventp->column[pvno].dataSize = sizeof (short);
		break;
	case DBF_LONG:
		eventp->column[pvno].dataSize = sizeof (long);
		break;
	case DBF_FLOAT:
		eventp->column[pvno].dataSize = sizeof (float);
		break;
	case DBF_DOUBLE:
		eventp->column[pvno].dataSize = sizeof (double);
		break;
	case DBF_ENUM:
		eventp->column[pvno].dataSize = sizeof (short);
		break;
	default:
		eventp->column[pvno].dataSize = 0;
		break;
	}

	return 0;
}
/* sets the flags of the output column */
int acqBaseOutput::default_pvFlags(acqKey_t key, int eventno, int pvno, const char *flags)
{
	acqBaseOutput *basep = (acqBaseOutput *)key;
	acqOutputEvent_t *eventp;
	acqOutputColumn_t *col;
	if( basep->debug) printf("default_pvFlags(%p, %d, %d, %s)\n", key, eventno, pvno, flags);
	eventp = basep->find_event_number(eventno);
	if( eventp == NULL)
		return -1;
	if (pvno >= eventp->nColumn)
		return -1;
	col = &(eventp->column[pvno]);
	if( col->columnFlags)
		free(col->columnFlags);
	col->columnFlags = strdup(flags);
	return 0;
}

//
// break a string up into a vector
//
std::vector<std::string>
acqBaseOutput::takeApart(const std::string s, const char *whitespace)
{
	std::vector<std::string> vs;
	int first=0, last=0;

	if( s.empty())
		return vs;

	while( 1) {
		first = s.find_first_not_of( whitespace, last);
		if( first < 0)
			return vs;
		if( s[first] != '"')
		{
			last = s.find_first_of( whitespace, first+1);
			if( last > 0)
				vs.insert( vs.end(), s.substr(first, last-first));
			else
			{
				vs.insert( vs.end(), s.substr( first, s.length()-first) );
				return vs;
			}
		}
		else
		{
			last = s.find_first_of( "\"", first+1);
			if( last < 0)
			{
				// no closing quote mark. take the rest of the line as the string
				vs.insert( vs.end(), s.substr( first, s.length()-first) );
				return vs;
			}
			vs.insert( vs.end(), s.substr(first+1, last-first-1));
			last++;		// point past the closing quote mark
		}
	}
	/* NOT REACHED */
	return vs;
}

//
// currently, this creates a new property if it isn't already defined.
//
void acqBaseOutput::setPropertyValue( const std::string aname, const std::string avalue)
{

	if( aname == "Debug")
		setDebug( atoi(avalue.c_str() ) );

	if( bsp  && bsp->validProperty(aname) )
	{
		bsp->setProperty( aname, avalue);
		handlerSignal( acqBaseOutput_NewProperty, aname.c_str() );
		return;
	}
	property::properties::iterator idx = propertyList.find(aname);
	if( idx == propertyList.end() )
		defProperty(aname);
	propertyList[aname]->setValue(avalue);
	handlerSignal(acqBaseOutput_NewProperty, aname.c_str() );
}

void acqBaseOutput::setProperty(const std::string name, const std::string value)
{
	setPropertyValue(name, value);
}

std::string &acqBaseOutput::getProperty(const std::string aname)
{
	if( bsp && bsp->validProperty(aname) )
		return bsp->getProperty(aname);

	return propertyList[aname]->getValue();
}

///
// assign properties from a string of assignments
///
void acqBaseOutput::setPropertyList(const std::string list)
{
	std::vector<std::string> setList;
	std::string newname;

	setList = takeApart(list, ",");
	for(unsigned int propno=0; propno < setList.size(); propno++)
	{
		unsigned int eqpos;
		eqpos = setList[propno].find('=');
		if( eqpos == std::string::npos)
		{
			if( !validProperty(setList[propno]) )
				defProperty( setList[propno]) ;
			setProperty( setList[propno], "");
			continue;
		}
		newname = setList[propno].substr(0,eqpos);
		if( !validProperty(newname) )
			defProperty( newname );
		setProperty( newname, setList[propno].substr(eqpos+1) );
	}
}

//
// returns the known valid properties
//
const std::string acqBaseOutput::getPropertyList()
{
	std::string names;
	if( bsp)
		names = bsp->getPropertyList();
	for(property::iterator idx = propertyList.begin(); idx!= propertyList.end(); ++idx)
		names += idx->first + ",";
	return names;
}

bool acqBaseOutput::validProperty(const std::string aname)
{
	if( propertyList.find(aname) != propertyList.end() )
		return true;

	if( bsp)
		return( bsp->validProperty(aname) );
	return false;
}

void acqBaseOutput::defProperty(std::string propname, std::string possibleEnums)
{
	   if( !validProperty( propname))
				propertyList[propname] = new property(possibleEnums);
}

int
acqBaseOutput::default_statusMessage(acqKey_t key, const char *message)
{
	printf("key:%p message:%s\n", key, message);
	return 0;
}

//
// support displaying messages back to the user
//
void acqBaseOutput::statusMessage(const char * message)
{
	handler.statusMessage_cb( handler.regKey, message);
}

void acqBaseOutput::statusMessage( const std::string message)
{
	handler.statusMessage_cb( handler.regKey, message.c_str() );
}

void acqBaseOutput::handlerSignal(unsigned int signalID, const void *data)
{
	handler.sendHandlerSignal_cb( handler.regKey, (acqKey_t) this, signalID, data);
}
