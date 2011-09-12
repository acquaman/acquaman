/**
 ** $Header: acqMonitor.c 1.4.1.5 2010/01/18 16:22:01CST Glen Wright (wrightg) Exp  $
 ** Copyright Canadian Light Source, Inc. All rights reserved.
 **
 ** Description:
 **    Support for building event and scan structures,
 **    linking structures,
 **    and verifying structures.
 **/

#include "acquisitionLib.internal.h"

#define UNUSED(x) (void)(x)
/*
 * number of monitor callbacks, and milliseconds between each callback
 */
// Commented out to avoid compiler warning (David Chevrier, Aug 25 2011)
//static char * valid_scanRecord_settings( acqScan_t *as);

static void defaultProgress(acqMaster_t *master, const char *title, double percent);
static void defaultonStop(acqMaster_t *master);

/* turn a SECONDS indicator into the number of calls
 * of the acq_showStatus(master) call, rounded up.
 */
#define tickCount(MASTER, SECONDS) (((SECONDS)*1000+MASTER->MsecDelay-1)/MASTER->MsecDelay)

void
acqMonitor_init(acqMaster_t *master)
{
	if( master->MsecDelay == 0)
		master->MsecDelay = MSEC_DELAY;
	master->RunupDelay = tickCount(master, RUNUP_DELAY);
	acqMonitor_putState(master, AS_OFF);
}

void
acqMonitor_putState(acqMaster_t *master, acqState mode)
{
	if( mode == master->globalState)	/* if this is our current state, do nothing */
		return;
	master->lastState = master->globalState;
	master->globalState = mode;
	/*
	 * execute any end-of-state code
	 */
	switch (master->lastState)
	{
		case AS_OFF:
		case AS_STARTUP:
		case AS_STANDBY:
		case AS_RUNUP:
		case AS_RUN:
		case AS_RUNDOWN:
		case AS_SHUTDOWN:
		case AS_FAULT:
		default:
		;
	}
	/*
	 * execute any start-of-state code
	 */
	switch (master->globalState)
	{
		case AS_OFF:
			HANDLERS(master, odh->stop_cb(key) );
			break;
		case AS_STARTUP:
			break;
		case AS_STANDBY:
			break;
		case AS_RUNUP:
			HANDLERS(master, odh->start_cb(key) );
			break;
		case AS_RUN:
			break;
		case AS_RUNDOWN:
			break;
		case AS_SHUTDOWN:
			HANDLERS(master, odh->shutdown_cb(key) );
			break;
		case AS_FAULT:
			break;
		default:
			break;
	}

	master->putMode(master, mode);
}

/*
 * control the operating status: STANDBY/RUN/OFF
 * If all scans and events stopped: OFF
 * If some scans RUN and some scans STANDBY: RUN
 *
 */
int
acqMonitor_status(acqMaster_t *master)
{
	acqScan_t *sc;
	acqEvent_t *ev;
	char result[40];
	int i;
	int num_active;
	unsigned int stateCount[AS_NUMBER];
	acqState state;
	static int inactive;

	master->messageBoxClear(master);

	if( master->globalPause)
	{
		master->showProgress(master, "Paused...", 0.0);
		return 1;
	}
	if( master->RunupDelay > 0)
	{
		int numItems=0;
		
		for(i=0; i < AS_NUMBER; i++)
			stateCount[i] = 0;
		for( sc = first_acqScan(master); sc; sc=next_acqScan(sc) )
		{
			stateCount[sc->state]++;
			numItems++;
		}
		for(ev = first_acqEvent(master); ev; ev=next_acqEvent(ev) ){
			stateCount[ev->state]++;
			numItems++;
		}
		
		master->RunupDelay--;
		/* if all threads started successfully, then initialization is complete
		 */
		//if( stateCount[AS_STANDBY] >= numItems)		/* safety check: if numItems is 0, consider this done (avoids later divide by zero */
		// Cast to unsigned to fix compiler warning (David Chevrier, Aug 25 2011)
		if( stateCount[AS_STANDBY] >= (unsigned)numItems)		/* safety check: if numItems is 0, consider this done (avoids later divide by zero */
			master->RunupDelay = 0;
		
		if( master->RunupDelay)
		{
			double progress = stateCount[AS_STANDBY];
			progress /= numItems;			/* by above check, non-zero value */
			
			master->showProgress(master, "Initializing...", progress);
			acqMonitor_putState(master, AS_RUNUP);
			return 1;
		}
		/*
		 * notify output handlers that the first pass is starting,
		 * this must occur before the start signal is sent that
		 * causes the event-specific record outputs to occur.
		 */
		HANDLERS(master, odh->startrepeat_cb(key, 1) );
		HANDLERS(master, odh->nextOutput_cb(key) );
		/* signal the thread to start processing
		 */
		for( sc = first_acqScan(master) ; sc ; sc=next_acqScan(sc) )
			if( sc->useStart)
				epicsEventSignal( sc->startEvent);
		/*
		 * timing condition: if none of the tasks have processed their
		 * startup by the time this routine is re-entered, then all
		 * the tasks will list as 'standby', and this will be considered
		 * a condition to initiate a shutdown.
		 * The 'inactive' count allows more time for threads to become active.
		 */
		inactive = -tickCount(master,2);
		return 1;
	}

	for(i=0; i < AS_NUMBER; i++)
		stateCount[i] = 0;

	for( sc = first_acqScan(master); sc; sc=next_acqScan(sc) )
	{
		Connector *conp;
		/*
		 * user report
		 */
		master->messageAdd(master, "Scan %s: %s", sc->scanName, curState(sc->state) );
		if( sc->state == AS_RUN)
			conp = sc->acqControlList[sc->currentControl].controlChan;
		else
			conp = sc->acqControlList[0].controlChan;
		if( conp)
		{
			if(conp->chan->chan->valid == 0)
				strcpy(result, "<no connection>");
			else
				val_to_string( conp->chan->chan, result, sizeof result, 0);
			master->messageAdd(master, "control %s: %s", conp->chan->chan->chan_name, result);
		}
		else
			master->messageAdd(master,"No valid control PV\n");

		stateCount[sc->state]++;
	}

	/* count those running or just being initialized
	 */
	num_active = stateCount[AS_STARTUP] + stateCount[AS_RUNUP] + stateCount[AS_RUN] + stateCount[AS_RUNDOWN];
	/*
	 * when activity noted, clear the inactivity reports count. Otherwise, increment
	 * the count and trigger an all-threads shutdown once N or more (n == 2) passes
	 * have shown no activity.
	 */
	if( num_active)
		inactive = 0;
	else
	{
		if( ++inactive >= tickCount(master,2) )
		{
			HANDLERS( master, odh->endrepeat_cb(key, master->acqRuns) );
			/*
			 * two choices: if the number of runs has been satisfied, start the shutdown process.
			 * otherwise, open a new run file and trigger a new run.
			 */
			if( master->acqRuns++ >= master->acqRunMax)
			{
				inactive = 0;
				master->globalShutdown = 1;
				/* the handler notification doesn't occur until all scans have reported
				 * a shutdown.
				 */
			}
			else
			{
				inactive = -tickCount(master,5);		/* take some leeway */
				HANDLERS( master, odh->startrepeat_cb(key, master->acqRuns) );
				HANDLERS( master, odh->nextOutput_cb(key) );
				/* signal the thread to start processing
				 */
				for( sc = first_acqScan(master) ; sc ; sc=next_acqScan(sc) )
					if( sc->useStart)
						epicsEventSignal( sc->startEvent);
			}
		}
	}

	/*
	 * determine state just on scans.
	 */
	if( num_active > 0 && stateCount[AS_OFF] > 0)
		state = AS_STANDBY;
	//else if( num_active > 0 && num_active == stateCount[AS_RUN] )
	// Cast to unsigned to avoid compiler warning (David Chevrier, Aug 25 2011)
	else if( num_active > 0 && (unsigned)num_active == stateCount[AS_RUN] )
		state = AS_RUN;
	else
		state = AS_STANDBY;

	for( ev = first_acqEvent(master); ev; ev=next_acqEvent(ev) )
	{
		master->messageAdd(master, "Event %s: %s, count %d", ev->eventName, curState(ev->state), ev->outputCount );
		for(i=0; i < ev->numPvList; i++)
		{
			Channel *ch;
			if( ev->pvList[i].conEvent == NULL)
				continue;
			ch = ev->pvList[i].conEvent->chan->chan;
			if( ! ch->valid )
				master->messageAdd(master, "Warning: PV %s: Not connected", ev->pvList[i].pvName);
		}
		stateCount[ev->state]++;
	}

	/*
	 * more stringent 'active' definition: not OFF or FAULT. This count
	 * is used to determine if all threads have been shut down.
	 */
	num_active = 0;
	for(i=0; i < AS_NUMBER; i++)
		if ( i != AS_OFF && i != AS_FAULT)
			num_active += stateCount[i];
	if( num_active == 0)
	{
		acqMonitor_putState( master, AS_SHUTDOWN);
		master->messageAdd(master, "All threads shut down");
		for( sc = first_acqScan(master); sc; sc=next_acqScan(sc) )
			erase_scanRecord_links( sc);
		for( ev = first_acqEvent(master) ; ev ; ev=next_acqEvent(ev) )
			erase_eventRecord_links( ev);
		/*
		 * note: return 0 when status monitoring done.
		 * this allows tasks that watch the return value (e.g. gtk_timer)
		 * to not reschedule once the end of the run has been determined.
		 */
		master->showProgress(master, "Done", 1.0);
		master->onStop(master);
		acqMonitor_putState( master, AS_OFF);
		return 0;
	}
	acqMonitor_putState(master, state);
	/*
	 * Display the progress labels and bars
	 *
	 * What is done: look at the active scan(s), count the loops, estimate total loops, give a percentage.
	 * What should be done:
	 * for each 'trigger at start' scan, estimate the completion, and take the 'active' scans percentage
	 * estimate. 
	 */
	if( state == AS_RUN)
	{
		char info[100];
		float percent = 0.0;
		acqControl_t *curctlp, *ctlp;
		int idx;
		double loopEstimate, loopCount;

		loopEstimate = 0;
		loopCount = 0;
		info[0] = '\0';

		for( sc = first_acqScan(master); sc; sc=next_acqScan(sc) )
		{
			double thisEstimate = 0;

			if( sc->state != AS_RUN)
				continue;

			if( sc->useStart == 0)		/* triggered by other events, or not used! */
				continue;
			curctlp = & sc->acqControlList[sc->currentControl];

			/* estimate total number of loops */
			for(idx=0; idx < sc->numControlPV; idx++)
			{
				double deltav, startv, finalv;

				ctlp = &sc->acqControlList[idx];
				if( ctlp->haveStartVal)
					startv = ctlp->startVal;
				else
					if( idx <= sc->currentControl)
						startv = ctlp->recordStart;
					else
					{
						if( ctlp->controlPV == NULL)
							startv = ctlp[-1].finalVal;
						else
							startv = 0;	/* weak assumption */
					}
				if( ctlp->haveFinalVal)
					finalv = ctlp->finalVal;
				else
					finalv = startv;
				if( ctlp->haveDeltaVal)
					deltav = ctlp->deltaVal;
				else
					deltav = (finalv-startv)/10;
				if( deltav)
					thisEstimate += 1 + (finalv - startv)/deltav;
				else
					thisEstimate++;
			}
			VERBOSE(master) printf("Estimating %d loops of %g\n", sc->count, thisEstimate);
			loopCount += sc->count;
			loopEstimate += thisEstimate;
			if( sc->count > 0 && sc->count < thisEstimate)
				snprintf(info, sizeof info, "Scan: %s PV: %s", sc->scanName, curctlp->controlChan->chan->chan->chan_name);
		}
			
		if( loopEstimate > 0)
			percent = loopCount / loopEstimate;
		if( percent > 1)
			percent = 1;
		master->showProgress( master, info, percent);
	}
	return 1;
}

void
acqSetShowProgress( progress_t f, acqMaster_t *master)
{
	if( master == NULL)
		return;
	if( f)
		master->showProgress = f;
	else
		master->showProgress = defaultProgress;
}

static void
defaultProgress(acqMaster_t * master, const char *title, double percent)
{
	// Unused parameter to avoid compiler warning (David Chevrier, Aug 25 2011)
	UNUSED(master);
	printf("state %s completion %g\n", title, percent);
}


void
acqSetOnStop( onStop_t f, acqMaster_t *master)
{
	if( master == NULL)
		return;
	if( f)
		master->onStop = f;
	else
		master->onStop = defaultonStop;
}

static void
defaultonStop(acqMaster_t *master)
{
	// Unused parameter to avoid compiler warning (David Chevrier, Aug 25 2011)
	UNUSED(master);
	printf("Stopped\n");
}

static int
monitorTask(void *arg)
{
	acqMaster_t *master;
	master = arg;
	DEBUGM(master,1) printf("monitorTask(%p)\n", arg);
	acqMonitor_init(master);
	while( 1)
	{
		if( acqMonitor_status(master) == 0)
			break;
		epicsThreadSleep(master->MsecDelay/1000.0);
	}
	DEBUGM(master,1) printf("exit monitorTask()\n");
	return 0;
}

void
startMonitorTask(acqMaster_t *master)
{
	epicsThreadId id;

	id = epicsThreadCreate( "monitor", epicsThreadPriorityMedium, epicsThreadGetStackSize(epicsThreadStackBig), (EPICSTHREADFUNC)monitorTask, master);

}
/*
 *  $Log: acqMonitor.c  $
 *  Revision 1.4.1.5 2010/01/18 16:22:01CST Glen Wright (wrightg) 
 *  Updates for calling 'shutdown'; cleaner property management (3.2.0 and 3.2.1)
 *  Revision 1.4.1.4 2009/12/23 10:37:03CST Glen Wright (wrightg) 
 *  Complete the handler callback cases (required for new text+spectrum handler)
 *  Revision 1.4.1.3 2009/03/04 15:13:47CST Glen Wright (wrightg) 
 *  Revisions for caller-defined output streams
 *  Revision 1.4.1.2 2008/05/28 13:53:13CST Glen Wright (wrightg) 
 *  Initial 3.0 version
 *  Revision 1.4.1.1 2007/03/18 15:03:17CST Glen Wright (wrightg) 
 *  Duplicate revision
 */

