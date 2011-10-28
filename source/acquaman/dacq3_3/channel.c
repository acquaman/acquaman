/* $Header: epicsConnectApp/src/channel.c 1.3.1 2011/10/27 21:18:56CST David Chevrier (chevrid) Exp  $
 * Copyright Canadian Light Source, Inc. All rights reserved.
 *
 * Description:
 *     Channel access support
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "epicsConnect.h"
#include <epicsMutex.h>

int channelDebug;
#define DEBUGP if(channelDebug)

static void getChanInfo(struct event_handler_args arg);
static void connect_callback(struct connection_handler_args args);
static void monitorCallback(struct event_handler_args);
static void getAccessRights(struct access_rights_handler_args);

static int startupFlag;			/* when non-zero, callbacks should be delayed */
static Channel *reProcessList;		/* list of Channels that have outstanding processing requests */
static epicsMutexId processFlag;	/* lock the processing list */

/*
 * start channel access
 */
static int
init_CA_common(int flag)
{
    int stat;
    struct ca_client_context *context;
    stat = ca_context_create(flag?ca_enable_preemptive_callback:ca_disable_preemptive_callback);
    /* ECA_NOTTHREADED indicates that this thread is recognized as
     * part of an existing context, and the application will have to
     * trust the callbacks being made.
     */
    if( stat != ECA_NORMAL && stat != ECA_NOTTHREADED) {
	printf("Can't start channel access (%d)\n", stat);
	return -1;
    }
    if( processFlag == 0)
	    processFlag = epicsMutexCreate();

    context = ca_current_context();
    epicsConnectAddContext( context, flag);
    return 0;
}

int
init_CA()
{
	return init_CA_common(1);
}
int
init_CA_noPreempt()
{
	return init_CA_common(0);
}

int
enable_CA()
{
	Channel *chan;
	Connector *conp;

	if( processFlag == 0 )
	    processFlag = epicsMutexCreate();

	epicsMutexLock( processFlag);
	startupFlag = 0;
	for( chan=reProcessList; chan; chan = chan->next_proc)
	{
	    chan->reprocess = 0;
	    /*
	     * update the data values
	     */
	    for( conp=chan->first_connector; conp ; conp=conp->next_chan)
	    {
			if( conp->newState)
				(*conp->newState)(conp);
			DEBUGP printf("Calling function %p\n", conp->update);
			if( conp->update)
				conp->update(conp);
	    }
	}
	epicsMutexUnlock(processFlag);
	
	return 0;
}

int
disable_CA()
{
	startupFlag = 1;
	
	return 0;
}
/*
 * end channel access: note - should just clean up the CURRENT CONTEXT!
 */
int
end_CA()
{
    int stat;
    stat = ca_pend_event(0.01);
    if( stat != ECA_TIMEOUT && stat != ECA_NORMAL)
	printf("ca_pend_event failed\n");
#if 0
    stat = ca_task_exit();
    if( stat != ECA_NORMAL) {
	printf("ca_task_exit failed\n");
	return -1;
    }
#endif
    return 0;
}

/*
 * callback from connection handler: change the state of the channel valid
 * flag depending on the connection being up or down.
 * ASSUMPTIONS: chan_id is valid.
 */
static void
connect_callback(struct connection_handler_args args)
{
    Connector *conp;
    
    Channel *chan = ca_puser( args.chid);
    int status;
    
    DEBUGP printf("connect callback %p\n", chan);
    switch( args.op)
    {
    case CA_OP_CONN_UP:
    	if( chan->initData == 1)
	{
		chan->maxelement = ca_element_count(chan->chan_id);
		chan->chan_type = ca_field_type(chan->chan_id);
		chan->initData = 0;
	}
	/*
	 * note that a dropped and recreated connection can cause a free and alloc. This
	 * is not always redundant, as software may have restarted with different sizes
	 * for the field!
	 */
	if( chan->createData == 1 )
	{
		if( chan->dataval)
			free( chan->dataval);
		chan->dataval = calloc(dbr_size[chan->chan_type], chan->maxelement+1);
	}

	chan->readAccess = ca_read_access( chan->chan_id);
	chan->writeAccess = ca_write_access( chan->chan_id);
	status = ca_array_get_callback( dbf_type_to_DBR_CTRL(ca_field_type(chan->chan_id)), 1, chan->chan_id, getChanInfo, (void*)chan);
	SEVCHK(status, "connect_callback : ca_get_DBR_CTRL_event failed");
	status = ca_replace_access_rights_event( chan->chan_id, getAccessRights);
	SEVCHK(status, "connect_callback : ca_replace_access_rights_event failed");

	chan->valid = 1;
	chan->outstanding = 0;
	if( chan->monitor)
	{
	    chan->status = 100;	/* MAGIC: an absurd status */
	    status = ca_add_array_event(dbf_type_to_DBR_STS(chan->chan_type), chan->maxelement, chan->chan_id, monitorCallback, chan, 
		    		(double) 0.0, (double) 0.0, (double) 0.0, &(chan->eventId));
	    SEVCHK(status, "connect_callback : ca_add_event failed");
	    status = ca_flush_io();
	    SEVCHK(status, "connect_callback : ca_flush_io failed");
	}
	break;
	
    case CA_OP_CONN_DOWN:
	if( chan->valid && chan->eventId)
	    ca_clear_event(chan->eventId);
	chan->valid = 0;
	chan->status = 99;
	
	/*
	 * update the access rights and severity for all the connectors
	 */
	for(conp=chan->first_connector; conp ; conp=conp->next)
	{
	    if(conp->newAccess)
		(*conp->newAccess)(conp);
	    if( conp->newState)
	    	(*conp->newState)(conp);
	}
	
	break;
    default:
	break;
    }

    return;
}
    
/*
 * create a channel entry, and lookup all details
 */
Channel *
build_PV(char * name)
{
	Channel *chan;
	int st;

	chan = (Channel *) calloc( 1, sizeof *chan);
	if( !chan)
		return 0;
	chan->chan_name = strdup(name);
	chan->initData = 1;
	chan->createData = 1;
	chan->monitor = 1;
	chan->outstanding = 1;
	st = ca_search_and_connect(name, &(chan->chan_id), connect_callback, chan);
	DEBUGP printf("ca_search_and_connect = %d\n", st);
	ca_flush_io();
	return chan;
}

Channel *
init_PV(char *name, chtype chan_type, void *dataptr, int size)
{
    Channel *chan;
    int st;
    
    DEBUGP printf("init_PV(%s, ...)\n", name);
    chan = (Channel *) calloc( 1, sizeof (Channel));
    if( !chan)
	return 0;
    DEBUGP printf("Channel Allocated %p\n", chan);
    chan->chan_name = strdup(name);
    chan->chan_type = chan_type;
    chan->dataval = dataptr;
    chan->maxelement = size;
    chan->outstanding = 1;
    
    st = ca_search_and_connect(name, &(chan->chan_id), connect_callback, chan);
    DEBUGP printf("ca_search_and_connect = %d\n", st);
    ca_flush_io();
    return chan;
}


int
channel_put(Channel *chan, int count)
{
    int status;
    
    DEBUGP printf("channel_put %s %d %s\n", chan->chan_name, count, chan->valid?"valid":"invalid");
    if(! chan->valid)
	return -1;
    if( count > chan->maxelement)
	count = chan->maxelement;
    status = ca_array_put(chan->chan_type, count, chan->chan_id, chan->dataval);
    SEVCHK(status, "channel_put : ca_array_put failed");
    ca_flush_io();
    return 0;
}

int
channel_put_callback( Channel *chan, int count, CHANNEL_USERFUNC func, void *arg)
{
    int status;
    
    DEBUGP printf("channel_put_callback %s %d %s\n", chan->chan_name, count, chan->valid?"valid":"invalid");
    if( !chan->valid)
	return -1;
    if( count > chan->maxelement)
	count = chan->maxelement;
    chan->outstanding = 1;
    status = ca_array_put_callback( chan->chan_type, count, chan->chan_id, chan->dataval, func, arg);
    SEVCHK(status, "channel_put_callback : ca_array_put_callback failed");
    ca_flush_io();
    return 0;
}

int
channel_get(Channel *chan)
{
    DEBUGP printf("channel_get %s %s\n", chan->chan_name, chan->valid?"valid":"invalid");
    if(! chan->valid)
	return -1;
    
    ca_array_get(chan->chan_type, chan->maxelement, chan->chan_id, chan->dataval);
    ca_flush_io( );
    return 0;
}

int
channel_async_get(Channel *chan, CHANNEL_USERFUNC func, void * arg)
{
    DEBUGP printf("channel_async_get %s %s\n", chan->chan_name, chan->valid?"valid":"invalid");
    if(! chan->valid)
	return -1;
    
    chan->outstanding = 1;
    ca_array_get_callback(chan->chan_type, chan->maxelement, chan->chan_id, func, arg);
    
    /* no pend_io or flush_io, responsibility of caller to set that up */
    return 0;
}

int
channel_put_acks(Channel *chan, int severity)
{
	short s_severity = severity;
	int status;

	if(! chan->valid)
		return -1;
	status = ca_array_put( DBR_PUT_ACKS, 1, chan->chan_id, &s_severity);
	SEVCHK(status, "channel_put : ca_array_put ACKS failed");
	ca_flush_io();
	return 0;
}

int
channel_put_ackt(Channel *chan, int acknowledge)
{
	short s_acknowledge = acknowledge;
	int status;

	if(! chan->valid)
		return -1;
	status = ca_array_put( DBR_PUT_ACKT, 1, chan->chan_id, &s_acknowledge);
	SEVCHK(status, "channel_put : ca_array_put ACKT failed");
	ca_flush_io();
	return 0;
}

int
heartBeat()
{
    ca_pend_event( 0.001);
    return 1;
}

/*
 * called when a PV sends a value. If the 'update' variable for this channel
 * is non-null, it is called to perform the update.
 * watches for alarm status and severity changes.
 */
typedef union DataBuf {
	struct dbr_sts_string strng;
	struct dbr_sts_short s;
	struct dbr_sts_float f;
	struct dbr_sts_enum e;
	struct dbr_sts_char c;
	struct dbr_sts_long l;
	struct dbr_sts_double d;
} DataBuf;


static void
monitorCallback(struct event_handler_args arg)
{
    Channel *chan;
    Connector *conp;
    int nbytes;
    int status, severity;
    int newState;
    
    chan = (Channel *) arg.usr;
    if( chan->unused)
	return;
   
    DEBUGP printf("monitorCallback chan %p %s\n", chan, chan->chan_name);
    DEBUGP printf("arg.type=%ld, arg.count=%ld\n", arg.type, arg.count);

    if( arg.status != ECA_NORMAL)
    {
	int newSeverity = CA_EXTRACT_SEVERITY(arg.status);
	if (newSeverity == chan->severity)
	    return;
	chan->severity = newSeverity;
	for( conp=chan->first_connector; conp ; conp=conp->next_chan)
	{
	    if( conp->newState)
		(*conp->newState)(conp);
	}
	return;
    }
    chan->nbytes = nbytes = dbr_value_size[arg.type] * arg.count;
    DEBUGP printf("Chan %s got %d bytes offset %d\n", chan->chan_name, chan->nbytes, dbr_value_offset[arg.type]);
    memcpy(chan->dataval, dbr_value_offset[arg.type]+(char *)arg.dbr, nbytes);

    /*
     * handle any alarm state
     */
    status = ((DataBuf *)arg.dbr)->s.status;
    severity = ((DataBuf *)arg.dbr)->s.severity;
    newState = status != chan->status || severity != chan->severity;
    chan->status = status;
    chan->severity = severity;

    if( startupFlag)
    {
    	if (chan->reprocess)
		return;

    	epicsMutexLock(processFlag);
	chan->reprocess++;
	chan->next_proc = reProcessList;
	reProcessList = chan;
	epicsMutexUnlock(processFlag);
	return;
    }
    /*
     * update the data values
     */
    for( conp=chan->first_connector; conp ; conp=conp->next_chan)
    {
	if( newState && conp->newState)
		(*conp->newState)(conp);
	    DEBUGP printf("Calling function %p\n", conp->update);
	    if( conp->update)
		conp->update(conp);
    }
}

/*
 * this should be a union of all possible graphics control returns
 */
typedef union InfoBuf {
	struct dbr_ctrl_int ca_int;
	struct dbr_ctrl_short ca_short;
	struct dbr_ctrl_float ca_float;
	struct dbr_ctrl_enum ca_enum;
	struct dbr_ctrl_char ca_char;
	struct dbr_ctrl_long ca_long;
	struct dbr_ctrl_double ca_double;
} InfoBuf_t;

static void
getChanInfo( struct event_handler_args arg)
{
	Channel *chan;
	// Commented out to avoid comparison warning (David Chevrier, Aug 25 2011)
	//int bytecount;
	unsigned int bytecount;
	InfoBuf_t InfoBuf;

	chan = arg.usr;
	if( chan == NULL)
		return;
	/* make sure we can accept any new data, even if it's in a format
	 * we don't know
	 */
	bytecount = dbr_size_n(arg.type, arg.count);
	if( bytecount > sizeof InfoBuf)
		return;
	memcpy( (void*)&InfoBuf, arg.dbr, bytecount );
	switch( ca_field_type(chan->chan_id) )
	{
	/* DBF_INT: note: this is identical to DBF_SHORT */
	case DBF_SHORT:
		chan->cg.lopr = InfoBuf.ca_short.lower_disp_limit;
		chan->cg.hopr = InfoBuf.ca_short.upper_disp_limit;
		strncpy(chan->cg.units, InfoBuf.ca_short.units, MAX_UNITS_SIZE);
		break;
	case DBF_FLOAT:
		chan->cg.lopr = InfoBuf.ca_float.lower_disp_limit;
		chan->cg.hopr = InfoBuf.ca_float.upper_disp_limit;
		strncpy(chan->cg.units, InfoBuf.ca_float.units, MAX_UNITS_SIZE);
		chan->cg.prec = InfoBuf.ca_float.precision;
		break;
	case DBF_ENUM:
	    {
		int i;

		chan->cg.lopr = 0;
		chan->cg.hopr = InfoBuf.ca_enum.no_str - 1;
		for(i=0; i < MAX_ENUM_STATES; i++)
		{
			if( chan->cg.states[i] == NULL)
				continue;
			free( chan->cg.states[i]);
			chan->cg.states[i] = NULL;
		}
		for(i=0; i <= chan->cg.hopr; i++)
		{
			chan->cg.states[i] = strdup(InfoBuf.ca_enum.strs[i] );
		}
	    }
	    break;
	case DBF_CHAR:
		chan->cg.lopr = InfoBuf.ca_char.lower_disp_limit;
		chan->cg.hopr = InfoBuf.ca_char.upper_disp_limit;
		strncpy(chan->cg.units, InfoBuf.ca_char.units, MAX_UNITS_SIZE);
		break;
	case DBF_LONG:
		chan->cg.lopr = InfoBuf.ca_long.lower_disp_limit;
		chan->cg.hopr = InfoBuf.ca_long.upper_disp_limit;
		strncpy(chan->cg.units, InfoBuf.ca_long.units, MAX_UNITS_SIZE);
		break;
	case DBF_DOUBLE:
		chan->cg.lopr = InfoBuf.ca_double.lower_disp_limit;
		chan->cg.hopr = InfoBuf.ca_double.upper_disp_limit;
		strncpy(chan->cg.units, InfoBuf.ca_double.units, MAX_UNITS_SIZE);
		chan->cg.prec = InfoBuf.ca_double.precision;
		DEBUGP printf("read precision %d units %s\n", chan->cg.prec, chan->cg.units);
		break;
	default:
	    break;
	}
}

/*
 * get the access rights:
 * note that even though there is separate read and write access, the rights
 * as of R3.14.1 are actually [ NONE, READ, and READ/WRITE]. If this changes,
 * things could get interesting.
 */
static void
getAccessRights( struct access_rights_handler_args args)
{
	Channel *chan = ca_puser(args.chid);
	Connector *conp;
	int readAccess, writeAccess;

	readAccess = ca_read_access(chan->chan_id);
	writeAccess = ca_write_access(chan->chan_id);
	chan->writeAccess = writeAccess?1:0;
	chan->readAccess = readAccess?1:0;

	DEBUGP printf("Channel %s read %d write %d\n", chan->chan_name, readAccess, writeAccess);
	/*
	 * user now allowed to write to this PV
	 */
	epicsMutexLock( processFlag);
	for( conp=chan->first_connector; conp; conp=conp->next_chan)
	    if( conp->newAccess)
		(*conp->newAccess)(conp);
	epicsMutexUnlock( processFlag);
	
}

void
epicsConnectLock()
{
	if( processFlag == 0)
		processFlag = epicsMutexCreate();
	epicsMutexLock( processFlag);
}

void
epicsConnectUnlock()
{
	if( processFlag == 0)
		return;
	epicsMutexUnlock( processFlag);
}

/*
 * $Log: epicsConnectApp/src/channel.c  $
 * Revision 1.3.1 2011/10/27 21:18:56CST David Chevrier (chevrid)
 * Revision 1.3 2010/11/02 11:45:39CST Glen Wright (wrightg)
 * Fix close of channel that had error return from ca_subscription()
 * Revision 1.2 2009/12/23 09:58:56CST Glen Wright (wrightg) 
 * Compilation messages cleanup;
 * more cautious cleanup on removal of a connector and channel;
 * bug fix when monitor callback structure doesn't have ECA_NORMAL as a status.
 * Revision 1.1 2008/05/08 10:19:18CST David Beauregard (beaured0) 
 * Initial revision
 * Member added to project e:/MKS_Home/archive/cs/epics_local/epics_tools/epicsConnect/project.pj
 * Revision 1.2 2007/05/19 12:37:02CST Elder Matias (matiase) 
 * Added header/log
 */

/*
 * Local variables:
 * c-indent-level: 4
 * c-basic-offset: 4
 * tab-width: 8
 * End:
 */
