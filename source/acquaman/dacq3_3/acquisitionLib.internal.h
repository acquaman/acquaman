/*
 * Copyright 2007 Canadian Light Source, Inc. All rights reserved.
 *
 * In a C++ only world, these would be private entries.
 */

// Changed to ifndef and added DACQ_LIB_ prefix (David Chevrier, Oct 27 2011)
//#if !defined(ACQUISITIONLIB_INTERNAL_H)
#ifndef DACQLIB_ACQUISITIONLIB_INTERNAL_H
#define DACQLIB_ACQUISITIONLIB_INTERNAL_H 1
#include "acquisitionLib.h"

#define CONNECT_VALID(CONP) ((CONP&&CONP->chan)?(CONP->chan->chan->valid):0)

#define VERBOSE(M) if(M->acqVerbose)
#define VERBOSEHEADER(M) if(M->acqVerboseHeader)
#define DEBUGM(MASTER,LVL) if(MASTER->acqDebug>=LVL)

/*
 * for a given master, execute CODE for each handler - makes most sense for scanning the callbacks
 */
#define HANDLERS( MASTER, CODE) {int MYidx; \
				acqKey_t key; \
				eventDataHandler_t *odh; \
				epicsMutexId mutex; \
				for(MYidx=0;MYidx < MASTER->numOutputHandler; MYidx++) { \
					odh = MASTER->outputHandler[MYidx]; \
					key = MASTER->outputKeys[MYidx]; \
					mutex = MASTER->handlerLock[MYidx] ; \
					epicsMutexLock(mutex); \
					CODE ; \
					epicsMutexUnlock(mutex); \
				} }

/* runup delay is in seconds */
#define RUNUP_DELAY 6
/* msec delay is in milliseconds: this is the default delay;
 * and an application can change master->MsecDelay to reflect
 * the calling interval of acq_showStatus */
#define MSEC_DELAY 500
void callback_eventPVstate(Connector *conp);

void callback_PVmonitor( Connector *);
void callback_PVdescription( Connector *);
void callback_check_delay(Connector *);

#endif
