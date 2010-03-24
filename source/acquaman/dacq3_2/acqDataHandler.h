/*
 * $Header: acqDataHandler.h 1.2 2009/03/04 15:13:47CST Glen Wright (wrightg) Exp  $
 * Copyright Canadian Light Source, Inc. All rights reserved.
 *
 * Description:
 *    data structure for registering output methods
 */

#if !defined(ACQ_DATA_HANDLER_H)
#define ACQ_DATA_HANDLER_H 1

/**
 ** data stream handler that gets registered with the acquisition master, and used by the
 ** event handler.
 **/
typedef struct eventDataHandler eventDataHandler_t;
typedef void * acqKey_t;
typedef void * registrarKey_t;

struct eventDataHandler {
	void * registerReserved;								/* available to the program registering the handler */
	registrarKey_t regKey;									/* key available for reverse calls */
	/* Calls made from the acquisition software to the output handlers */
	int (* pvlist_cb)(acqKey_t key, int eventno, char **pvlist);				/* instead, see pvName_cb */
	int (* start_cb)(acqKey_t key);								/* start a run */
	int (* pause_cb)(acqKey_t key);								/* pause a run */
	int (* resume_cb)(acqKey_t key);							/* resume a paused run */
	int (* stop_cb)(acqKey_t key);								/* stop a run */
	int (* shutdown_cb)(acqKey_t key);							/* shut down a run */
	int (* startpass_cb)(acqKey_t key, int passno);						/* start a pass within a scan */
	int (* endpass_cb)(acqKey_t key);							/* end a pass within a run */
	int (* startrepeat_cb)(acqKey_t key, int repeat);					/* start a scan within a run */
	int (* endrepeat_cb)(acqKey_t key, int repeat);						/* end a scan within a run */
	int (* nextOutput_cb)(acqKey_t key);							/* request the next output file */
	int (* eventName_cb)(acqKey_t key, int eventno, const char *name);			/* give an event # a name */
	int (* eventFlags_cb)(acqKey_t key, int eventno, const char *flags);			/* give an event # flags */
	int (* pvName_cb)(acqKey_t key, int eventno, int pvno, const char *pvname);		/* set the name information for a column header */
	int (* pvDescription_cb)(acqKey_t key, int eventno, int pvno, const char *description );
	int (* pvRange_cb)(acqKey_t key, int eventno, int pvno, int count );			/* outputs 'count' elements starting at the beginning of the array */

	int (* startRecord_cb)(acqKey_t key, int eventno);					/* start a single output record (usually scan point) */
	int (* endRecord_cb)(acqKey_t key, int eventno);					/* end a single output record */
	int (* pvValue_cb)(acqKey_t key, int eventno, int pvno, const void *value, int count);	/* output data values */
	int (* pvDataType_cb)(acqKey_t key, int eventno, int pvno, int dataType, int maxCount);	/* define the data type (uses EPICS DBF_/DBR_) */
	int (* pvState_cb) ( acqKey_t key, int eventno, int pvno, int state, int valid);	/* notifies of state changes for a PV */
	int (* pvFlags_cb)(acqKey_t key, int eventno, int pvno, const char *flags);		/* sets output flags for a column */
	int (* comment_cb)(acqKey_t key, const char *comment);					/* record a comment line for possible later output */
	int (* getHandlerSignal_cb)( acqKey_t key, acqKey_t from, unsigned int signal, const void *data);/* recieve a signal from another handler */
	/* Calls made from the output handlers to the acquisition software */
	int (* statusMessage_cb)(registrarKey_t key, const char *message);			/* get a message to the user */
	int (* sendHandlerSignal_cb)( registrarKey_t key, acqKey_t from, unsigned int signal, const void *data);	/* signal the controlling app */
};

#endif
