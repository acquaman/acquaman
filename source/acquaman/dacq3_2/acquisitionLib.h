/*
 * $Header: acquisitionLib.h 1.3.1.4 2009/03/04 15:13:46CST Glen Wright (wrightg) Exp  $
 * Copyright Canadian Light Source, Inc. All rights reserved.
 *
 * Description:
 *    data structures for acquisition control.
 */

/**/
//#if !defined(ACQUISITIONLIB_H)
#ifndef DACQLIB_ACQUISITIONLIB_H
#define DACQLIB_ACQUISITIONLIB_H 1
/**/
/*
#ifndef ACQUISITIONLIB_H
#define ACQUISITIONLIB_H
*/

#include "acqDataHandler.h"

typedef struct acqMaster acqMaster_t;

/* Version numbering: note that each of major, minor, and release range from 0 to 99
 */
#define ACQ_VERSION_MAJOR 3
#define ACQ_VERSION_MINOR 0
#define ACQ_VERSION_RELEASE 0

#define ACQ_VERSION(MA,MI,RE) ((MA*10000)+(MI*100)+(RE))
#define ACQ_VERSION_CURRENT ACQ_VERSION(ACQ_VERSION_MAJOR,ACQ_VERSION_MINOR,ACQ_VERSION_RELEASE)

#define ACQ_VERSION_LT(MA,MI,RE) ( ACQ_VERSION_CURRENT < ACQ_VERSION(MA,MI,RE) )
#define ACQ_VERSION_GE(MA,MI,RE) ( ACQ_VERSION_CURRENT >= ACQ_VERSION(MA,MI,RE) )

#include "epicsConnect.h"
#include <epicsEvent.h>
#include <epicsMutex.h>
#include <stdio.h>
#include <stdlib.h>
// MB removed to build on mac os x: #include <malloc.h>
#include <string.h>

//?#include "acqDataHandler.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct acqEvent acqEvent_t;
typedef struct acqScan acqScan_t;
typedef struct acqMotor acqMotor_t;
typedef struct acqOneMotor acqOneMotor_t;
typedef struct acqAction acqAction_t;
typedef struct acqTrigger acqTrigger_t;
typedef struct acqDelay acqDelay_t;
typedef struct acqControl acqControl_t;
typedef struct acqPv acqPv_t;


/*
 * General Action description for triggers. Replaces convoluted trigger/delay code
 */
typedef enum acqActionType {
	AA_DELAY_TIME,		/* pauses the scan for the indicated time				*/
	AA_SET_PV,		/* sets a PV to a string value (EPICS may convert)			*/
	AA_WAIT_PV,		/* compares a PV to a value (tries to do intelligent numeric comparisons) */
	AA_WAIT_MOTOR,		/* waits for a motor group (don't use this!)				*/
	AA_CALL_EVENT,		/* call an output event - waitflag TRUE means wait for event completion */
	AA_CALL_SCAN,		/* call another scan sequence - waitflag TRUE means wait for completion */
	AA_WAIT_EVENT,		/* wait for an event to complete */
	AA_WAIT_SCAN,		/* wait for a scan to complete */
	AA_SET_CONTROL,		/* set the control PV at this point, instead of at the beginning of 'move' */
	AA_NEXT_OUTPUT,		/* increment the next output event */
	AA_NO_ACTION		/* empty action */
	} acqActionType_t;

/*
 * General "No Connection" response for events and scans.
 */
typedef enum acqNoConnectResponse {
	NCR_IGNORE,		/* (DEFAULT) ignore the connection failure and continue */
	NCR_RECONNECT,		/* silently wait for reconnect */
	NCR_PAUSE,		/* (NOT IMPLEMENTED) pause the acquisition while waiting */
	NCR_PAUSE_TIME_60,	/* (NOT IMPLEMENTED) if no reconnect in 60 seconds, fail */
	NCR_PAUSE_TIME_600,	/* (NOT IMPLEMENTED) if no reconnect in 600 seconds, fail */
	NCR_PAUSE_TIME_3600,	/* (NOT IMPLEMENTED) if no reconnect in 3600 seconds, fail */
	NCR_FAIL		/* fail immediately a missing required connection is noticed */
} acqNoConnectResponse_t;

/*
 * structures for acqAction union
 */
struct acqAction_delayTime
{
	double delay;
};

struct acqAction_setPV
{
	Connector *connector;		/* connector for this PV */
	char *name;			/* name of the PV */
	char *value;			/* value to set the PV */
	acqNoConnectResponse_t noConnect;
};

struct acqAction_waitPV
{
	Connector *connector;		/* connector for this PV */
	char *name;			/* name of the PV */
	char *value;			/* value to watch for in the PV */
	epicsEventId endDelay;		/* signal to send to notify delay completion */
	unsigned int waitFlag:1,	/* 1: when waiting for a signal */
		testNumeric:1;		/* 1: if 'value' appears numeric, perform a numeric comparison */
	acqNoConnectResponse_t noConnect;
};

struct acqAction_waitMotor
{
	char *name;			/* name of the motor or motor group to monitor */
	acqMotor_t *motor;		/* pointer to the motor structure */
};

struct acqAction_callEvent
{
	char *name;
	acqEvent_t *event;		/* event to be called */
	unsigned int autoWait:1;	/* 1 to automatically wait for event completion */
};

struct acqAction_callScan
{
	char *name;
	acqScan_t *scan;		/* scan to be called */
	unsigned int autoWait:1;	/* 1 to automatically wait for scan completion */
};

struct acqAction_waitEvent
{
	char *name;
	acqEvent_t *event;
};

struct acqAction_waitScan
{
	char *name;
	acqScan_t *scan;
};

struct acqAction_nextOutput
{
};

struct acqAction_setControl
{
};

/*
 * controlling structure for actions.
 */
struct acqAction
{
	acqMaster_t *master;
	struct acqAction *next, *prev;	/* list of actions to be executed on a trigger */
	acqActionType_t type;		/* the type of action this is */
	union
	{
	struct acqAction_delayTime	dt;
	struct acqAction_setPV		spv;
	struct acqAction_waitPV		wpv;
	struct acqAction_waitMotor	wm;
	struct acqAction_callEvent	ce;
	struct acqAction_callScan	cs;
	struct acqAction_waitEvent	we;
	struct acqAction_waitScan	ws;
	struct acqAction_nextOutput	no;
	struct acqAction_setControl	sc;
	} au;
} ;

/**
 **
 **/

/*
 * general states for scans and events. This is taken from the CLS software standards document.
 */
typedef enum acqState { AS_OFF, AS_STARTUP, AS_STANDBY, AS_RUNUP, AS_RUN, AS_RUNDOWN, AS_SHUTDOWN, AS_FAULT, AS_NUMBER } acqState;


/**
 **
 **/

/* TG == Trigger Generator */
/* index into triggers[]: Note that PV-generated triggers are not part of the scan record */
/* BEGIN - before any other actions are taken */
/* MOVE - after the movement value has been written */
/* DWELL - after the movement has been completed */
/* DONE - At the end of the last scan */
/* PAUSE - at the beginning of a 'pause' event */
/* PAUSE_END - when a run is resumed */
/* BEGIN_PASS - at the beginning of each loop iteration */
/* END_PASS - at the end of each loop iteration */

#define ACQ_TG_BEGIN	0
#define ACQ_TG_MOVE 	1
#define ACQ_TG_DWELL 	2
#define ACQ_TG_DONE 	3
#define ACQ_TG_PAUSE	4
#define ACQ_TG_PAUSE_END	5
#define ACQ_TG_BEGIN_PASS	6
#define ACQ_TG_END_PASS	7
#define NUM_SCAN_TRIGGERS 8

/*
 * list of controlling PV's for a scan.
 */
struct acqControl {
	char * controlPV;	/* PV set through this scan */
	Connector *controlChan;	/* channel connection for above */
	double startVal,	/* setting range for the controlling PV */
		deltaVal,
		finalVal;
	char	* startMacro,	/* if non-zero, decode to get the matching value */
		* deltaMacro,
		* finalMacro;
	unsigned int		/* indicate if a non-null string entered */
		haveStartVal:1,	/* if 0, no matching value entered */
		haveDeltaVal:1,
		haveFinalVal:1,
		useInitial:1,	/* when starting a scan, use the input initial value */
		useFinal:1;	/* when ending a scan, always put out the final value */
	double recordStart;	/* save of value used at start of scan */
} ;
	

struct acqScan {
	acqMaster_t *master;
	struct acqScan *next, *prev;
	char * scanName;	/* user name for this scan */
	int scanNum;		/* Id for this scan, used if no user name assigned */
	int useStart;		/* non-zero if this scan is triggered by the START button */
	int numControlPV;	/* number of controlling PV elements */
	acqControl_t *acqControlList;	/* list of controlling PV elements */

	acqAction_t *actions[NUM_SCAN_TRIGGERS];	/* alternative to the delay/trigger fixed configuration */
	acqNoConnectResponse_t noConnect;		/* how to respond to a connection failure */
	/*
	 * used only during processing
	 */
	epicsEventId startEvent, endEvent;	/* communication of event state change */
	epicsEventId putNotify;			/* communication of completion of an epicsPut */
	epicsEventId connectNotify;		/* communication of change of connection status */
	epicsThreadId threadID;
	acqState state;
	int needSetControl;	/* non-zero if the control point is set explicity in the MOVE trigger list */
	int shutdown;		/* non-zero to end this thread */
	int count;		/* keeps track of the number of loops */
	int currentControl;	/* current control element */
} ;

acqScan_t * new_acqScan(const char * scanName , acqMaster_t *master);
acqScan_t * first_acqScan( const acqMaster_t *master );
acqScan_t * next_acqScan(const acqScan_t *);
acqScan_t * lookup_acqScan( const char * , const acqMaster_t *master);
acqScan_t * convert_acqScan( acqScan_t *);	/* change from trigger/delay model to action model */
void destroy_acqScan( acqScan_t * , acqMaster_t *master);

/*
 * support for actions within scans
 */
acqAction_t * new_acqAction( acqAction_t **head, acqActionType_t type, acqAction_t *insertion, acqMaster_t *master);
acqAction_t * destroy_acqAction( acqAction_t **head, acqAction_t *acq);
acqActionType_t getActionType( const char *name);
char * getActionName( acqActionType_t type);

acqAction_t * new_acqAction_delayTime( acqAction_t **head, double delay, acqMaster_t *master);
acqAction_t * new_acqAction_setPV( acqAction_t **head, char *pvname, char *value, acqMaster_t *master);
acqAction_t * new_acqAction_waitPV( acqAction_t **head, char *pvname, char *value, acqMaster_t *master);
acqAction_t * new_acqAction_waitMotor( acqAction_t **head, char *motorName, acqMaster_t *master);
acqAction_t * new_acqAction_callEvent( acqAction_t **head, char *eventName, int autowait, acqMaster_t *master);
acqAction_t * new_acqAction_waitEvent( acqAction_t **head, char *eventName, acqMaster_t *master);
acqAction_t * new_acqAction_callScan( acqAction_t **head, char *scanName, int autowait, acqMaster_t *master);
acqAction_t * new_acqAction_waitScan( acqAction_t **head, char *scanName, acqMaster_t *master);
acqAction_t * new_acqAction_setControl( acqAction_t **head, acqMaster_t *master);
acqAction_t * new_acqAction_nextOutput( acqAction_t **head, acqMaster_t *master);

/**
 **
 **/

/*
 * minimum space allocation for PV list in events.
 */
#define NUM_EVENT_PV 10

/*
 * define the display range for a single PV. If no display ranges are defined,
 * display the first element (typical of non-array PV's). If a display range
 * is defined, assume an array.
 */
#define MAX_DISPLAY_RANGE 10
typedef struct displayPvRange displayPvRange_t;
struct displayPvRange {
	enum { singleVal, closedRange, openRange } displayType;
	unsigned int firstIdx, lastIdx;
};

/*
 * how to obtain an output value. The default is to use whatever value is currently ready;
 * usePvGet ensures that the held value is in sync with a remote server;
 * waitForMonitor allows us to delay until an update.
 */
typedef enum { useCurrent, usePvGet, waitForMonitor } acqPvReady_t;
/*
 * conEvent is only non-NULL during acquisition; it is always null
 * when user configuration is in progress.
 */
struct acqPv {
	Connector *conEvent;	/* connectors for the PV's */
	Connector *conDesc;	/* description field */
	char description[100];	/* value from description field */
	char *pvName;		/* names of PV's to be recorded */
	char *outputFormat;	/* if non-null, either decimal precision or a format stmt */
	unsigned int
		noRecord:1,		/* 0 (default) to record, 1 to not record */
		isSpectrum:1,		/* 0 if this data is being recorded to the spectrum file */
		waitState:1,		/* 0 if data ready, 1 if waiting for 'ready' condition to be true */
		haveMonitor:1;		/* 0 when waiting for monitor, 1 when monitor occurs */
	int displayRow;		/* clist row number for display; -1 if not in the display: NOT USED! */
	acqNoConnectResponse_t noConnect;		/* how to respond to a connection failure */
	acqPvReady_t	ready;	/* how this data is to be acquired */
	struct acqEvent *event;	/* circular reference to event structure containing this PV */
	int outputCount;	/* number of values that will be output by this PV if numDisplayRange is >0 */
	int numDisplayRange;	/* number of display ranges - usually 0 */
	displayPvRange_t displayRange[MAX_DISPLAY_RANGE];	
} ;

/*
 * Acquisition event structure. Special details:
 * pvList always points to a structure of at least 10 elements.
 * numPvList is the number of actual elements allocate: note the above mentioned minimum of 10.
 * from the 10-list event screen, numPvList is unchanged, so it always represents the number of
 * elements allocated. This may not correspond to the number of elements requested!
 */

struct acqEvent {
	acqMaster_t *master;
	struct acqEvent *next, *prev;
	char * eventName;	/* user name for this event */
	int eventNum;		/* ID for this event, used if no user name */
	int numPvList;		/* number of PV's in pvList */
	acqPv_t * pvList;	/* list of PV's */
	int outputCount;		/* number of events recorded */
	int shutdown;			/* non-zero to end this thread */
	acqState state;
	unsigned int
		putVerboseHeader:1,	/* set if the event should write one line per PV in the header */
		putCommentPrefix:1,	/* set if the event should be a comment (hash mark) in the data stream */
		putEventId:1,	/* set if the event ID should be in the data stream */
		putAbsTime:1,	/* put the absolute timestamp in the data stream */
		putRel0Time:1,	/* put the relative timestamp to the first event */
		putRelPTime:1;	/* put the relative timestamp to the previous event */
	epicsThreadId threadID;
	epicsEventId startEvent,
			endEvent,
			monitorPvEvent;
	epicsEventId connectNotify;	/* communication of change of connection status */
} ;
acqEvent_t * new_acqEvent(const char *eventName , acqMaster_t *master);
acqEvent_t * first_acqEvent(const acqMaster_t *master);
acqEvent_t * next_acqEvent(const acqEvent_t *);
acqEvent_t * lookup_acqEvent( const char * , const acqMaster_t *master);
void destroy_acqEvent( acqEvent_t * , acqMaster_t *master);

/**
 **
 **/

struct acqOneMotor {
	acqOneMotor_t *next, *prev;
	acqMotor_t *parent;
	char *name;
	Connector *busy;
	Connector *done;
	int moving;
};

struct acqMotor {
	acqMotor_t *next, *prev;
	acqMaster_t *master;
	char *name;		/* name of this motor group */
	acqOneMotor_t *list;	/* motors in this group */
	epicsEventId waitEvent;	/* signal that a change has occured */
	int waiting;		/* flag that we're waiting */
};

acqMotor_t *new_acqMotor( const char *name , acqMaster_t *master);
void destroy_acqMotor( acqMotor_t *m , acqMaster_t *master);
acqMotor_t *lookup_acqMotor( const char *name , const acqMaster_t *master);
acqMotor_t *first_acqMotor(const acqMaster_t *master);
acqMotor_t *next_acqMotor(const acqMotor_t *m);
int waitAcqMotor( acqMotor_t *m);

acqOneMotor_t *new_acqOneMotor( acqMotor_t *motorGroup, const char *name);
void destroy_acqOneMotor( acqOneMotor_t *om);
acqOneMotor_t *next_acqOneMotor(  acqOneMotor_t *om);

/**
 ** acqMaster
 ** All the library "global" variables are in a single structure.
 ** This supports multiple active acquisition configurations per application.
 **
 **/

typedef void (*progress_t)( acqMaster_t *, const char *, double);
typedef void (*onStop_t) (acqMaster_t *);
typedef void (*message_t)( acqMaster_t *, const char *fmt, ...);
typedef void (*messageboxclear_t)(acqMaster_t *);
typedef void (*putmode_t)(acqMaster_t *, acqState);

struct acqMaster {
	char *acqName;			/* identifier for this acquisition */
	acqEvent_t *acqEventList;	/* all the output events	*/
	acqScan_t *acqScanList;		/* all the scan structures	*/
	acqMotor_t *acqMotorList;	/* all the known motor groups	*/

	void * user_data;		/* available to the application calling the acquisition library */

	int globalShutdown;		/* set non-zero to request all threads to exit */
	int globalPause;		/* set non-zero to pause all scan threads */
	acqState globalState;		/* overall operational state */
	acqState lastState;		/* last reported operational state */
	int RunupDelay;			/* scan initialization timing: number of callbacks to delay before starting the run */
	int MsecDelay;			/* time interval between calls to monitor_status() */

	int acqDebug;			/* flag for debugging output */
	int acqVerbose;			/* flag for verbose output to standard output */

	int acqRuns;			/* count of the number of runs made since start: at least 1! */
	int acqRunMax;			/* number of runs to be made */
	
	unsigned int fileVersion;	/* version information from the last loaded file */

	/*
	 * registered output methods
	 */
	eventDataHandler_t ** outputHandler;
	void **outputKeys;
	int numOutputHandler;

	/*
	 * various callbacks
	 */
	progress_t showProgress;
	onStop_t onStop;

	/*
	 * message display support (Also callbacks)
	 */
	void * user_message_data;
	message_t messageAdd;
	messageboxclear_t messageBoxClear;
	void * user_mode_data;
	putmode_t putMode;
	void * user_status_data;
	message_t acqStatusMessage;

	/* support for multi-threaded epics application (when multiple epics environments are implemented...)
	 */
	struct ca_client_context * GlobalContext;
};

acqMaster_t *new_acqMaster(void);
/*
 * output handlers are registered with a master scan structure, the handler key,
 * and the callback structure.
 */
int acqRegisterOutputHandler(acqMaster_t *, void *, eventDataHandler_t *);
/* de-registers a specific output handler */
int acqRemoveOutputHandler(acqMaster_t *, void *);
/* de-register all output handlers */
// Changed this to void from int to fix compiler warning. See if it works. (David Chevrier, Aug 25 2011)
//int acqClearOUtputHandlers(acqMaster_t *);
void acqClearOUtputHandlers(acqMaster_t *);
/* broadcast a signal to all handlers (ignore 'key') */
int acqHandlerSignal( acqMaster_t *master, acqKey_t key, unsigned int signalID, const void *data);

/*
 * setup support
 */
int Run_mode(acqMaster_t *master);
int Standby_mode(acqMaster_t *master);
void startMonitorTask(acqMaster_t *master);
int acqMonitor_status(acqMaster_t *master);		/* NOTE: this is suitable for calling by gtk_timer() */
void acqMonitor_init(acqMaster_t *master);
void acqMonitor_putState(acqMaster_t *master, acqState state);
/*
 * support for callbacks from the monitoring task
 */
void acqSetShowProgress( progress_t f , acqMaster_t *master);
void acqSetOnStop( onStop_t f , acqMaster_t *master);

void acqSetMessageAdd( message_t f, acqMaster_t *master);
void acqSetMessageBoxClear( messageboxclear_t f, acqMaster_t *master);
void acqSetPutMode( putmode_t f, acqMaster_t *master);
void acqSetStatusMessage( message_t f, acqMaster_t *master);

/* returns the string name of the argument state */
char *curState(acqState st);	/* compatibility version */
char *acqCurState(acqState st);
acqState acqGetStateFromName(const char *name);	/* returns the state based on the string name */


	/* returns the index of the added event */
int addEventPv( acqEvent_t *event, char * name, int norecord, char *fmt, acqPvReady_t ready, int isSpectrum);
int deleteEventPv( acqEvent_t *event, int rowNum);
char * getPvReadyName( acqPvReady_t pva);
acqPvReady_t getPvReadyType( char *name);
void addScanControl( acqScan_t *scanp, acqControl_t *ctlp);
void deleteScanControl( acqScan_t *scanp, int index);
int erase_scanRecord_links( acqScan_t *scan);
void erase_eventRecord_links( acqEvent_t *event);

int getPVcolumn( const char*event, const char*pvname, const acqMaster_t *master);


/*
 * processing support
 */
void runEvent(acqEvent_t *);
void runAcqScan(acqScan_t *);

char * tgTypeName(int idx);	/* convert ACQ_TG names to strings */
int tgTypeIndex(char *tname);	/* converts strings to ACQ_TG values */

void new_PV_entry(void);
void delete_PV_entry(void);

void acq_setComment(const acqMaster_t *master, const char *comment);	/* inform the output handlers of a comment string */
/*
 * configuration file support
 */
void acq_file_load( const char *fname , acqMaster_t *master);
void acq_file_save_as(const char *fname , acqMaster_t *master);
void acq_xmlOutputConfig(const char *filename, acqMaster_t *master);
int acq_ReadXMLconfig(const char *filename, acqMaster_t *master);

/*
 * unfortunately, used by calling programs. This should be a utility function of some sort.
 */
void setCharPointerFromEntry( char **ptr, const char *str);

#if defined(__cplusplus)
}
#endif

#endif /* ACQUISITIONLIB_H */

/*
 * $Log: acquisitionLib.h  $
 * Revision 1.3.1.4 2009/03/04 15:13:46CST Glen Wright (wrightg) 
 * Revisions for caller-defined output streams
 * Revision 1.3.1.3 2008/05/28 13:53:11CST Glen Wright (wrightg) 
 * Initial 3.0 version
 * Revision 1.3.1.2 2007/11/14 17:04:02CST Glen Wright (wrightg) 
 * Can't have the same file check out multiple times. Sucks.
 * Revision 1.3.1.1 2007/03/18 15:05:21CST Glen Wright (wrightg) 
 * Duplicate revision
 */

