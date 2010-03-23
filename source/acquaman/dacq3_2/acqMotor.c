/**
 ** $Header: acqMotor.c 1.3.1.3 2009/03/04 15:13:47CST Glen Wright (wrightg) Exp  $
 ** Copyright Canadian Light Source, Inc. All rights reserved.
 **
 ** Description:
 **    support a wait for a group of motors
 **/

#include "acquisitionLib.internal.h"

/**
 ** support a wait for a group of motors
 **/


static int
getInt( Connector *conp)
{
	if( conp == NULL || conp->chan->chan == NULL)
		return 0;
	return val_to_int( conp->chan->chan, 0);
}

/*
 * wait for a motor movement to complete. 500 msec with no moving indicator
 * says that movement is done.
 * a return of '1' indicates movement was completed; 0 indicates waiting was aborted.
 */
int
waitAcqMotor( acqMotor_t *m)
{
	int i;
	acqOneMotor_t *op;
	int moving, busy;
	int status;

	if( m == NULL || m->list == NULL)
		return 0;

	DEBUGM(m->master,1) printf("waitAcqMotor (%p) %s %p\n", m, m->name, m->list);
	/*
	 * starting assumption is that the motors are moving. 
	 * the busy indicator says definitely moving if != 0.
	 */
	for( op = m->list; op ; op=next_acqOneMotor(op) )
	{
		if( CONNECT_VALID( op->busy) == 0 || CONNECT_VALID(op->done) == 0)
		{
			/* no connection! */
			op->moving = 0;
			continue;
		}
		busy = getInt( op->busy);
		DEBUGM(m->master,1) printf(" ... op:%s\n", op->name);
		if( busy)
			op->moving = 2;
		else
			op->moving = 1;
	}
	m->waiting = 1;
	for(;;)
	{
		/*
		 * note: this will return immediately upon spotting the 'done'
		 * flag going high. The flag is ignored on entry to the routine
		 * in case the 'done' indicator is from a previous movement.
		 * Worst case: motor movement goes out, done indicator gets read back
		 * and ignored, waiting flag gets set, and we sit through two timeouts
		 * noting the 'busy' never goes high.
		 */
		DEBUGM(m->master,1) printf("motorGroupWait()\n");
		status = epicsEventWaitWithTimeout( m->waitEvent, 0.7 );
		if( m->master->globalShutdown )
			return 0;
		if( status == epicsEventWaitTimeout )
		{
			for(op=m->list; op ; op=next_acqOneMotor(op) )
			{
				DEBUGM(m->master,1) printf("  %s moving=%d\n", op->name, op->moving);
				if( op->moving == 1)
					op->moving = 0;
			}
		}
		if( status == epicsEventWaitError )
		{
			m->master->messageAdd(m->master, "MotorWait - bad event code!\n");
			return 0;
		}
		/*
		 * event received. check our status
		 */
		moving = 0;
		for( op=m->list; op ; op=next_acqOneMotor(op) )
			if( op->moving)
				moving++;

		if( moving == 0)
			break;

	}
		
	m->waiting = 0;
	return 1;
}

/*
 * busy has changed: if => 1, then set moving flag. if => 0, state is unknown.
 */
static void
busyCallback( Connector *conp)
{
	acqOneMotor_t *op;

	op = (acqOneMotor_t *) conp->user_data;
	if( op->parent->waiting == 0)
		return ;

	if( getInt( conp) != 0)
		op->moving = 2;
	else
		op->moving = 0;

	return ;
}

/*
 * done has changed: if => 1, then clear moving flag.
 * => 0 has no meaning.
 */
static void
doneCallback( Connector *conp)
{
	acqOneMotor_t *op;

	op = (acqOneMotor_t *) conp->user_data;
	if( op->parent->waiting == 0)
		return;

	if( getInt( conp) != 0)
	{
		op->moving = 0;
		epicsEventSignal( op->parent->waitEvent);
	}
}

acqMotor_t *
first_acqMotor( const acqMaster_t *master)
{
	return master->acqMotorList;
}

acqMotor_t *
next_acqMotor(const  acqMotor_t *m)
{
	return ((!m||m->next==m->master->acqMotorList)?NULL:m->next);
}

acqMotor_t *
lookup_acqMotor( const char * name, const acqMaster_t *master)
{
	acqMotor_t *m;

	for( m = master->acqMotorList; m ; m = (m->next==master->acqMotorList?NULL:m->next) )
	{
		if( strcmp(name, m->name) == 0)
			return m;
	}
	return NULL;
}

acqMotor_t *
new_acqMotor(const char *name, acqMaster_t *master)
{
	acqMotor_t *m;
	m = calloc( 1, sizeof *m);
	if( name)
		m->name = strdup(name);
	m->master = master;
	if( master->acqMotorList == NULL)
		master->acqMotorList = m->next = m->prev = m;
	else
	{
		m->prev = master->acqMotorList->prev;
		m->next = master->acqMotorList;
		m->prev->next = m;
		m->next->prev = m;
	}
	return m;
}

acqOneMotor_t *
new_acqOneMotor( acqMotor_t *m, const char *name)
{
	acqOneMotor_t *om;

	om = calloc( 1, sizeof *om);
	om->name = strdup(name);
	om->parent = m;

	if( m->list == NULL)
		m->list = om->next = om->prev = om;
	else
	{
		om->next = m->list;
		om->prev = m->list->prev;
		om->prev->next = om;
		om->next->prev = om;
	}
	return om;
}

void
destroy_acqOneMotor( acqOneMotor_t *om)
{
	acqMotor_t *m; 
	m = om->parent;
	om->prev->next = om->next;
	om->next->prev = om->prev;
	if( m->list == om)
	{
		if( om->next == om)
			m->list = NULL;
		else
			m->list = om->next;
	}
	free( om->name);
	free( om);
}

acqOneMotor_t *
next_acqOneMotor( acqOneMotor_t *om)
{
	return ( (om==NULL||om->next==om->parent->list)?NULL:om->next );
}

void
destroy_acqMotor( acqMotor_t *m, acqMaster_t *master)
{
	while( m->list)
		destroy_acqOneMotor( m->list->prev);

	m->prev->next = m->next;
	m->next->prev = m->prev;
	if( master->acqMotorList == m)
	{
		if( m->next == m)
			master->acqMotorList = NULL;
		else
			master->acqMotorList = m->next;
	}

	free( m->name);
	free( m);
}

/*
 * initialize elements of the motor record for running.
 */
int
setup_acqMotor( acqMotor_t *m)
{
	acqOneMotor_t *om;
	char pvname[100];

	if( m == NULL)
		return 0;
	m->waitEvent = epicsEventCreate(0);
	for( om=m->list; om; om = (om->next==m->list?NULL:om->next) )
	{
		snprintf( pvname, sizeof pvname, "%s:moving", om->name);
		om->busy = build_connector( NULL, pvname);
		om->busy->update = busyCallback;
		om->busy->user_data = om;
		snprintf( pvname, sizeof pvname, "%s:done", om->name);
		om->done = build_connector( NULL, pvname);
		om->done->update = doneCallback;
		om->done->user_data = om;
		om->moving = 0;
	}
	return 1;
}

/*
 * close elements of the running motor record
 */

int
shutdown_acqMotor( acqMotor_t *m)
{
	acqOneMotor_t *om;
	if( m == NULL)
		return 0;
	epicsEventDestroy( m->waitEvent);
	for( om=m->list; om; om = (om->next==m->list?NULL:om->next) )
	{
		connector_disconnect( om->busy);
		connector_disconnect( om->done);
		om->busy = NULL;
		om->done = NULL;
	}
	return 1;
}

/*
 * $Log: acqMotor.c  $
 * Revision 1.3.1.3 2009/03/04 15:13:47CST Glen Wright (wrightg) 
 * Revisions for caller-defined output streams
 * Revision 1.3.1.2 2008/05/28 13:53:13CST Glen Wright (wrightg) 
 * Initial 3.0 version
 * Revision 1.3.1.1 2007/05/08 09:26:37CST Glen Wright (wrightg) 
 * Duplicate revision
 * Revision 1.2 2007/03/18 15:04:20CST matiase 
 * Added header/log
 */
