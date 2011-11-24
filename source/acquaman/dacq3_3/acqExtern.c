/**
 ** $Header: acqExtern.c 1.4.1.3 2009/03/04 15:13:46CST Glen Wright (wrightg) Exp  $
 ** Copyright Canadian Light Source, Inc. All rights reserved.
 **
 ** Description:
 **   Threefold responsibilities
 **     1. declare the extern variables
 **     2. redefine functions specific to the library version
 **     3. add functions useful for external use of the library
 */

#include "acquisitionLib.internal.h"
#include <stdio.h>

int
acqRegisterOutputHandler( acqMaster_t *master, acqKey_t key, struct eventDataHandler *edh)
{
	int i;
	/*
	 * don't register null arguments
	 */
	if( key == NULL || edh == NULL)
		return -1;
	/*
	 * don't re-register an existing key, as this would cause duplicate calls for the 
	 * same key resulting in mass confusion and rioting in the streets. If a duplicate
	 * key is entered, then the event handler list is updated.
	 */
	for(i=0; i < master->numOutputHandler; i++)
	{
		if( key == master->outputKeys[i])
		{
			master->outputHandler[i] = edh;
			return 0;
		}
	}
	i = master->numOutputHandler++;
	master->outputHandler = (eventDataHandler_t **)realloc(master->outputHandler, (master->numOutputHandler)*sizeof (eventDataHandler_t *) );
	master->outputKeys = (void *) realloc( master->outputKeys, (master->numOutputHandler)* sizeof (void *));
	master->handlerLock = (epicsMutexId *) realloc( master->handlerLock, (master->numOutputHandler) * sizeof (epicsMutexId));
	master->outputHandler[i] = edh;
	master->outputKeys[i] = key;
	master->handlerLock[i] = epicsMutexCreate();
	return 0;
}

/*
 * inter-handler communication: broadcast a signal from 'key' to all other handlers
 */
int acqHandlerSignal( acqMaster_t *master, acqKey_t key, unsigned int signalID, const void *data)
{
	int i;

	for(i=0; i < master->numOutputHandler; i++)
	{
		if( key != master->outputKeys[i])
		{
			epicsMutexLock(master->handlerLock[i]);
			master->outputHandler[i]->getHandlerSignal_cb(master->outputKeys[i], key, signalID, data);
			epicsMutexUnlock(master->handlerLock[i]);
		}
	}
	return 0;
}

int acqRemoveOutputHandler(acqMaster_t *master, acqKey_t key)
{
	int i;
	for( i=0; i < master->numOutputHandler; i++)
	{
		if( key == master->outputKeys[i])
			break;
	}
	if (i >= master->numOutputHandler)
		return -1;
	epicsMutexDestroy( master->handlerLock[i]);
	master->numOutputHandler--;
	for( ; i < master->numOutputHandler; i++)
	{
		master->outputKeys[i] = master->outputKeys[i+1];
		master->outputHandler[i] = master->outputHandler[i+1];
	}
	return 0;
}

//int acqClearOutputHandlers(acqMaster_t *master)
// Changed this to void from int to fix compiler warning. See if it works. (David Chevrier, Aug 25 2011)
void acqClearOutputHandlers(acqMaster_t *master)
{
	master->numOutputHandler = 0;
	free( master->outputKeys);
	master->outputKeys = 0;
	free( master->outputHandler);
	master->outputHandler = 0;
}


acqMaster_t *
new_acqMaster()
{
	acqMaster_t *master;

	master = calloc( 1, sizeof (acqMaster_t) );
	master->acqRunMax = 1;
	setDefaultMessage(master);
	/*
	 * following calls set the callbacks to their defaults
	 */
	acqSetShowProgress( NULL, master);
	acqSetOnStop( NULL, master);

	return master;
}

/**
 ** support functions
 **/

/*
 * map between strings and numerics for trigger&action keywords
 */
struct tgKeyword {
	char *name;
	int idx;
};
static struct tgKeyword tgKeyword[] =
{
	{ "Begin", ACQ_TG_BEGIN },
	{ "Move", ACQ_TG_MOVE },
	{ "Dwell", ACQ_TG_DWELL },
	{ "Finish", ACQ_TG_DONE },
	{ "Done", ACQ_TG_DONE },
	{ "Pause", ACQ_TG_PAUSE },
	{ "EndPause", ACQ_TG_PAUSE_END },
	{ "PauseEnd", ACQ_TG_PAUSE_END },
	{ "StartPass", ACQ_TG_BEGIN_PASS },
	{ "EndPass", ACQ_TG_END_PASS },
};

#define NtgKeyword (sizeof tgKeyword/sizeof tgKeyword[0])
char *
tgTypeName(int idx)
{
	//int i;
	//Fixed compiler comparison warning (David Chevrier, Aug 25 2011)
	unsigned int i;
	for( i=0 ;i < NtgKeyword; i++)
		if( tgKeyword[i].idx == idx)
			return tgKeyword[i].name;
	return "Unknown";
}

int
tgTypeIndex(char *tname)
{
	//int i;
	//Fixed compiler comparison warning (David Chevrier, Aug 25 2011)
	unsigned int i;
	for( i=0 ;i < NtgKeyword; i++)
		if( strcmp(tgKeyword[i].name,tname) == 0)
			return tgKeyword[i].idx;
	return -1;
}

/*
 * set a character pointer to a new value.
 * Don't assign if the value is unchanged.
 * Free previously assigned values.
 */
void
setCharPointerFromEntry( char **ptr, const char *str)
{
	/*
	 * if both set, and already equal, don't change.
	 */
	if( *ptr && str)
	{
		if( strcmp(*ptr, str) == 0)
			return;
	}

	if( *ptr)
		free(*ptr);
	if( str == NULL || *str == '\0')
	{
		*ptr = NULL;
		return;
	}
	*ptr = strdup(str);
}

/*
 * returns column number '1' to 'N'. This can be used as a direct index into the double array
 * passed to a callback routine.
 * -1 returned if the pvName is not being recorded (not in list or record disabled)
 */
int
getPVcolumn(const char *eventName, const char *pvName, const acqMaster_t *master)
{
	acqEvent_t *ev;
	int i;
	int colNum;
	acqPv_t *pv;

	ev = lookup_acqEvent(eventName, master);
	if( ev == NULL)
		return -1;

	colNum = 0;
	if( ev->putEventId)
		colNum++;
	if( ev->putAbsTime)
		colNum++;
	if( ev->putRel0Time)
		colNum++;
	if( ev->putRelPTime)
		colNum++;

	for(i=0; i < ev->numPvList; i++)
	{
		pv = &ev->pvList[i];
		if( pv->noRecord == 1)
			continue;
		if( pv->pvName && strcmp(pv->pvName, pvName) == 0)
			return colNum;
		colNum++;
	}
	return -1;
}

/*
 * $Log: acqExtern.c  $
 * Revision 1.4.1.3 2009/03/04 15:13:46CST Glen Wright (wrightg) 
 * Revisions for caller-defined output streams
 * Revision 1.4.1.2 2008/05/28 13:53:12CST Glen Wright (wrightg) 
 * Initial 3.0 version
 * Revision 1.4.1.1 2007/03/18 14:55:15CST Glen Wright (wrightg) 
 * Duplicate revision
 */
