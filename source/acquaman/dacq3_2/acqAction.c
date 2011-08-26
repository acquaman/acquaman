/**
 ** $Header: acqAction.c 1.5.1.4 2009/12/23 10:37:06CST Glen Wright (wrightg) Exp  $
 ** Copyright Canadian Light Source, Inc. All rights reserved.
 **
 ** generate the threads for running the scans and events.
 ** Uses the 'action' structure rather than the trigger and delay structures.
 */
#include "acquisitionLib.internal.h"
#include <math.h>

static int WaitForEvent( epicsEventId e_event, volatile int *shutdown, volatile acqState *state, acqMaster_t *master );
static void runAcqAction( acqScan_t *acq, acqAction_t *aap, Channel *chan, double outval);
static int checkScanConnections(acqScan_t *acq);
static int checkEventConnections(acqEvent_t *ev);
static int test_delay_result( Connector *conp, struct acqAction_waitPV *delay, acqMaster_t *master);
static void acq_put_double( Channel *chan, double val, acqScan_t *acq, short waitflag);
static void acq_putCallback( struct event_handler_args args);
static void eventDataReady( struct event_handler_args args);
static void channelDataReady( struct event_handler_args args);
static void build_event_stream(acqMaster_t *master, acqEvent_t *ev);

/*
 * caller responsibility:
 * epicsEvent fields are initialized.
 * all channel and connector pointers are initialized.
 * all connector callbacks are in place.
 * the sign of the value delta is the same as (final - start)
 * delta is non-zero
 */
void
runAcqScan( acqScan_t *acq)
{
	double val;		/* value of current control PV */
	double delta;		/* delta value actually used */
	double userDelta;	/* records the last user delta value, to monitor for a change */
	double lastout=0;	/* last value output for the control PV */
	acqControl_t *ctlp;	/* pointer to control PV structure */
	int controlCount;	/* index of current control structure for this scan */
	Channel *chan;		/* connected channel for each iteration */
	int inPause = 0;	/* non-zero while this scan is paused */
	int initial_count;	/* loop counter when waiting on initialization */
	int connect_result;	/* results of checking scan connections */
	
	

	VERBOSE(acq->master) printf("Scan %s\n", acq->scanName);
	ca_attach_context( (struct ca_client_context *)acq->master->GlobalContext );
	/*
	 * wait for up to 5 seconds for connections to be good
	 */
	for(initial_count = 0; initial_count < 5; initial_count++)
	{
		epicsThreadSleep(1.0);
		connect_result = checkScanConnections(acq);
		if( connect_result == 2)
			break;
	}
	
	/* run this while waiting for 'Pause' connections */
	while( connect_result == -1 || connect_result == 0)
	{
		epicsThreadSleep(0.1);
		connect_result = checkScanConnections(acq);
	}
		
	/* not connected for a "fail immediately" or "pause with timeout". Done.
	 */
	if( connect_result == -2)
	{
		acq->master->acqStatusMessage( acq->master, "Failed to connect all required PV's\n");
		acq->state = AS_FAULT;
		return;
	}
	
	for(;;)
	{
		acq->state = AS_STANDBY;
		if( WaitForEvent( acq->startEvent, &acq->shutdown, NULL, acq->master) == 0)
		{
			acq->state = AS_OFF;
			/* Note: don't run the 'DONE' queue here, as it should
			 * have already been run when the loop completed.
			 */
			return;
		}

		acq->currentControl = 0;		/* this value is only valid when RUN or PAUSE */
		acq->state = AS_RUN;
		acq->count = 0;
		runAcqAction( acq, acq->actions[ACQ_TG_BEGIN], NULL, 0.0);

		for( controlCount=0; controlCount < acq->numControlPV; controlCount++)
		{
			ctlp = &acq->acqControlList[controlCount];
			acq->currentControl = controlCount;
			if( ctlp->controlChan == NULL)
				continue;
			HANDLERS(acq->master, odh->startpass_cb(key, controlCount) );
			chan = ctlp->controlChan->chan->chan;
			if( ctlp->haveStartVal == 1)
				val = ctlp->startVal;
			else
			{
				if( chan && chan->valid)
					val = val_to_double(chan, 0);
				else
					val = 0;
			}
			/*
			 * record the requested delta value
			 */
			userDelta = delta = ctlp->deltaVal;
			/*
			 * if no delta specified and not forced to fail due to lack of initial value,
			 * estimate a delta to give 10 outputs
			 */
			if( ctlp->deltaVal == 0.0 || ctlp->haveDeltaVal == 0)
			{
				delta = (ctlp->finalVal - val)/10;
			}
			/* if no starting value, the assumption is that the PV is already set to
			 * the value and has already performed an event based on that value.
			 * Avoid using the same value on the next iterative loop.
			 */
			if( ctlp->haveStartVal == 0)
				val += delta;
			ctlp->recordStart = val;
			runAcqAction( acq, acq->actions[ACQ_TG_BEGIN_PASS], NULL, 0.0);
			/*
			 * What to do here? lastout should be initialized, in case
			 * the loop doesn't occur. But when the finalVal is 0, it
			 * will not be output, inconsistent with all other operations.
			 *
			 */
			lastout = 0.0;

			for( ; delta != 0 && (delta>0?(val<=ctlp->finalVal) :(val>=ctlp->finalVal) ) ; val += delta)
			{
				for(;;)
				{
					connect_result = checkScanConnections(acq);
					if( connect_result == -2)
					{
						acq->master->acqStatusMessage( acq->master, "Required PV not connected\n");
						acq->state = AS_FAULT;
						return;
					}
					if(connect_result > 0)
						break;
					/* at this point, either an indefinite pause, or still waiting for a timeout */
					acq->master->acqStatusMessage( acq->master, "Waiting for PV connection\n");
					epicsThreadSleep(0.1);
				}
				
				if( acq->master->globalShutdown )
				{
					runAcqAction( acq, acq->actions[ACQ_TG_DONE], NULL, 0.0);
					acq->state = AS_OFF;
					epicsEventSignal( acq->endEvent);
					return;
				}
				//
				// Why are the pause and resume handlers here?
				// this attempts to synchronize outputs with the pause/resume.
				// the bad part of this is that multiple active scans will give multiple
				// 'pause' and 'resume' elements.
				while( acq->master->globalPause )
				{
					if( inPause == 0)
					{
						inPause = 1;
						runAcqAction( acq, acq->actions[ACQ_TG_PAUSE], NULL, 0.0);
						HANDLERS(acq->master, odh->pause_cb(key) );
					}
					epicsThreadSleep(0.1);
				}
				if( inPause)
				{
					inPause = 0;
					runAcqAction( acq, acq->actions[ACQ_TG_PAUSE_END], NULL, 0.0);
					HANDLERS(acq->master, odh->resume_cb(key) );
				}
				if( ctlp->deltaVal != userDelta)
				{
					/* if the user changes the delta value, and the sign is different, or
					 * the delta is 0, ignore it
					 */
					userDelta = ctlp->deltaVal;
					if( (delta < 0) == (ctlp->deltaVal > 0) || ctlp->deltaVal == 0)
						;
					else
						delta = ctlp->deltaVal;

				}
				lastout = val;
				acq->count++;
				if( acq->needSetControl)
				{
					VERBOSE(acq->master) printf("setting %s -> %g\n", chan->chan_name, val);
					acq_put_double( chan, val, acq, 1);
				}
				runAcqAction( acq, acq->actions[ACQ_TG_MOVE], chan, val);

				runAcqAction( acq, acq->actions[ACQ_TG_DWELL], NULL, val);
			}
			if( fabs(lastout-ctlp->finalVal) > fabs(0.01*delta) )
			{
				while( acq->master->globalPause )
				{
					if( inPause == 0)
					{
						inPause = 1;
						runAcqAction( acq, acq->actions[ACQ_TG_PAUSE], NULL, 0.0);
					}
					epicsThreadSleep(0.5);
				}
				if( inPause)
				{
					inPause = 0;
					runAcqAction( acq, acq->actions[ACQ_TG_PAUSE_END], NULL, 0.0);
				}
				acq->count++;
				if( acq->needSetControl)
				{
					VERBOSE(acq->master) printf("final %s -> %g\n", chan->chan_name, ctlp->finalVal);
					acq_put_double( chan, ctlp->finalVal, acq, 1);
				}
				runAcqAction( acq, acq->actions[ACQ_TG_MOVE], chan, ctlp->finalVal);

				runAcqAction( acq, acq->actions[ACQ_TG_DWELL], NULL, ctlp->finalVal);
			}
			runAcqAction( acq, acq->actions[ACQ_TG_END_PASS], NULL, 0.0);
			HANDLERS(acq->master, odh->endpass_cb(key) );
		}
		runAcqAction( acq, acq->actions[ACQ_TG_DONE], NULL, 0.0);
		epicsEventSignal( acq->endEvent);
	}
}

/*
 * check a scan structure for valid connections.
 * Returns:
 *	2 if all connections good
 *	1 if connections good or 'silent reconnect' connections not made
 *	0 if connections good, 'silent reconnect', and at least 1 'pause until reconnect'
 *	-1 if at least one connection is not made but still waiting for timeout
 *	-2 if no connections are waiting on timeout, and at least 1 connection has timed out
 */
static int
checkScanConnections(acqScan_t *acq)
{
	int count_connected=0, count_ignored=0, count_timingout=0, count_timingpast=0, count_paused=0;
	int controlCount;
	int actionCount;

	/* check each control structure */
	for( controlCount=0; controlCount < acq->numControlPV; controlCount++)
	{
		acqControl_t *ctlp;	/* pointer to control PV structure */
		//Connector *conp;
		Channel *ch;
		
		/* decision time: we check all the connector PV's,
		 * but should it be just the one for the current scan?
		 */
		ctlp=&acq->acqControlList[controlCount];
		if( ctlp->controlChan == NULL)
			continue;
		ch = ctlp->controlChan->chan->chan;
		if( ch == NULL || ch->valid == 0)
		{
			switch(acq->noConnect)
			{
			case NCR_IGNORE:
				count_ignored++;
				break;
			case NCR_RECONNECT:
			case NCR_PAUSE:
				count_paused++;
				break;
				
			case NCR_PAUSE_TIME_60:
			case NCR_PAUSE_TIME_600:
			case NCR_PAUSE_TIME_3600:
				/* need a timestamp to track when the connection failed */
				count_timingout++;
				break;
				
			case NCR_FAIL:
				count_timingpast++;
				break;
			}
			
		}
		else
			count_connected++;
	}

	/* check each action list */
	for( actionCount=0; actionCount < NUM_SCAN_TRIGGERS; actionCount++)
	{
		acqAction_t *actp;
		Connector *conp;
		acqNoConnectResponse_t noConnect;
		Channel *ch;
		
		for(actp = acq->actions[actionCount]; actp; actp=(actp->next==acq->actions[actionCount])?NULL:actp->next)
		{
			/* for any actions that have a connector, check their status */
			switch( actp->type)
			{
			case AA_SET_PV:
				conp = actp->au.spv.connector;
				noConnect = actp->au.spv.noConnect;
				break;
				
			case AA_WAIT_PV:
				conp = actp->au.wpv.connector;
				noConnect = actp->au.wpv.noConnect;
				break;
			default:
				conp = NULL;
				break;
			}
			if( conp == NULL)
				continue;

			ch = conp->chan->chan;
			if( ch == NULL || ch->valid == 0)
			{
				switch(noConnect)
				{
				case NCR_IGNORE:
					count_ignored++;
					break;
				case NCR_RECONNECT:
				case NCR_PAUSE:
					count_paused++;
					break;
					
				case NCR_PAUSE_TIME_60:
				case NCR_PAUSE_TIME_600:
				case NCR_PAUSE_TIME_3600:
					/* need a timestamp to track when the connection failed */
					count_timingout++;
					break;
					
				case NCR_FAIL:
					count_timingpast++;
					break;
				}
			}
			else
				count_connected++;
				
		}
	}

	DEBUGM(acq->master,1) printf("checkScanConnections(%p) good:%d ignored:%d timingout:%d timingpast:%d\n", acq,
		count_connected, count_ignored, count_timingout, count_timingpast);
	
	if( count_timingpast > 0)	/* no matter what else we see, this is 'fatal' */
		return -2;
	if( count_timingout > 0)	/* waiting on something that might time out */
		return -1;
	if( count_paused > 0)		/* at least one PV wants us to pause */
		return 0;
	if( count_ignored > 0)		/* the application can proceed, but not all the PV's may be valid */
		return 1;

	return 2;			/* all PV's are in a connected state */
}

static void
runAcqAction( acqScan_t *acq, acqAction_t *aap, Channel *chan, double outval)
{
	acqAction_t *ca;

	for(ca = aap; ca; ca=(ca->next==aap?NULL:ca->next) )
	{
		DEBUGM(ca->master,1) printf("runAcqAction=%d\n", ca->type);
		switch( ca->type)
		{
		case AA_SET_CONTROL:
			if( chan)
			{
				VERBOSE(acq->master) printf("output %s -> %g\n", chan->chan_name, outval);
				acq_put_double(chan, outval, acq, 1);
			}
			/* chan is NULL if called from other than MOVE list */
				
			break;

		case AA_DELAY_TIME:
			if( ca->au.dt.delay > 0.0)
				epicsThreadSleep( ca->au.dt.delay );
			break;

		case AA_SET_PV:
			if( ca->au.spv.connector && ca->au.spv.connector->chan->chan)
				channel_put_string( ca->au.spv.connector->chan->chan, ca->au.spv.value);
			break;
			
		case AA_WAIT_PV:
			/*
			 * wait for a signal that this PV has a new value.
			 * If the value is aleady set, then don't wait.
			 */
			DEBUGM(ca->master,1) printf(" AA_WAIT_PV endDelay %p\n", ca->au.wpv.endDelay);
			/*
			 * if there is no valid PV, we're unable to test
			 */
			if( ca->au.wpv.connector->chan->chan->valid == 0)
			{
				DEBUGM( ca->master,1) printf("Unable to test invalid PV '%s'\n", ca->au.wpv.name);
				break;
			}

			/* if there is no endDelay epicsEvent, we're unable to wait and compare.
			 */
			if( ca->au.wpv.endDelay == NULL)
			{
				ca->master->acqStatusMessage( ca->master, "Unable to compare PV '%s' to '%s'\n", ca->au.wpv.name, ca->au.wpv.value);
				break;
			}

			ca->au.wpv.waitFlag = 1;
			channel_async_get( ca->au.wpv.connector->chan->chan, channelDataReady, ca);
			ca_flush_io();
			WaitForEvent( ca->au.wpv.endDelay, NULL, NULL, ca->master);
			ca->au.wpv.waitFlag = 0;
			DEBUGM(ca->master,1) printf(" done AA_WAIT_PV\n");
			break;

		case AA_WAIT_MOTOR:
			waitAcqMotor( ca->au.wm.motor);
			break;

		case AA_CALL_EVENT:
			{
				/*
				 * TO DO: if the destination state is AS_RUN, it is currently processing!
				 * Wait for completion, and then trigger again!
				 */
				acqEvent_t *ev;
				ev = ca->au.ce.event;
				if( ev == NULL)
					break;
				if( ev->state != AS_OFF && ev->startEvent)
				{
					epicsEventSignal( ev->startEvent);
					if( ca->au.ce.autoWait != 0)
						WaitForEvent( ev->endEvent, NULL, &ev->state, ev->master);
				}
			}
			break;

		case AA_WAIT_EVENT:
			{
				acqEvent_t *ev;
				ev = ca->au.we.event;
				if( ev)
					WaitForEvent( ev->endEvent, NULL, &ev->state, ev->master);
			}
			break;

		case AA_CALL_SCAN:
			{
				/*
				 * TO DO: if the destination state is AS_RUN, it is currently processing!
				 * Wait for completion, and then trigger again!
				 */
				acqScan_t *acq;
				acq = ca->au.cs.scan;
				if( acq->state != AS_OFF && acq->startEvent)
					epicsEventSignal( acq->startEvent);
			}

			if(ca->au.cs.autoWait == 0)
				break;

			/* fall through */
		case AA_WAIT_SCAN:
			{
				acqScan_t *sc;
				sc = ca->au.ws.scan;
				if( sc)
					WaitForEvent( sc->endEvent, &sc->shutdown, &sc->state, sc->master);
			}
			break;
		case AA_NEXT_OUTPUT:
			HANDLERS(acq->master, odh->nextOutput_cb(key) );
			break;

			case AA_NO_ACTION:
			break;
			

		}
	}
}

#warning "Commented out because unused. Function was previously commented out, now try the declaration too"
//static int countColumns( acqEvent_t *ev);
/*
 * run an event request. Note: Each event runs in its own thread. When receiving a start
 * signal, it outputs the values of the requested PV's.
 * when the PV mode is 'immediate', the PV is assumed to be up-to-date due to monitoring. 
 * when the PV mode is 'readback', a pvget is performed to return the data.
 * when the PV mode is 'wait', the output is delayed until a readback occurs.
 * This will have to be expanded to support array PV's that send a
 * fill request and may return multiple buffers.
 *
 * Calling routine responsibilities:
 *	initialize startSignal and mutex;
 */


void
runEvent( acqEvent_t *ev)
{
	int i;//, nchar;
	int Done;
	//char result[50];
	//char timebuf[50];
	//char *separator;
	//epicsTimeStamp startTime, curTime, prevTime;
	epicsTimeStamp startTime, prevTime;
	//double relTime;
	int initial_count, connect_result;
	eventDataHandler_t *odh;	/* Output Data Handler */

	VERBOSE(ev->master) printf("runEvent %s\n", ev->eventName);

	ca_attach_context( (struct ca_client_context *)ev->master->GlobalContext);

	/* wait for up to 5 seconds for connections to be good */
	for( initial_count = 0; initial_count < 5; initial_count++)
	{
		epicsThreadSleep(1.0);
		connect_result = checkEventConnections(ev);
		if( connect_result == 2)
			break;
	}
	
	while( connect_result == -1 || connect_result == 0)
	{
		epicsThreadSleep(0.1);
		connect_result = checkEventConnections(ev);
	}
	
	/* not connect for a "fail immediately" or "pause with timeout." Done
	 */
	if( connect_result == -2)
	{
		ev->master->acqStatusMessage(ev->master, "Event failed to connect all required PV's\n");
		ev->state = AS_FAULT;
		return;
	}
	
	build_event_stream(ev->master, ev);
	

	epicsTimeGetCurrent( &startTime);
	prevTime = startTime;

	for(;;)
	{
		ev->state = AS_STANDBY;
		if( WaitForEvent( ev->startEvent, &ev->shutdown, NULL, ev->master ) == 0)
		{
			ev->state = AS_OFF;
			return;
		}

		ev->state = AS_RUNUP;

		/* initialize the PV reading:
		 *	useCurrent means use whatever is already here;
		 *	waitForMonitor means waiting for the next monitor call on this PV
		 *	usePvGet means requesting a read from a PV
		 */
		Done = 1;
		for(i=0; i < ev->numPvList; i++)
		{
			if( ev->pvList[i].pvName == NULL || ev->pvList[i].pvName[0] == 0 )
			{
				ev->pvList[i].waitState = 0;
				continue;
			}
			if( ev->pvList[i].ready == useCurrent)
				continue;
			if( ev->pvList[i].conEvent == NULL || ev->pvList[i].conEvent->chan == NULL || ev->pvList[i].conEvent->chan->chan->valid == 0)
			{
				ev->pvList[i].waitState = 0;
				continue;
			}
				
			if( ev->pvList[i].ready == waitForMonitor)
			{
				ev->pvList[i].waitState = 1;
				ev->pvList[i].haveMonitor = 0;
				Done = 0;
			}
			if( ev->pvList[i].ready == usePvGet)
			{
				Channel *chan;
				ev->pvList[i].waitState = 1;
				chan = ev->pvList[i].conEvent->chan->chan;
				channel_async_get( chan, eventDataReady, &ev->pvList[i] );
				Done = 0;
			}
		}
		ca_flush_io();		/* this should be a NO-OP if no I/O to flush ... */

		while( Done == 0)
		{
			if( ev->master->globalShutdown )
			{
				ev->state = AS_OFF;
				return;
			}
			epicsEventWaitWithTimeout( ev->monitorPvEvent, 1.0);
			Done = 1;
			for( i=0; i < ev->numPvList; i++)
			{
				if( ev->pvList[i].ready == useCurrent)
					continue;
				if( ev->pvList[i].waitState == 1)
					Done = 0;
			}
		}

		ev->state = AS_RUN;

		/* To Do: mutex lock the output handlers */
		HANDLERS(ev->master, odh->startRecord_cb( key, ev->eventNum));

		for(i=0; i < ev->numPvList; i++)
		{
			Channel *chan;
			acqPv_t *pvp;
			pvp = &ev->pvList[i];
			if( pvp->noRecord)
				continue;
			if( pvp->conEvent == NULL)
				continue;
			chan = pvp->conEvent->chan->chan;

			if( pvp->numDisplayRange == 0)
			{
				if( chan->valid)
				{
					HANDLERS(ev->master, odh->pvValue_cb( key, ev->eventNum, i, chan->dataval, chan->maxelement) );
				} else
				{
					HANDLERS(ev->master, odh->pvValue_cb( key, ev->eventNum, i, NULL, 1) );
				}
			}
			else
			{
				int dr_idx;
//				int ch_idx;
				void *base = chan->dataval;
				unsigned int size = dbr_value_size[chan->chan_type];
				void * offset;
				unsigned int count;
				for( dr_idx=0; dr_idx < pvp->numDisplayRange; dr_idx++)
				{
					offset = base + size*pvp->displayRange[dr_idx].firstIdx;
					switch (pvp->displayRange[dr_idx].displayType)
					{
					case singleVal:
						HANDLERS( ev->master, odh->pvValue_cb(key, ev->eventNum, i, offset, 1) );
						break;
					case closedRange:
						count = pvp->displayRange[dr_idx].lastIdx-pvp->displayRange[dr_idx].firstIdx + 1;;
						HANDLERS( ev->master, odh->pvValue_cb(key, ev->eventNum, i, offset, count) );
						break;
					case openRange:
						count = chan->maxelement-pvp->displayRange[dr_idx].firstIdx;
						HANDLERS( ev->master, odh->pvValue_cb(key, ev->eventNum, i, offset, count) );
						break;
					}
				}
			}

			pvp->haveMonitor = 0;
		}
		HANDLERS( ev->master, odh->endRecord_cb(key, ev->eventNum) );
		ev->outputCount++;

		epicsEventSignal( ev->endEvent);
	}
}

static int
checkEventConnections(acqEvent_t *ev)
{
	int count_connected=0, count_ignored=0, count_timingout=0, count_timingpast=0, count_paused=0;
	int pvCount;
	
	for(pvCount=0; pvCount < ev->numPvList; pvCount++)
	{
		Connector * conp = ev->pvList[pvCount].conEvent;
		/*
		 * check that there is a link, it has a channel, and the channel is valid
		 */
		if( conp != NULL)
		{
			if( conp->chan && conp->chan->chan && conp->chan->chan->valid)
			{
				count_connected++;
				continue;
			}
			
		}
		/* not connected */
		switch( ev->pvList[pvCount].noConnect)
		{
		case NCR_IGNORE:
			count_ignored++;
			break;
		case NCR_RECONNECT:
		case NCR_PAUSE:
			count_paused++;
			break;
					
		case NCR_PAUSE_TIME_60:
		case NCR_PAUSE_TIME_600:
		case NCR_PAUSE_TIME_3600:
		/* need a timestamp to track when the connection failed */
			count_timingout++;
			break;
					
		case NCR_FAIL:
			count_timingpast++;
			break;
		}
	}
	
	DEBUGM(ev->master,1) printf("checkEventConnections(%p) good:%d ignored:%d timingout:%d timingpast:%d\n", ev,
		count_connected, count_ignored, count_timingout, count_timingpast);
	
	if( count_timingpast > 0)	/* no matter what else we see, this is 'fatal' */
		return -2;
	if( count_timingout > 0)	/* waiting on something that might time out */
		return -1;
	if( count_paused > 0)		/* at least one PV wants us to pause */
		return 0;
	if( count_ignored > 0)		/* the application can proceed, but not all the PV's may be valid */
		return 1;

	return 2;			/* all PV's are in a connected state */
}	

//static int
//countColumns( acqEvent_t *ev)
//{
//	int colnum, i;

//	colnum = 1;

//	if( ev->putEventId)
//	{
//		colnum++;
//	}
//	if( ev->putAbsTime)
//	{
//		colnum++;
//	}
//	if( ev->putRel0Time)
//	{
//		colnum++;
//	}
//	if( ev->putRelPTime)
//	{
//		colnum++;
//	}
//	for(i=0; i < ev->numPvList; i++)
//	{
//		Channel *ch;
//		acqPv_t *pvp;
//		displayPvRange_t *drp;
//		int dr_idx;

//		pvp = &ev->pvList[i];
//		if(pvp->pvName == NULL || pvp->conEvent == NULL)
//			continue;
//		ch = pvp->conEvent->chan->chan;
//		if( pvp->noRecord)
//		{
//			continue;
//		}

//		/* need to increment by the number of columns that
//		 * will be printed by an array entry!
//		 */
		
//		if( pvp->numDisplayRange == 0)
//		{
//			colnum++;
//			continue;
//		}
//		for(dr_idx = 0; dr_idx < pvp->numDisplayRange; dr_idx++)
//		{
//			drp = & pvp->displayRange[dr_idx];
//			switch( drp->displayType)
//			{
//			case singleVal:
//				colnum++;
//				break;
//			case closedRange:
//				colnum += 1+drp->lastIdx-drp->firstIdx;
//				break;
//			case openRange:
//				/* Danger: it is quite possible that the channel is not open
//				 * at this point, and that the maxelement is not correct.
//				 */
//				if( (unsigned)ch->maxelement > drp->firstIdx)
//					colnum += 1 + ch->maxelement - drp->firstIdx;
//				break;
//			}
//		}
//	}

//	return colnum;
//}
	
/*
 * wait for an epics event to signal. Check every .5 seconds. If the
 * event is invalid, returns right away as if a signal had occured.
 */
static int
WaitForEvent( epicsEventId e_event, volatile int *shutdown, volatile acqState *state, acqMaster_t *master)
{
	epicsEventWaitStatus status;
	acqState startState;
	int countdown = 0;
	
	if( e_event == NULL)
		return 1;
	if( state)
		startState = *state;

	for(;;)
	{
		status = epicsEventWaitWithTimeout(e_event, 0.25);
		if( master->globalShutdown || (shutdown && *shutdown) )
			return 0;
		if( status == epicsEventWaitTimeout)
		{
			/* if the caller has requested the state be monitored:
			 * if the entrance state was run, then the event may have
			 * been missed. Wait 1 more loop in case of race condition
			 * between the timeout and the event being queued.
			 * if the entrance state was standby, then wait for a run
			 * state. If it doesn't occur, (magic numbers: 0.25 in delay, 8 in countdown)
			 * then assume something went wrong.
			 * At this point, should it be a fatal error or damn the torpedoes?
			 */
			if( state)
			{
				if( countdown)
				{
					if( *state != AS_RUN)
					{
						if( --countdown == 0)
							return 1;
					}
					else
						countdown = 0;
					continue;
				}
				if( *state != AS_RUN && startState == AS_RUN)
					countdown = 1;
				if( *state == AS_STANDBY && startState == AS_STANDBY)
					countdown = 8;
			}
			continue;
		}
		if( status == epicsEventWaitError)
			master->acqStatusMessage(master, "WaitForEvent(%p,%p) - bad event code!\n", e_event, shutdown);
		return 1;	/* either got a signal or an error! */
	}
}

/*
 * callback after a monitor is detected
 */
void
callback_PVmonitor( Connector *conp)
{
	Channel *chan;
	acqPv_t *ap;

	chan = conp->chan->chan;

	/*
	 * check for valid acquisition event
	 */
	if( chan->valid == 0)		/* not valid, nothing to do! SHOULD NOT HAPPEN! */
		return;

	if( conp->user_data)
	{
		ap = (acqPv_t *)(conp->user_data);
		if( ap->event == NULL || ap->event->state == AS_OFF)
			return;
		if(ap->event->state > AS_OFF && ap->haveMonitor == 0 )
		{
			DEBUGM(ap->event->master,1) printf( "callback_PVmonitor: %s \n", chan->chan_name);
			ap->haveMonitor = 1;
			ap->waitState = 0;
			epicsEventSignal( ap->event->monitorPvEvent);
		}
	}
}

/*
 * callback because the description field has been set or is changed
 */
void
callback_PVdescription(Connector *conp)
{
	Channel *chan;
	acqPv_t *ap;

	chan = conp->chan->chan;
	ap = (acqPv_t *)conp->user_data;
	if( ap == NULL)
		return;

	/*
	 * check for valid acquisition event
	 */
	if( ap->event == NULL)
		return;
	if( chan->valid == 0)		/* not valid, nothing to do! */
		return;

	val_to_string( chan, ap->description, sizeof ap->description, 0);
	/* only communicate the description when running? */
	if( ap->event->master->globalState == AS_RUN)
	{
		int pvno;
		pvno = ap - ap->event->pvList;
		HANDLERS( ap->event->master, odh->pvDescription_cb(key, ap->event->eventNum, pvno, ap->description) );
	}
}
/*
 * callback when waiting for a PV value
 */
void
callback_check_delay( Connector *conp)
{
	acqAction_t *delay;

	delay = (acqAction_t *) conp->user_data;
	if( delay == NULL)		/* dangerous, because a thread is waiting for this */
		return;
	if( delay->au.wpv.waitFlag == 0)	/* don't care about checking the value */
		return;
	if( test_delay_result( conp, &(delay->au.wpv), delay->master ) != 0)
		epicsEventSignal( delay->au.wpv.endDelay);
}

/*
 * callback when the state of an event PV changes.
 */
void
callback_eventPVstate(Connector *conp)
{
	Channel *chan;
	acqPv_t *ap;
	acqEvent_t *ev;
//	acqMaster_t *master;
	int pvID;

	chan = conp->chan->chan;

	if( conp->user_data == NULL)	/* if user_data not set, there is no way to inform registered handlers of changes */
		return;
	
	ap = (acqPv_t *)(conp->user_data);
	if( ap->event == NULL)
		return;
	// if not yet running, then don't update
	if( ap->event->master->globalState == AS_RUN)
	{
		ev = ap->event;
		pvID = ap - ap->event->pvList;
		HANDLERS(ev->master, odh->pvState_cb(key, ev->eventNum, pvID, chan->severity, chan->valid) );

		if(chan->valid)		/* be sure to update the data type */
			HANDLERS(ev->master, odh->pvDataType_cb(key, ev->eventNum, pvID, chan->chan_type, chan->maxelement) );
	}
	
}

/*
 * called when checking to see if a PV delay condition is true.
 */
static int
test_delay_result( Connector *conp, struct acqAction_waitPV *delay, acqMaster_t *master)
{
	Channel *chan;
	char result[100];
	char *s;
	double dval;
	double val;
	double cval;	/* comparison precision */
	int precision;
//	int debugflag;

	DEBUGM(master,1) printf("test_delay_result(%p, %p)\n", conp, delay);
	chan = conp->chan->chan;
	if( delay->value == NULL)	/* always assume true */
		return 1;
	if( chan->valid == 0)		/* not valid, we won't wait! */
		return 1;

	switch( chan->chan_type)
	{
	case DBF_STRING:
	case DBF_ENUM:
		/* perform a string comparison */
		val_to_string( chan, result, sizeof result, 0);
		DEBUGM(master,1) printf(" string compare '%s' '%s'\n", result, delay->value);
		if( strcmp( result, delay->value) == 0)
			return 1;
		return 0;
	default:
		/* perform a numeric comparison */
		dval = strtod( delay->value, 0);
		val = val_to_double(chan, 0);
		DEBUGM(master,1) printf("  numeric compare '%s' '%g'\n", delay->value, val);
		if( val == dval)
			return 1;
		/* strange job: check to see if the stored value was
		 * "imprecise", and determine the amount of precision needed
		 * to get a good comparision.
		 * measure to E^'precision'/2
		 */
		precision = 0;
		s = strchr( delay->value, 'e');
		if( !s)
			s = strchr( delay->value, 'E');
		if( s)
			precision = atoi(s+1);
		s = strchr( delay->value, '.');
		if( s)
		{
			s++;
			while( *s && strchr( "0123456789", *s) )
			{
				precision--;
				s++;
			}
		}
		cval = 1;
		while (precision > 0)
		{
			cval = cval*10;
			precision--;
		}
		while( precision < 0)
		{
			cval = cval/10;
			precision++;
		}
		cval /= 2;
		if( fabs(val-dval) <= cval)
			return 1;
		return 0;
	}
}

/*
 * set the comment field of the output handlers to the supplied value
 */
void acq_setComment(const acqMaster_t *master, const char *comment)
{
	HANDLERS(master, odh->comment_cb(key, comment) );
}

/*
 * support creating, deleting, and inserting action elements
 */

acqAction_t *
new_acqAction( acqAction_t **head, acqActionType_t type, acqAction_t *target, acqMaster_t *master)
{
	acqAction_t *ap;

	ap = calloc( 1, sizeof *ap);
	if( ap == NULL)
		return NULL;

	ap->master = master;
	ap->type = type;
	/*
	 * insert into linked list BEFORE 'target'.
	 */
	if( *head == NULL)
	{
		*head = ap->next = ap->prev = ap;
		return ap;
	}
	if( target == NULL)
		target = *head;
	ap->next = target;
	ap->prev = target->prev;
	target->prev->next = ap;
	target->prev = ap;
	return ap;
}

acqAction_t *
new_acqAction_delayTime( acqAction_t **head, double delay, acqMaster_t *master)
{
	acqAction_t *ap;
	ap = new_acqAction( head, AA_DELAY_TIME, NULL, master);
	ap->au.dt.delay = delay;
	return ap;
}

acqAction_t *
new_acqAction_setPV( acqAction_t **head, char *pvname, char *value, acqMaster_t *master)
{
	acqAction_t *ap;
	ap = new_acqAction( head, AA_SET_PV, NULL, master);
	if( pvname)
		ap->au.spv.name = strdup(pvname);
	if( value)
		ap->au.spv.value = strdup(value);
	return ap;
}

acqAction_t *
new_acqAction_waitPV( acqAction_t **head, char *pvname, char *value, acqMaster_t *master)
{
	acqAction_t *ap;
	ap = new_acqAction( head, AA_WAIT_PV, NULL, master);
	if( pvname)
		ap->au.wpv.name = strdup(pvname);
	if( value)
		ap->au.wpv.value = strdup(value);
	return ap;
}

acqAction_t *
new_acqAction_waitMotor( acqAction_t **head, char *motorName, acqMaster_t *master)
{
	acqAction_t *ap;
	ap = new_acqAction( head, AA_WAIT_MOTOR, NULL, master);
	if( motorName)
		ap->au.wm.name = strdup(motorName);
	return ap;
}

acqAction_t *
new_acqAction_callEvent( acqAction_t **head, char *eventName, int autowait, acqMaster_t *master)
{
	acqAction_t *ap;
	ap = new_acqAction( head, AA_CALL_EVENT, NULL, master);
	if( eventName)
		ap->au.ce.name = strdup(eventName);
	ap->au.ce.autoWait = autowait;
	return ap;
}

acqAction_t *
new_acqAction_waitEvent( acqAction_t **head, char *eventName, acqMaster_t *master)
{
	acqAction_t *ap;
	ap = new_acqAction( head, AA_WAIT_EVENT, NULL, master);
	if( eventName)
		ap->au.we.name = strdup(eventName);
	return ap;
}

acqAction_t *
new_acqAction_callScan( acqAction_t **head, char *scanName, int autowait, acqMaster_t *master)
{
	acqAction_t *ap;
	ap = new_acqAction( head, AA_CALL_SCAN, NULL, master);
	if( scanName)
		ap->au.cs.name = strdup(scanName);
	ap->au.cs.autoWait = autowait;
	return ap;
}

acqAction_t *
new_acqAction_waitScan( acqAction_t **head, char *scanName, acqMaster_t *master)
{
	acqAction_t *ap;
	ap = new_acqAction( head, AA_WAIT_SCAN, NULL, master);
	if( scanName)
		ap->au.ws.name = strdup(scanName);
	return ap;
}

acqAction_t *
new_acqAction_setControl( acqAction_t **head, acqMaster_t *master)
{
	acqAction_t *ap;
	ap = new_acqAction( head, AA_SET_CONTROL, NULL, master);
	return ap;
}

acqAction_t *
new_acqAction_nextOutput( acqAction_t **head, acqMaster_t *master)
{
	return new_acqAction( head, AA_NEXT_OUTPUT, NULL, master);
}

/*
 * free a string if it exists, and then zero the pointer
 */
static void
freestring(char **ptr)
{
	if( *ptr)
		free(*ptr);
	*ptr = NULL;
}

acqAction_t *
destroy_acqAction( acqAction_t **head, acqAction_t *ap)
{
	if( ap == NULL)
		return *head;
	
	if( ap->next == ap)
		*head = NULL;
	else
	{
		ap->next->prev = ap->prev;
		ap->prev->next = ap->next;
		if( *head == ap)
			*head = ap->next;
	}

	/*
	 * FREE UP ALL ALLOCATED DATA!
	 */
	switch( ap->type)
	{
	case AA_DELAY_TIME:
		break;
	case AA_SET_PV:
		freestring(&ap->au.spv.name);
		freestring(&ap->au.spv.value);
		break;
	case AA_WAIT_PV:
		freestring(&ap->au.wpv.name);
		freestring(&ap->au.wpv.value);
		break;
	case AA_WAIT_MOTOR:
		freestring(&ap->au.wm.name);
		break;
	case AA_CALL_EVENT:
		freestring(&ap->au.ce.name);
		break;
	case AA_CALL_SCAN:
		freestring(&ap->au.cs.name);
		break;
	case AA_WAIT_EVENT:
		freestring(&ap->au.we.name);
		break;
	case AA_WAIT_SCAN:
		freestring(&ap->au.ws.name);
		break;
	case AA_SET_CONTROL:
		break;
	case AA_NEXT_OUTPUT:
		break;
	case AA_NO_ACTION:
		break;
	}

	free( ap);
	return *head;
}

static char * actionNames[] = { "Delay", "SetPV", "WaitPV", "WaitMotor", "CallEvent", "CallScan", "WaitEvent", "WaitScan", "SetControl", "NextOutput" };

char *
getActionName( acqActionType_t aat)
{
//	if( aat >= 0 && aat < sizeof actionNames/sizeof actionNames[0] )
	if( aat < sizeof actionNames/sizeof actionNames[0] )
		return actionNames[aat];
	return "Unknown";
}

acqActionType_t
getActionType( const char *name)
{
	unsigned int i;
	for(i=0; i < sizeof actionNames/sizeof actionNames[0]; i++)
		if( strcasecmp( name, actionNames[i]) == 0)
			return i;
	return -1;
}

static char * PvReadyNames[] = { "Immediate", "Read-back", "Wait", "Unknown" };
char *
getPvReadyName( acqPvReady_t pva)
{
//	if( pva >= 0 && pva <=2 )
	if( pva <=2 )
		return PvReadyNames[pva];
	return PvReadyNames[3];

}

acqPvReady_t
getPvReadyType( char *name)
{
	unsigned int i;
	for(i=0; i < sizeof PvReadyNames/sizeof PvReadyNames[0]; i++)
		if( strcasecmp( name, PvReadyNames[i]) == 0)
			return i;
	return -1;
}

/* RU! where should these routines reside? */

/*
 * Use "waitflag = 0" if you want to bypass callback mode, e.g. a PV that
 * has odd or missing response properties
 * "acq" is used as a user-defined callback value, e.g. to ID the action
 */
static void
acq_put_double( Channel *chan, double val, acqScan_t *acq, short waitflag)
{
	if( acq == NULL)
	{
		/*
		 * note: not a verbose or debug situation, and there is
		 * no 'master' information to find a print command.
		 */
		acq->master->messageAdd( acq->master, "acq_put_double(): missing a scan definition.  Not using callback.\n");
		waitflag = 0;
		return;
	}

	if( chan->valid == 0)
		return;

	if( waitflag )
	{
		channel_put_double_callback( chan, val, acq_putCallback, acq);
		epicsEventWait( acq->putNotify);
	}
	else
	{
		channel_put_double( chan, val);
	}
}

/*
 * callback when data ready. Note that this assumes that a scan will only have
 * one PV per scan task with an outstanding 'put'. This is the way acq_put_double is
 * designed, and changes there have a huge impact on this routine.
 *
 * When setting up for callbacks using this function, set the "user data"
 * to the "acqScan_t" var associated with the callback.
 */
static void
acq_putCallback( struct event_handler_args args)
{

	acqScan_t *acq = (acqScan_t *)args.usr;


	if( acq == NULL )
	{
		/* this should never be reached if the 'put_double' is set up properly
		 */
		printf( "acq_putCallback(): someone forgot to provide a signal ID to end a wait\n");
		return;
	}

	DEBUGM(acq->master,1) printf("acq_putCallback\n");
	if( args.status != ECA_NORMAL)
	{
		/* put error flagging here */
		acq->master->messageAdd(acq->master, "acq_putCallback - error %d!\n", args.status);
		return;
	}
	
		epicsEventSignal( acq->putNotify);
}

/*
 * called when data has arrived for a 'pvget' on a pv.
 * The arg should be a pointer to the acqPv_t being read.
 */
static void
eventDataReady( struct event_handler_args args)
{
	if( args.usr && args.status == ECA_NORMAL)
	{
		acqPv_t *apv;
		Channel *chan;
		apv = ( acqPv_t *) args.usr;
		DEBUGM(apv->event->master,1) printf("eventDataReady %d\n", args.status);
		chan = apv->conEvent->chan->chan;
		apv->waitState = 0;
		chan->nbytes = dbr_value_size[args.type] * args.count;
		memcpy( chan->dataval, dbr_value_offset[args.type]+(char *)args.dbr, chan->nbytes);
		epicsEventSignal(apv->event->monitorPvEvent);
	}
	else
		printf("eventDataReady bad call usr:%p status:%u\n", args.usr, args.status);
}

/*
 * called when data has arrived for a PV to be tested. Note that the pointer is to the current action structure.
 */
static void
channelDataReady( struct event_handler_args args)
{
	acqAction_t *ap;
	struct acqAction_waitPV *wpv;
	Channel *chan;

	if( args.usr && args.status == ECA_NORMAL)
	{
		ap = (acqAction_t *)args.usr;

		DEBUGM(ap->master,1) printf("channelDataReady u:%p st:%d\n", args.usr, args.status);
		if( ap->type != AA_WAIT_PV)
			return;
		wpv = &ap->au.wpv;
		chan = wpv->connector->chan->chan;
		chan->nbytes = dbr_value_size[args.type] * args.count;
		memcpy( chan->dataval, dbr_value_offset[args.type]+(char *)args.dbr, chan->nbytes);
		callback_check_delay(wpv->connector);
	}
	else
		printf("channelDataReady bad call usr:%p status:%u\n", args.usr, args.status);
}

static void
build_event_stream(acqMaster_t *master,acqEvent_t *ev)
{
	char eventFlags[100];
	char pvFlags[100];
	int idx;
	HANDLERS(master, odh->eventName_cb(key, ev->eventNum, ev->eventName) );

	eventFlags[0] = '\0';
	if( ev->putEventId)
		strncat(eventFlags, "putEventId ", sizeof eventFlags);
	if( ev->putAbsTime)
		strncat(eventFlags, "absTime ", sizeof eventFlags);
	if( ev->putRel0Time)
		strncat(eventFlags, "rel0Time ", sizeof eventFlags);
	if( ev->putRelPTime)
		strncat(eventFlags, "relTime ", sizeof eventFlags);
	if( ev->putCommentPrefix)
		strncat(eventFlags, "commentPrefix ", sizeof eventFlags);
	HANDLERS(master, odh->eventFlags_cb(key, ev->eventNum, eventFlags) );
	for(idx=0; idx < ev->numPvList; idx++)
	{
		int flagLen;
		acqPv_t *pvp;
		Connector *conp;

		pvp = &ev->pvList[idx];
		if( pvp->pvName == NULL || pvp->pvName[0] == 0 || pvp->noRecord)
			continue;
		conp = pvp->conEvent;
		HANDLERS(master, odh->pvName_cb(key, ev->eventNum, idx, conp->chan->chan->chan_name) );
		if( pvp->description[0])
		{
			HANDLERS(master, odh->pvDescription_cb( key, ev->eventNum, idx, pvp->description) );
		}
		else
		{
			HANDLERS(master, odh->pvDescription_cb( key, ev->eventNum, idx, pvp->pvName) );
		}
		pvFlags[0] = '\0';
		if( pvp->outputFormat)
		{
			flagLen = strlen(pvFlags);
			snprintf(&pvFlags[flagLen], (sizeof pvFlags)-flagLen, "format \"%s\" ", pvp->outputFormat);
		}
		if( pvp->isSpectrum)
			strncat( pvFlags, "isSpectrum 1 ", sizeof pvFlags);

		flagLen = strlen(pvFlags);
		snprintf(&pvFlags[flagLen], (sizeof pvFlags)-flagLen, "ready %d", pvp->ready);
		HANDLERS(master, odh->pvFlags_cb(key, ev->eventNum, idx, pvFlags) );

		HANDLERS(master, odh->pvState_cb(key, ev->eventNum, idx, conp->chan->chan->severity, conp->chan->chan->valid) );

		if( CONNECT_VALID( conp) == 0)	/* this connector may never know it's data type ... */
		{
			HANDLERS(ev->master, odh->pvDataType_cb(key, ev->eventNum, idx, DBF_NO_ACCESS, 0) );
			continue;
		}
		HANDLERS(ev->master, odh->pvDataType_cb(key, ev->eventNum, idx, conp->chan->chan->chan_type, conp->chan->chan->maxelement) );
	}
}
