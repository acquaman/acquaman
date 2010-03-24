/**
 ** $Header: acqActSetup.c 1.5.1.3 2009/03/04 15:13:44CST Glen Wright (wrightg) Exp  $
 ** Copyright Canadian Light Source, Inc. All rights reserved.
 **
 ** Description:
 **   Support for building event and scan structures,
 **   linking structures,
 **   and verifying structures.
 **/

#include "acquisitionLib.internal.h"
#include <stdlib.h>

/*
 * number of monitor callbacks, and milliseconds between each callback
 */
static char * valid_scanRecord_settings( acqScan_t *as);
static void buildEventRange( acqPv_t *, const char *range);
/******
 *  Event Support
 ******/

/*
 * create a new acqEvent, and put it in the list
 */
acqEvent_t *
new_acqEvent(const char * eventName, acqMaster_t *master)
{
	acqEvent_t *acq;

	acq = calloc(1, sizeof (*acq));
	if( acq == NULL)
		return NULL;
	acq->master = master;
	acq->eventName = strdup(eventName);
	acq->pvList = calloc(1, sizeof (*acq->pvList) );	/* some applications expect 'at least 1'. 1st realloc should be a no-op */

	if( master->acqEventList == NULL)
	{
		master->acqEventList = acq;
		acq->next = acq->prev = acq;
		return acq;
	}
	acq->next = master->acqEventList;
	acq->prev = master->acqEventList->prev;
	acq->next->prev = acq;
	acq->prev->next = acq;
	return acq;
}

void
destroy_acqEvent( acqEvent_t *acq, acqMaster_t *master)
{
	int i;
	
	if( acq->eventName)
		free( acq->eventName);

	if( acq == master->acqEventList)
		master->acqEventList = acq->next;
	acq->prev->next = acq->next;
	acq->next->prev = acq->prev;
	if( acq == master->acqEventList)		/* last item in list */
		master->acqEventList = NULL;

	for( i=0; i < acq->numPvList; i++)
	{
		if( acq->pvList[i].pvName)
			free(acq->pvList[i].pvName);
		if( acq->pvList[i].outputFormat)
			free( acq->pvList[i].outputFormat);
	}
	if( acq->pvList)
		free( acq->pvList);
}

acqEvent_t *
first_acqEvent(const acqMaster_t *master)
{
	return master->acqEventList;
}

acqEvent_t *
next_acqEvent(const acqEvent_t *acq)
{
	acqEvent_t *evp;

	if( acq == NULL)
		return NULL;
	evp = acq->next;
	if( evp == acq->master->acqEventList)
		return NULL;
	return evp;
}

acqEvent_t *
lookup_acqEvent(const char *name, const acqMaster_t *master)
{
	char *e;
	int id;
	acqEvent_t *ev;

	for(ev=first_acqEvent(master); ev; ev=next_acqEvent(ev) )
		if( strcmp( ev->eventName, name) == 0)
			return ev;

	/*
	 * if numeric, try looking by ID
	 */
	id = strtol( name, &e, 0);
	if( *e == '\0' || *e == ' ')
	{
		for( ev=first_acqEvent(master); ev; ev=next_acqEvent(ev) )
			if( ev->eventNum == id)
				return ev;
	}

	return NULL;
}

int
addEventPv( acqEvent_t *ev, char *name, int norecord, char *fmt, acqPvReady_t ready, int isSpectrum)
{
	int idx;
	int cnt;
	int nbytes;

	if( name == NULL || ! *name )	/* don't add null PV's */
		return -1;

	idx = ev->numPvList;
	nbytes = (1+ev->numPvList)* sizeof (*ev->pvList);
	DEBUGM(ev->master,1) printf("adding event %d total bytes %d (%d*%d)\n", idx, nbytes, ev->numPvList+1, sizeof(*ev->pvList));
	ev->pvList = realloc(ev->pvList, nbytes);

	ev->numPvList++;
	ev->pvList[idx].pvName = strdup( name);
	ev->pvList[idx].noRecord = norecord;
	ev->pvList[idx].ready = ready;
	ev->pvList[idx].isSpectrum = isSpectrum;
	ev->pvList[idx].displayRow = -1;
	ev->pvList[idx].noConnect = NCR_IGNORE;
	if( fmt && *fmt)
		ev->pvList[idx].outputFormat = strdup( fmt);
	else
		ev->pvList[idx].outputFormat = NULL;
	/* RU! I'm not sure you should have added this.  Consider taking it back out.*/
	/* Glen: this allows passing the pvList directly as a callback, and then allows referencing
	 * the parent event structure of the PV
	 */
	for( cnt=0; cnt < ev->numPvList; cnt++)
		ev->pvList[cnt].event = ev;
	return idx;
}

/*
 * delete the structure containing an event PV. Don't worry about freeing the memory here, either realloc() or
 * free will handle it eventually.
 */
int
deleteEventPv( acqEvent_t *ev, int rowNum)
{
	int idx;
	int maxPv;

	if( ev == NULL)
		return -1;
	if( rowNum >= ev->numPvList)
		return -1;

	if( ev->pvList[rowNum].pvName)
		free(ev->pvList[rowNum].pvName) ;
	if( ev->pvList[rowNum].outputFormat )
		free(ev->pvList[rowNum].outputFormat);
	ev->pvList[rowNum].pvName = NULL;
	ev->pvList[rowNum].outputFormat = NULL;
	for( idx = rowNum; idx < ev->numPvList-1; idx++)
	{
		ev->pvList[idx] = ev->pvList[idx+1];
		ev->pvList[idx].event = ev;
	}
	// following lines are in case we're copying garbage from a higher location
	// this works because we're not feeing the memory being used, so the highest
	// spot is still valid memory.
	ev->pvList[idx].pvName = NULL;
	ev->pvList[idx].outputFormat = NULL;
	ev->numPvList--;
	return 0;
}

static char *
valid_eventRecord_settings( acqEvent_t *ev)
{

	return NULL;
}

static int
build_eventRecord_links( acqEvent_t *ev)
{
	int i;
	acqPv_t *apv;
	char realname[100];
	char *dot, *lbracket;
	char *realName;

	for( i=0; i < ev->numPvList; i++)
	{
		apv = &ev->pvList[i];
		if( apv->pvName && apv->pvName[0])
		{
			/* set up to read the event PV
		 	 */
			strncpy(realname, apv->pvName, sizeof realname);
			if( ( lbracket = strchr(apv->pvName, '[') ) != NULL)
			{
				realname[lbracket-apv->pvName] = '\0';
				buildEventRange(apv, lbracket);
			}
			else
			{
				realName = apv->pvName;
				apv->numDisplayRange = 0;
			}
			apv->conEvent = build_connector( NULL, realname);
			apv->conEvent->user_data = apv;
			apv->conEvent->newState = callback_eventPVstate;
			if( apv->ready == waitForMonitor)
			{
				apv->waitState = 0;
				apv->haveMonitor = 1;
				apv->conEvent->update = callback_PVmonitor;
			}
			if( apv->ready == usePvGet)
			{
				apv->waitState = 0;
			}
			/*
			 * set up to read the event PV description field
			 */
			apv->description[0] = '\0';
			dot = strchr(realname, '.');
			if( dot == NULL)
				dot = realname + strlen(realname);
			if( dot < &realname[sizeof realname - 6])
			{
				strcpy(dot, ".DESC");
				apv->conDesc = build_connector( NULL, realname);
				apv->conDesc->update  = callback_PVdescription;
				apv->conDesc->user_data = apv;
				if( CONNECT_VALID( apv->conDesc) )
					apv->conDesc->update( apv->conDesc);
			}
		}
	}

	ev->startEvent = epicsEventCreate(0);
	ev->endEvent = epicsEventCreate(0);
	ev->monitorPvEvent = epicsEventCreate(0);
	ev->outputCount = 0;
	return 1;
}

void
erase_eventRecord_links( acqEvent_t *ev)
{
	int i;

	for( i=0; i < ev->numPvList; i++)
	{
		if( ev->pvList[i].conEvent == NULL)
			continue;
		connector_disconnect( ev->pvList[i].conEvent);
		ev->pvList[i].conEvent = NULL;
	}

	if( ev->startEvent)
	{
		epicsEventDestroy(ev->startEvent);
		ev->startEvent = 0;
	}
	if( ev->endEvent)
	{
		epicsEventDestroy(ev->endEvent);
		ev->endEvent = 0;
	}
	if( ev->monitorPvEvent)
	{
		epicsEventDestroy(ev->monitorPvEvent);
		ev->monitorPvEvent = 0;
	}
	return;
}

static void
buildEventRange(acqPv_t *apv, const char *subscript)
{
	const char *st;
	char *end_st;	
	int idx = 0;
	unsigned int val;

	apv->numDisplayRange = 0;
	if( *subscript != '[')
		return;
	subscript++;
	for(st=subscript;idx <= MAX_DISPLAY_RANGE && subscript && subscript[0] != ']';subscript = end_st)
	{
		subscript += strspn(subscript, " ,");
		val = strtol( subscript, &end_st, 10);
		if( end_st == subscript)	/* invalid character */
			break;
		apv->displayRange[idx].firstIdx = val;
		end_st += strspn( end_st, " ");
		if ( *end_st == '-')
		{
			end_st += 1+strspn( end_st+1, " ");
			if( end_st[0] >= '0' && end_st[0] <= '9')
			{
				st = end_st;
				val = strtol(st, &end_st, 10);
				apv->displayRange[idx].lastIdx = val;
				apv->displayRange[idx++].displayType = closedRange;
			}
			else
			{
				apv->displayRange[idx++].displayType = openRange;
			}
		}
		else
			apv->displayRange[idx++].displayType = singleVal;
	}
	apv->numDisplayRange = idx;
}

/********
 * Scan Support
 *******/

/*
 * create a new acqScan, and put it in the list
 */
acqScan_t *
new_acqScan( const char *scanName, acqMaster_t *master)
{
	acqScan_t *scan;

	scan = calloc(1, sizeof (*scan) );
	if( scan == NULL)
		return NULL;
	scan->master = master;
	scan->scanName = strdup(scanName);
	scan->numControlPV = 1;
	scan->acqControlList = calloc(1, sizeof (acqControl_t) );

	if( master->acqScanList == NULL)
	{
		master->acqScanList = scan;
		scan->next = scan->prev = scan;
		return scan;
	}
	scan->next = master->acqScanList;
	scan->prev = master->acqScanList->prev;
	scan->next->prev = scan;
	scan->prev->next = scan;
	return scan;
}
void
destroy_acqScan( acqScan_t *scan, acqMaster_t *master)
{
	int i;
	if( scan == master->acqScanList)
		master->acqScanList = scan->next;
	scan->next->prev = scan->prev;
	scan->prev->next = scan->next;
	if( scan == master->acqScanList)		/* only entry in list */
		master->acqScanList = NULL;
	for( i=0; i < NUM_SCAN_TRIGGERS; i++)
	{
		while( scan->actions[i])
			destroy_acqAction( &scan->actions[i], scan->actions[i]->prev);
		
	}
	if( scan->acqControlList)
	{
		for( i=0; i < scan->numControlPV; i++)
		{
			if( scan->acqControlList[i].controlPV)
				free( scan->acqControlList[i].controlPV);
		}
		free( scan->acqControlList);
	}
	free(scan);
}

/*
 * add a new control entry. This is always at the end of the list
 */
void
addScanControl( acqScan_t *scanp, acqControl_t *ctlp)
{
	int i, sz;
	if( scanp->numControlPV == 1 && scanp->acqControlList->controlPV == NULL)
		i = 0;
	else
		i = scanp->numControlPV++;
	sz = sizeof (acqControl_t)*scanp->numControlPV;
	scanp->acqControlList = realloc( scanp->acqControlList, sz);
	if( ctlp == NULL)
		memset( &scanp->acqControlList[i], '\0', sizeof (acqControl_t) );
	else
	{
		scanp->acqControlList[i] = *ctlp;
		if( ctlp->controlPV)
			scanp->acqControlList[i].controlPV = strdup( ctlp->controlPV);
		if( ctlp->startMacro)
			scanp->acqControlList[i].startMacro = strdup( ctlp->startMacro);
		if( ctlp->deltaMacro)
			scanp->acqControlList[i].deltaMacro = strdup( ctlp->deltaMacro);
		if( ctlp->finalMacro)
			scanp->acqControlList[i].finalMacro = strdup( ctlp->finalMacro);
	}
}

void
deleteScanControl( acqScan_t *scanp, int index)
{
	int i;
	acqControl_t *ctlp;

	if( scanp == NULL || scanp->acqControlList == NULL )
		return;
	if( index < 0 || index >= scanp->numControlPV)
		return;

	ctlp = &scanp->acqControlList[index];
	if( ctlp->controlPV)
		free(ctlp->controlPV);
	for(i=index; i < scanp->numControlPV-1; i++)
		scanp->acqControlList[i] = scanp->acqControlList[i+1];
	if( scanp->numControlPV > 1)
		scanp->numControlPV--;
	else
		memset( scanp->acqControlList, '\0', sizeof (acqControl_t) );
}

/*
 * access the elements of the list
 */
acqScan_t *
first_acqScan( const acqMaster_t *master)
{
	return master->acqScanList;
}

acqScan_t *
next_acqScan(const acqScan_t *acq)
{
	acqScan_t *nptr;
	if( acq == NULL)
		return NULL;
	nptr = acq->next;
	if( nptr == acq->master->acqScanList)
		return NULL;
	return nptr;
	
}

acqScan_t *
lookup_acqScan(const char *name, const acqMaster_t *master)
{
	char *e;
	int id;
	acqScan_t *sc;

	for(sc=first_acqScan(master); sc; sc=next_acqScan(sc) )
		if( strcmp( sc->scanName, name) == 0)
			return sc;

	/*
	 * if numeric, try looking by ID
	 */
	id = strtol( name, &e, 0);
	if( *e == '\0' || *e == ' ')
	{
		for( sc=first_acqScan(master); sc; sc=next_acqScan(sc) )
			if( sc->scanNum == id)
				return sc;
	}

	return NULL;
}

/*
 * determine the value based on the 'have' flag, the macro string, and the stored value.
 * if the macro string is not in a macro form, it is the reponsibility of the calling application
 * to have interpreted the string and placed a reasonable value in the 'setting' field.
 * Otherwise, the string is expanded, and on a successful expanson to a value that can
 * be interpreted as a double this is returned. In the event of expansion failure, it is\
 * assumed that there is no actual value.
 */
static int
find_scanRecord_value( unsigned int haveSetting, double setting, char *macro, double *result)
{
	double val;
	char expanded[100];
	char *last;

	if( haveSetting == 0)
		return 0;
	if( macro && *macro == '$')
	{
		macro_expand(macro, expanded, NULL);
		val = strtod(expanded, &last);
		if( last != expanded)
		{
			*result = val;
			return 1;
		}
		return 0;
	}
	*result = setting;
	return 1;

}

/* returns >0 if record valid, 0 if there is a problem.
 * Does not validate links to other records.
 */
static char *
valid_scanRecord_settings( acqScan_t *as)
{
	int i;
	int checkDelta;		/* flag set when able to check delta value */
	acqControl_t *ctlp;
	acqControl_t settings;
	acqAction_t *actp;
	char *previousPV = NULL;

	if( as == NULL)
		return "internal error";
	if( as->acqControlList == NULL)
		return "internal error";
	memset( &settings, '\0', sizeof settings);
	for( i=0; i < as->numControlPV; i++)
	{
		ctlp = &as->acqControlList[i];
		if( ctlp->controlPV == NULL)
		{
			if( settings.controlPV == NULL)
				return "No control PV";
		}
		else
		{
			previousPV = settings.controlPV;
			settings.controlPV = ctlp->controlPV;
		}
		
		if( find_scanRecord_value(ctlp->haveStartVal, ctlp->startVal, ctlp->startMacro, &ctlp->startVal) != 0)
			settings.startVal = ctlp->startVal;
		else
			if( settings.haveFinalVal)		/* this can only be set after the first iteration */
				settings.startVal = settings.finalVal;
			else
				return "No starting value";

		if( find_scanRecord_value(ctlp->haveFinalVal, ctlp->finalVal, ctlp->finalMacro, &ctlp->finalVal) == 0)
			return "No final value";		/* no final value */

		settings.haveFinalVal = ctlp->haveFinalVal;

		settings.haveDeltaVal = find_scanRecord_value(ctlp->haveDeltaVal, ctlp->deltaVal, ctlp->deltaMacro, &ctlp->deltaVal);

		settings.finalVal = ctlp->finalVal;
		settings.deltaVal = ctlp->deltaVal;

		/*
		 * test the delta if:
		 * we have our own start value.
		 * OR
		 * the PV name is unchanged.
		 */
		checkDelta = ctlp->haveStartVal ||
			ctlp->controlPV == NULL ||
			( i > 0 && previousPV &&
			 strcmp(ctlp->controlPV,previousPV) == 0);
		if( settings.finalVal > settings.startVal)
		{
			if( checkDelta && settings.deltaVal <= 0)	/* 0 if 'haveDeltaVal' == 0 */
				return "need positive delta";
		}
		else if(settings.finalVal < settings.startVal)
		{
			if( checkDelta && settings.deltaVal >= 0)	/* 0 if 'haveDeltaVal' == 0 */
				return "need negative delta";
		}
	}

	as->needSetControl = 1;
	for( i=0; i < NUM_SCAN_TRIGGERS;i++)
	{
		for( actp=as->actions[i]; actp; actp=(actp->next==as->actions[i]?NULL:actp->next) )
		{
			switch( actp->type)
			{
			case AA_SET_CONTROL:
				if( i == ACQ_TG_MOVE)
					as->needSetControl = 0;
				break;

			case AA_DELAY_TIME:
				break;

			case AA_NEXT_OUTPUT:
				break;

			case AA_SET_PV:
				if( actp->au.spv.name == NULL)
					return "missing Name for setting PV";
				if( actp->au.spv.value == NULL)
					return "missing Value for setting PV";
				break;

			case AA_WAIT_PV:
				if( actp->au.wpv.name == NULL)
					return "missing Name for PV wait";
				if( actp->au.wpv.value == NULL)
					return "missing Value for PV wait";
				break;

			case AA_CALL_SCAN:
				if( actp->au.cs.name == NULL)
					return "missing Name for scan call";
				break;

			case AA_CALL_EVENT:
				if( actp->au.cs.name == NULL)
					return "missing Name for event call";
				break;

			case AA_WAIT_SCAN:
				if( actp->au.ws.name == NULL)
					return "missing Name for scan wait";
				break;

			case AA_WAIT_EVENT:
				if( actp->au.we.name == NULL)
					return "missing Name for event wait";
				break;

			case AA_WAIT_MOTOR:
				if( actp->au.wm.name == NULL)
					return "missing Name for motor wait";
				break;

			}
		}
	}
	return NULL;
}

/**
 ** build links for a scan record. Returns 1 on success, 0 if 1 or more links failed to be made.
 **/

static int buildAction( acqAction_t *ap)
{
	switch( ap->type)
	{
	case AA_DELAY_TIME:
		return 1;

	case AA_SET_PV:
		ap->au.spv.connector = build_connector( NULL, ap->au.spv.name);
		return 1;

	case AA_WAIT_PV:
		ap->au.wpv.connector = build_connector( NULL, ap->au.wpv.name);
		ap->au.wpv.waitFlag = 0;
		ap->au.wpv.endDelay = epicsEventCreate(0);
		ap->au.wpv.connector->update = callback_check_delay;
		ap->au.wpv.connector->user_data = ap;		/* circular reference */
		return 1;

	case AA_WAIT_MOTOR:
		ap->au.wm.motor = lookup_acqMotor( ap->au.wm.name, ap->master);
		return setup_acqMotor( ap->au.wm.motor);

	case AA_CALL_EVENT:
		ap->au.ce.event = lookup_acqEvent( ap->au.ce.name, ap->master);
		return ap->au.ce.event != NULL;

	case AA_CALL_SCAN:
		ap->au.cs.scan = lookup_acqScan( ap->au.cs.name, ap->master);
		return ap->au.cs.scan != NULL;

	case AA_WAIT_EVENT:
		ap->au.we.event = lookup_acqEvent( ap->au.we.name, ap->master);
		return ap->au.we.event != NULL;

	case AA_WAIT_SCAN:
		ap->au.ws.scan = lookup_acqScan( ap->au.ws.name, ap->master);
		return ap->au.ws.scan != NULL;

	case AA_SET_CONTROL:
		return 1;
	}
}

static int
build_scanRecord_links(acqScan_t *sc)
{
	int i;
	char *controlPV = NULL;

	DEBUGM(sc->master,1) printf("build_scanRecord_links\n");
	if( sc->acqControlList)
	{
		for(i=0; i < sc->numControlPV; i++)
		{
			if( sc->acqControlList[i].controlPV)
				controlPV = sc->acqControlList[i].controlPV;
			if( !controlPV)	/* should not happen: 'valid settings' checks this */
				return 0;
			sc->acqControlList[i].controlChan = build_connector( NULL, controlPV);
			sc->acqControlList[i].controlChan->user_data = (void *) sc;
			sc->acqControlList[i].controlChan->update = NULL;		/* set to callbacks for updates */
		}
	}

	for( i=0; i < NUM_SCAN_TRIGGERS; i++)
	{
		acqAction_t *ap;
		for( ap=sc->actions[i]; ap ; ap=(ap->next==sc->actions[i]?NULL:ap->next) )
		{
			if( buildAction( ap) == 0)
			{
				sc->master->messageAdd(sc->master, "can't link action %s type %d", tgTypeName(i), ap->type);
				return 0;
			}
		}
	}

	sc->startEvent = epicsEventCreate(0);
	sc->endEvent = epicsEventCreate(0);
	sc->putNotify = epicsEventCreate(0);
	if( sc->startEvent == 0 || sc->endEvent == 0)
		return 0;
	return 1;
}

/*
 * remove components from the action list at the end of the run
 */
eraseActionList( acqAction_t *act_h)
{
	acqAction_t *ap;

	for(ap=act_h; ap; ap = (ap->next==act_h?NULL:ap->next) )
	{
		switch(ap->type)
		{
		case AA_SET_CONTROL:
			continue;
		case AA_DELAY_TIME:
			continue;
		case AA_SET_PV:
			connector_disconnect( ap->au.spv.connector);
			ap->au.spv.connector = NULL;
			continue;
		case AA_WAIT_PV:
			ap->au.wpv.connector->update = NULL;		/* prevent inconvenient callbacks */
			connector_disconnect( ap->au.wpv.connector);
			ap->au.wpv.connector = NULL;
			epicsEventDestroy(ap->au.wpv.endDelay);
			ap->au.wpv.endDelay = 0;
			continue;
		case AA_WAIT_MOTOR:
			shutdown_acqMotor( ap->au.wm.motor);
			ap->au.wm.motor = NULL;
			continue;
		case AA_CALL_EVENT:
			ap->au.ce.event = NULL;
			continue;
		case AA_CALL_SCAN:
			ap->au.cs.scan = NULL;
			continue;
		case AA_WAIT_EVENT:
			ap->au.we.event = NULL;
			continue;
		case AA_WAIT_SCAN:
			ap->au.ws.scan = NULL;
			continue;
		}
	}
}

int
erase_scanRecord_links(acqScan_t *sc)
{
	int i;

	for( i=0; i < sc->numControlPV; i++)
	{
		if( sc->acqControlList && sc->acqControlList[i].controlChan)
		{
			connector_disconnect( sc->acqControlList[i].controlChan);
			sc->acqControlList[i].controlChan = NULL;
		}
	}
	if( sc->startEvent)
	{
		epicsEventDestroy(sc->startEvent);
		sc->startEvent = 0;
	}
	if( sc->endEvent)
	{
		epicsEventDestroy(sc->endEvent);
		sc->endEvent = 0;
	}
	if( sc->putNotify)
	{
		epicsEventDestroy(sc->putNotify);
		sc->putNotify = 0;
	}
	for(i=0; i < NUM_SCAN_TRIGGERS; i++)
	{
		eraseActionList(sc->actions[i]);
	}

	return 0;
}

/*
 * move from 'stop' to 'standby'.
 * return 1 on success, 0 if unable to set up.
 */
int
Standby_mode( acqMaster_t *master)
{
	acqScan_t *sc;
	acqEvent_t *ev;
	char *errMsg;
	int valid = 1;
	int nManualTrigger = 0;

	if( master->GlobalContext == NULL)
	{
		init_CA();
		master->GlobalContext = ca_current_context();
	}
	master->messageAdd(master, "Standby Mode");
	master->putMode( master, AS_STARTUP);
	/*
	 * verify content for all records
	 */
	for( sc = first_acqScan(master); sc ; sc = next_acqScan( sc) )
	{
		if ( ( errMsg = valid_scanRecord_settings(sc) ) != NULL )
		{
			master->messageAdd(master, "Scan %s not valid: %s", sc->scanName, errMsg);
			valid = 0;
		}
		if( sc->useStart)
			nManualTrigger++;
	}

	if( !valid)
	{
		master->putMode( master, AS_OFF);
		return 0;
	}

	if( nManualTrigger == 0)
	{
		master->messageAdd(master, "No Scans set for 'Trigger on Start'");
		return 0;
	}

	for( ev= first_acqEvent(master); ev ; ev = next_acqEvent( ev) )
	{
		if( ( errMsg = valid_eventRecord_settings(ev) ) != NULL )
		{
			master->messageAdd(master, "Event %s not valid: %s", ev->eventName, errMsg);
			valid = 0;
		}
	}

	if( !valid)
	{
		master->putMode(master,  AS_OFF);
		return 0;
	}

	DEBUGM(master,1) printf("Standby mode: building links\n");
	/*
	 * establish all links. on failure, clean up and return
	 */
	for( sc = first_acqScan(master); sc ; sc = next_acqScan(sc) )
	{
		if( build_scanRecord_links(sc) != 1)
		{
			master->messageAdd(master, "Linking for scan '%s' failed", sc->scanName);
			valid = 0;
			break;
		}
	}

	for( ev = first_acqEvent(master); ev ; ev = next_acqEvent(ev) )
	{
		if( build_eventRecord_links( ev) != 1)
		{
			master->messageAdd(master, "Linking for event '%s' failed", ev->eventName);
			valid = 0;
			break;
		}
	}

	if( !valid)
	{
		/* remove the links just created
		 */
		for( sc = first_acqScan(master) ; sc ; sc = next_acqScan(sc) )
			erase_scanRecord_links(sc);
		for( ev = first_acqEvent(master); ev ; ev = next_acqEvent(ev) )
			erase_eventRecord_links(ev);
		master->putMode(master,  AS_OFF);
		return 0;
	}

	master->putMode(master,  AS_STANDBY);
	return 1;
}

/*
 * Called after all inter-record links created. Start all threads.
 * Note that the event threads are started first.
 * moves the application from standby to run mode.
 */

int
Run_mode(acqMaster_t *master)
{
	acqEvent_t *ev;
	acqScan_t *sc;
	char threadName[50];

	master->acqRuns = 1;
	master->globalShutdown = 0;
	master->globalPause = 0;

	for( ev = first_acqEvent(master); ev ; ev = next_acqEvent(ev) )
	{
	 	snprintf(threadName, sizeof threadName, "event_%s", ev->eventName);
		ev->outputCount = 0;
		ev->state = AS_STARTUP;
	 	ev->threadID = epicsThreadCreate(threadName, epicsThreadPriorityMedium, epicsThreadGetStackSize(epicsThreadStackMedium), (EPICSTHREADFUNC)runEvent, ev);
	}

	for( sc = first_acqScan(master) ; sc ; sc = next_acqScan(sc) )
	{
	 	snprintf(threadName, sizeof threadName, "scan_%s", sc->scanName);
		sc->state = AS_STARTUP;
	 	sc->threadID = epicsThreadCreate(threadName, epicsThreadPriorityMedium, epicsThreadGetStackSize(epicsThreadStackMedium), (EPICSTHREADFUNC)runAcqScan, sc);
	}

	master->putMode(master,  AS_RUNUP);
	master->messageAdd(master, "Run Mode");
	return 0;
}

static char * stateNames[] = {
	"Off",
	"Startup",
	"Standby",
	"Runup",
	"Run",
	"Rundown",
	"Shutdown",
	"Fault"
};

char *
curState(acqState st)
{
	return acqCurState(st);
}

char *
acqCurState(acqState st)
{
	if( st < 0 || st >= AS_NUMBER)
		return "unknown";
	return stateNames[st];
}

acqState
acqGetStateFromName(const char *name)
{
	int i;
	for(i=0; i < sizeof stateNames/sizeof stateNames[0]; i++)
		if( strcmp(name, stateNames[i]) == 0)
			return (acqState)i;
	return AS_NUMBER;
}

/*
 * $Log: acqActSetup.c  $
 * Revision 1.5.1.3 2009/03/04 15:13:44CST Glen Wright (wrightg) 
 * Revisions for caller-defined output streams
 * Revision 1.5.1.2 2008/05/28 13:53:02CST Glen Wright (wrightg) 
 * Initial 3.0 version
 * Revision 1.5.1.1 2007/03/18 14:57:21CST Glen Wright (wrightg) 
 * Duplicate revision
 * Revision 1.4 2007/03/18 14:56:08CST matiase 
 * Added header/log
 * Revision 1.3 2007/03/18 14:55:30CST matiase 
 * added header/log
 */
