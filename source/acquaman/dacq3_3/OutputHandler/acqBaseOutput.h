/*
 * Copyright 2007 Canadian Light Source, Inc.
 * All rights reserved
 */

// Changed to ifndef and appended DACQLIB_ prefix (David Chevrier, Oct 27 2011)
//#if !defined(ACQBASEOUTPUT_H)
#ifndef DACQLIB_ACQBASEOUTPUT_H
#define DACQLIB_ACQBASEOUTPUT_H 1

// Changed the locash and now this is up one directory (David Chevrier, Oct 27 2011)
//#include "acqDataHandler.h"
#include "../acqDataHandler.h"
#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif
typedef struct acqOutputEvent acqOutputEvent_t;
typedef struct acqOutputColumn acqOutputColumn_t;
typedef void * acqKey_t;

// TO DO: change to class definitions, add methods
// These two structures are not currently being referenced from 'C' code, and
// should be set up as classes to encourage a cleaner C++ interface.

///
// describe the settings for an output event. These values are passed from the
// scan handler. An output event is made up of one or move columns.
///
struct acqOutputEvent {
	acqOutputEvent_t *prev, *next;
	int eventNo;			/* event number, as passed in the callback */
	char * eventName;		/* event name, as passed in the callback */
	char * outputFlags;		/* raw flags as passed in callback */
	int nColumn;			/* maximum output column number */
	acqOutputColumn_t *column;	/* outputs */
	void * private_data;		/* available for use by a single registered output handler */
};

/**
 * describe a column or a related set of columns for a single output event.
 * Related sets are columns all generated from the same PV.
 **/
struct acqOutputColumn {
	char * columnName;		/* column name as passed by the callback */
	char *realName;			/* true name of the process variable being read */
	int columnID;			/* column identifier >= 0 valid, -1 indicates invalid entry */
	int columnType;			/* column type, as passed in the callback */
	char *columnFlags;		/* raw flags as passed in callback */
	char *description;		/* description field for this column */
	unsigned int dataSize;		/* number of bytes per data read */
	unsigned int dataCount;		/* max number of elements that can be read */
	void const *columnData;		/* storage space for data read */
	bool valid;			/* simple true/false for connection status */
	int severity;			/* the 'severity' error state of the PV */
	void *private_data;		/* available for use by a single registered output handler */
};

eventDataHandler_t * acqBaseOutput_GetHandler(acqKey_t key);
void acqBaseOutput_savePropertyList(acqKey_t , FILE *);

/*
 * support for output handler signals
 */

#define acqOHsignal(BROADCAST, MODULE,NUMBER) (((MODULE)<<16)|((BROADCAST)<<8)|(NUMBER))
#define acqGetSignal(SIGNAL) ((SIGNAL)&0xFF)
#define acqGetSignalBroadcast(SIGNAL) (((SIGNAL)>>8) & 0xFF)
#define acqGetSignalModule(SIGNAL) (((SIGNAL)>>16) & 0xFF)

#define acqBaseOutputID 0
#define acqBaseOutput_BadType acqOHsignal( 0, acqBaseOutputID, 1)
#define acqBaseOutput_BadEvent acqOHsignal( 0, acqBaseOutputID, 2)
#define acqBaseOutput_NewProperty acqOHsignal( 0, acqBaseOutputID, 10)

#if defined(__cplusplus)
}	/* end of extern "C" */

#include "acqBaseStream.h"

#include <iostream>
#include <vector>
#include <string>
#include "acqProperties.h"

/**
 * Base output class. This should be inherited by all output handler classes.
 **/
class acqBaseOutput {
private:
	acqOutputEvent_t *key;			// passed to all actions, must be first member.
	std::string identifier;			// available to calling applications to identify the handlers.
	property::properties propertyList;
	bool debug;
	acqBaseStream *bsp;			// if non-null, the I/O stream associated with this handler.
	/*
	 * event management functions
	 */
public:
	// duplicate data types as used by EPICS
#undef DBF_STRING
#undef DBF_SHORT
#undef DBF_FLOAT
#undef DBF_ENUM
#undef DBF_CHAR
#undef DBF_LONG
#undef DBF_DOUBLE
#undef DBF_NO_ACCESS
	enum acqDataTypes 
		{
			DBF_STRING=0,
			DBF_SHORT=1,
			DBF_FLOAT=2,
			DBF_ENUM=3,
			DBF_CHAR=4,
			DBF_LONG=5,
			DBF_DOUBLE=6,
			DBF_NO_ACCESS=7
		} ;
			
	void * private_data;			// available to the output handler
	eventDataHandler_t handler;

	void setIdentifier(const std::string newName) { identifier = newName; };
	std::string getIdentifier() { return identifier; };
	// methods for modifying the output event list
	acqOutputEvent_t *find_event_name(const char *name);
	acqOutputEvent_t *find_event_number(int id);
	acqOutputEvent_t *new_event();
	void destroy_event(acqOutputEvent_t *event);

	void setBaseStream( acqBaseStream *abs) { if(bsp) delete bsp; bsp = abs; };
	acqBaseStream * getBaseStream() { return bsp; };

	// methods used by the default data handler value
	static int nullFunction(void *);

		/* creates a new output event */
	static int default_eventName(acqKey_t key, int eventno, const char *name);

		/* adds the flags to the numbered event */
	static int default_eventFlags(acqKey_t key, int eventno, const char *flags);

		/* creates a new output column */
	static int default_pvName(acqKey_t key, int eventno, int pvno, const char *name);

		/* sets the pv description field*/
	static int default_pvDescription(acqKey_t key, int eventno, int pvno, const char *description);

		/* sets the expected # of columns for this output */
	static int default_pvRange( acqKey_t key, int eventno, int pvno, int count);

		/* gets a new value for an output column */
	static int default_pvValue(acqKey_t key, int eventno, int pvno, const void *value, int count);

		/* sets the status and connection valid information for a column */
	static int default_pvState(acqKey_t key, int eventno, int pvno, int severity, int valid);

		/* sets the data type and number of elements of the output column */
	static int default_pvDataType(acqKey_t key, int eventno, int pvno, int dataType, int maxCount);

		/* sets the flags of the output column */
	static int default_pvFlags(acqKey_t key, int eventno, int pvno, const char *flags);

		/* requests that a status message be displayed to the user */
	static int default_statusMessage(acqKey_t key, const char *message);

	acqBaseOutput();
	virtual ~acqBaseOutput();
	// access to data values
	int value_to_string(const void *value, int dataType, const char *format, char *result, int result_len);
	int value_to_double(const void *value, int dataType, double &result);
	int value_to_int   (const void *value, int dataType, int &result);
	// determine output column for a particular PV
	int pvIndex(const char * eventName, const char *pvName);
	int pvIndex(acqOutputEvent_t *event, const char * pvName);

	// non-iterator for accessing the event list
	acqOutputEvent_t *first() {
		acqOutputEvent_t **head = (acqOutputEvent_t **)key;
		if( !head) return NULL;
		return *head;
	};
		
	acqOutputEvent_t *next(acqOutputEvent_t *ptr) {
		if( ptr == NULL)
			return first();
		acqOutputEvent_t **head = (acqOutputEvent_t **)key;
		if( ptr->next == *head)
			return NULL;
		return ptr->next;
	};

	// general routine, much like 'split' in awk. Useful with the property lists.
	static std::vector<std::string> takeApart(const std::string list, const char* whitespace);

	// Methods for accessing the property list.
	// these 4 methods manipulate the list directly, and should be called by derived classes

	virtual const std::string getPropertyList();
	
	void setPropertyValue(std::string prop, std::string value);
	virtual std::string &getProperty(const std::string name);

	// this should call the above setPropertyValue from an immediately derived class.
	virtual void setProperty(const std::string name, const std::string value);

	// calls setProperty with each entry, and should not need to be over-ridden by a derived class.
	// but, it is virtual just in case.
	virtual void setPropertyList(const std::string list);

	virtual bool validProperty(const std::string name);
	void defProperty( std::string propname, std::string possibleEnums = "");
	
			
	//
	// manage messages
	//
	virtual void statusMessage(const std::string message);
	virtual void statusMessage(const char * const message);
	//
	// manage signals
	//
	virtual void handlerSignal(unsigned int signalID, const void *data=NULL);

	// manage debugging output
	void setDebug(bool flag) { debug = flag; } ;
	bool getDebug() { return debug; } ;

	virtual std::string default_stream() = 0;
	// should these be "protected"? It should be up to the class to control all I/O, and any application
	// creating an output stream would not have the right to interfere.
	int acq_write(const void *data, unsigned int size) { if (bsp) return bsp->write(data, size); return -1; }
	int acq_open() { if( bsp) return bsp->open(); return -1; }
	int acq_close() { if( bsp) return bsp->close(); return 0; }
	int acq_next() { if( bsp) return bsp->next(); return 0; }
	int acq_flush() { if( bsp) return bsp->flush(); return 0; }
	bool isReady() { if( bsp) return bsp->isReady(); return 0; }

};


// Generic conversion template. Doesn't belong here, doesn't belong anywhere else in the project
#include <sstream>
template <class T>
inline std::string to_string (const T& t, int wd=3, char fl='0')
{
	std::stringstream ss;
	ss.width(wd);
	ss.fill(fl);
	ss << t;
	return ss.str();
}
#endif

#endif
