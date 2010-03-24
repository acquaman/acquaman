/* $Id: epicsConnectApp/src/epicsConnect.h 1.2 2008/07/16 16:50:06CST Glen Wright (wrightg) Exp  $ */
/*
 * Copyright Canadian Light Source, Inc. All rights reserved.
 *
 * Description:
 *    Basic structure for connecting a channel to an internal value,
 *
 */

#if !defined(CHANNEL_H)
#define CHANNEL_H 1

#include <cadef.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct cw_connector Connector;
typedef void (*CHANNEL_USERFUNC)(struct event_handler_args );

typedef struct ChannelGraphics {
	double lopr;		/* low operating range (minimum value, use (VAL >= lopr) for valid range check) */
	double hopr;		/* high operating range (maximum value, use (VAL <= hopr) for valid range check */
	short prec;		/* display precision from database */
	char units[MAX_UNITS_SIZE];	/* engineering units */
	char *states[MAX_ENUM_STATES];		/* for (at least) DBF_ENUM, the list of valid states for this PV */
} ChannelGraphics_t;

typedef struct Channel
{
	char * chan_name;		/* PV name used */
	chtype chan_type;		/* channel type */
	chid chan_id;			/* channel ID returned on 'search and connect' */
	evid eventId;			/* event ID used when monitoring */
	void *dataval;			/* pointer to first data value associated with this PV */
	void *databuf;			/* pointer to data storage used for this variable */
	int maxelement;			/* max number of elements for this PV */
	int nbytes;			/* number of bytes read in a monitor call */
	short int status;		/* alarm status: 0 = NO_ALARM, o/w contains type of alarm (see choiceGbl.ascii) */
	short int severity;		/* alarm severity: NONE, MINOR, MAJOR, INVALID */
	unsigned int
		unused:1,		/* 0 when first allocated, 1 if unreferenced with outstanding calls */
		outstanding:1,		/* 1 when a callback to this record is expected */
		valid:1,		/* 0 if channel not available, 1 if connected */
		monitor:1,		/* 0 if not monitoring changes, 1 if monitoring */
		haveData:1,		/* 0 if no associated data structure, 1 when available */
		createData:1,		/* 0 if user-supplied data structure, 1 if automatically allocated */
		initData:1,		/* 0 if user-supplied max element and type, 1 if needs discovery */
		updating:1,		/* 1 if a software-initiated update of a value, 0 otherwise */
		readAccess:1,		/* 1 if read access to this PV */
		writeAccess:1;		/* 1 if write access to this PV */
	Connector *first_connector;	/* list of connectors that reference this channel */
	int reprocess;			/* non-zero if processing has been delayed */
	struct Channel *next_proc;	/* list of reprocessing channels */
	ChannelGraphics_t cg;		/* default graphical info for this PV */
} Channel;


typedef struct channel_hash channel_hash_t;

struct channel_hash
{
	struct channel_hash *next;		/* list of names that hash to the same value */
	Channel *chan;				/* pointer to the channel entry */
	short int refcnt;			/* reference count for this instance of the channel */
	short int hashval;			/* index into the global hash structure */
	struct ca_client_context *context;	/* when disable_preemptive_callback on a thread, track the unique context */
};

/*
 * return the table entry for the named channel.
 * creates one if it doesn't exist.
 * refcnt is always incremented.
 * returns NULL on error.
 */
channel_hash_t *get_channel(char *);
/*
 * looks for the table entry. returns NULL on
 * lookup failure. refcnt is unchanged.
 */
channel_hash_t *lookup_channel(char *);
/*
 * decrements the reference count, and deletes the entry
 * if the reference count is <= 0;
 */
void
unref_channel( channel_hash_t *);

/**
 **
 ** channel/widget connector
 **/

struct cw_connector
{
	Connector *next;		/* next connector in an area */
	Connector *next_chan;		/* next connector to the same channel */
	channel_hash_t *chan;		/* channel connector hash */
	void *private_data;		/* Available to library application working directly with connectors */
	void * user_data;		/* available for user application */
	void (*update)(Connector *);	/* if non-null, function to call when values are updated */
	void (*newAccess)(Connector *);	/* if non-null, function to call when Access permissions are noted */
	void (*newState)(Connector *);	/* if non-null, function to call when channel state changes */
	char *unexpanded_pv;		/* name of PV before macro expansion */
	ChannelGraphics_t cg;		/* any over-rides of PV's default graphical info */
	short int useCG;		/* if non-zero, use the connector's channel graphics info */
	short int alarmSeverity;	/* as severity, but includes type '4' NO CONNECTION */
};

Connector * build_connector( void *private_data, const char *pvname);	/*builds a new connector */
void connector_disconnect( Connector *);			/*removes the channel reference & destroys the connector*/

/*
 * support for macro expansion
 */
typedef struct macroTable {
	struct macroTable *next;	/* single linked list */
	char *macroName;		/* lookup name of macro */
	char *macroVal;			/* expansion value */
	int expand;			/* non-zero if expanding the current macro */
} macroTable;

void macro_expand( const char * string, char *buffer, macroTable *macroFirst );	/* expand 'string' into 'buffer' */
void parse_macros( const char *macro_string, macroTable **macroFirst );		/* interpret 'name=value' pairs */
macroTable *macro_head();							/* return the default macro head */
macroTable *find_macro(const char *macro_name, const macroTable *macroFirst);	/* look up a macro name. returns NULL on failure */
void set_macroValue(macroTable *macrop, const char *value);			/* set a macro value */

int init_CA(void);
int init_CA_noPreempt(void);
int end_CA(void);
Channel * init_PV(char *name, chtype chan_type, void *dataptr, int size);
Channel * build_PV(char *);
int heartBeat(void);
int channel_put(Channel *, int);
int channel_put_callback( Channel *, int, CHANNEL_USERFUNC, void *arg);
int channel_put_acks(Channel *chan, int severity);
int channel_put_ackt(Channel *chan, int acknowledge);
int channel_get(Channel *);
int channel_async_get(Channel *, CHANNEL_USERFUNC, void *arg);

/*
 * data conversion functions
 */

void val_to_string(Channel *chan, char *result, int result_size, int idx);
void val_to_string_fmt(Channel *chan, char *result, int result_size, int idx, char *fmt);
void con_val_to_string( Connector *conp, char *result, int result_size, int idx);
void con_val_to_string_fmt( Connector *conp, char *result, int result_size, int idx, char *fmt);
int val_to_int( Channel *chan, int index);
double val_to_double( Channel *chan, int index);
int channel_put_int(Channel *chan, const int val);
int channel_put_double(Channel *chan, const double val);
int channel_put_double_callback(Channel *chan, const double val, CHANNEL_USERFUNC func, void *arg);
int channel_put_string(Channel *chan, const char * val);
int channel_put_int( Channel *chan, const int val);
int string_to_val(Channel *chan, const char *val, const int idx);
int double_to_val( Channel *chan, const double val, const int idx);




void epicsConnectAddContext( struct ca_client_context *context, int pre_empt);
void epicsConnectLock();
void epicsConnectUnlock();

#if defined(__cplusplus)
}
#endif


#endif

/*
 *  $Log: epicsConnectApp/src/epicsConnect.h  $
 *  Revision 1.2 2008/07/16 16:50:06CST Glen Wright (wrightg) 
 *  
 *  Revision 1.1 2008/05/08 10:19:20CST David Beauregard (beaured0) 
 *  Initial revision
 *  Member added to project e:/MKS_Home/archive/cs/epics_local/epics_tools/epicsConnect/project.pj
 *  Revision 1.5 2008/03/27 11:14:48CST Glen Wright (wrightg) 
 *  Exposed more methods for the macro table.
 *  Revision 1.4 2007/11/14 16:52:24CST Glen Wright (wrightg) 
 *  Updates according to change package; Changed library versioning to easily follow 3-point convention
 *  Revision 1.3 2007/05/19 12:38:00CST Elder Matias (matiase) 
 *  Added header/log
 */
