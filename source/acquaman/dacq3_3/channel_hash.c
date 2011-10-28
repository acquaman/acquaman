/* $Header: epicsConnectApp/src/channel_hash.c 1.2 2009/12/23 09:58:55CST Glen Wright (wrightg) Exp  $
 * Copyright Canadian Light Source, Inc. All rights reserved.
 *
 * Description:
 */


#include <stdlib.h>
#include <string.h>
#include "epicsConnect.h"
#include <errlog.h>

#define HASHSIZE 7963

static channel_hash_t *ht[HASHSIZE];

typedef struct contextList contextList_t;
struct contextList 
{
	contextList_t *next;
	struct ca_client_context *context;
	enum { ENABLE, DISABLE } context_type;
};

static contextList_t *conListHead;

static contextList_t *
findContext( struct ca_client_context *context)
{
	contextList_t *cp;
	for( cp=conListHead; cp; cp=cp->next)
		if( cp->context == context)
			return cp;
	return NULL;
}

static contextList_t *
addContext( struct ca_client_context *context, int pre_empt )
{
	contextList_t *cp;
	cp = findContext( context);
	if( cp)
		return cp;
	cp = calloc(1, sizeof *cp);
	cp->context = context;
	cp->context_type = pre_empt?ENABLE:DISABLE;
	cp->next = conListHead;
	conListHead = cp;
	return cp;
}

void
epicsConnectAddContext( struct ca_client_context *context, int pre_empt)
{
	addContext( context, pre_empt);
}

static contextList_t *
find_hash_context( )
{
	struct ca_client_context *context;
	contextList_t *cp;
	context = ca_current_context();
	if( context == NULL)
	{
		/* ugly: we are about to associate a PV with a context, and no context exists.
		 * the default action is that a disable_preemptive_callback will be called.
		 * we beat the channel access code to the job by creating a context so we know
		 * what to use!
		 */
		ca_context_create( ca_disable_preemptive_callback);
		context = ca_current_context();
		cp = addContext( context, 0);
		return cp;
	}
	return findContext( context);
}
/*
 * calculate the hash for a string.
 */
static int gethash(char *val)
{
	unsigned int hash=0;
	while( *val)
		hash = (hash<<1) + *val++;
	return hash % HASHSIZE;
}

/*
 * find an entry for a given hash value. returns NULL if no match.
 */
static channel_hash_t *
find_hash(char *pvName, int hash, struct ca_client_context *context)
{
	channel_hash_t *e;

	for( e = ht[hash]; e ; e=e->next)
	{
		if( e->chan == NULL)		/* if a channel was never set, ignore this entry! */
			continue;
		if( e->context != context)	/* if looking for a particular context, must match */
			continue;
		if( strcmp( e->chan->chan_name, pvName) == 0)
			return e;
	}
	return NULL;
}

/*
 * add an entry at the given hash value.
 * creates the channel.
 */
static channel_hash_t *
add_hash(char * pvName, int hash, struct ca_client_context *context)
{
	channel_hash_t *e;
	e = calloc(1, sizeof (*e) );
	if( !e)
		return NULL;
	e->next = ht[hash];
	e->hashval = hash;
	ht[hash] = e;
	e->chan = build_PV(pvName);
	e->context = context;
	return e;
}

/*
 * return the table entry for the named channel.
 * creates one if it doesn't exist.
 * refcnt is always incremented.
 * returns NULL on error.
 */
channel_hash_t *
get_channel(char * pvName)
{
	int hash = gethash(pvName);
	channel_hash_t * e;
	struct ca_client_context *context;
	contextList_t *cp;

	cp = find_hash_context();
	if( cp->context_type == ENABLE)
		context = 0;
	else
		context = cp->context;
	e = find_hash(pvName, hash, context);
	if(e == NULL)
		e = add_hash( pvName, hash, context);
	if(e)
	{
		e->chan->unused = 0;
		e->refcnt++;
	}
	return e;
}
/*
 * looks for the table entry. returns NULL on
 * lookup failure. refcnt is unchanged.
 */
channel_hash_t *
lookup_channel(char * pvName)
{
	int hash = gethash(pvName);
	contextList_t *cp;
	struct ca_client_context *context;
	cp = find_hash_context();
	if( cp->context_type == ENABLE)
		context = 0;
	else
		context = cp->context;
	return find_hash(pvName, hash, context);
}
/*
 * decrements the reference count, and deletes the entry
 * if the reference count is <= 0;
 */
void
unref_channel( channel_hash_t * tableEntry)
{
	int hash;
	channel_hash_t *e,
			*prev;

	if( --tableEntry->refcnt > 0)
		return;

	if( tableEntry->chan == NULL)		/* shouldn't happen */
		return;

	/*
	 * remove the channel description. This is supposed to drop all outstanding callbacks.
	 */
	if( tableEntry->chan->valid == 1)
		ca_clear_event( tableEntry->chan->eventId);

	ca_clear_channel( tableEntry->chan->chan_id);
	ca_flush_io();
	tableEntry->chan->eventId = 0;

	tableEntry->chan->unused = 1;
	/* if there is more than one connector, we've miscounted the references.
	 * Things will start falling apart at this point,
	 */
	if( tableEntry->chan->first_connector && tableEntry->chan->first_connector->next_chan)
		errlogPrintf("unref_channel: disconnecting extra Connectors for %s\n", tableEntry->chan->chan_name);

	tableEntry->chan->first_connector = NULL;

#if 0
	/* note: because we're never certain about callbacks, we don't free the actual data
	 * structure. In theory, the 'ca_clear_channel' above should stop future callbacks.
	 */
	free( tableEntry->chan->chan_name);
	free( tableEntry->chan);
#endif

	tableEntry->chan = NULL;

	/*
	 * remove the entry from the hash list
	 */
	hash = tableEntry->hashval;
	prev = NULL;
	for( e = ht[hash] ; e ; e=e->next)
	{
		if( e == tableEntry)
			break;
		prev = e;
	}
	if( !e)		/* should record an error: unable to delete! */
		return;
	if( prev)
		prev->next = e->next;
	else
		ht[hash] = e->next;
	free( e);
}

/*
 * $Log: epicsConnectApp/src/channel_hash.c  $
 * Revision 1.2 2009/12/23 09:58:55CST Glen Wright (wrightg) 
 * Compilation messages cleanup;
 * more cautious cleanup on removal of a connector and channel;
 * bug fix when monitor callback structure doesn't have ECA_NORMAL as a status.
 * Revision 1.1 2008/05/08 10:19:19CST David Beauregard (beaured0) 
 * Initial revision
 * Member added to project e:/MKS_Home/archive/cs/epics_local/epics_tools/epicsConnect/project.pj
 * Revision 1.3 2007/05/19 12:37:35CST Elder Matias (matiase) 
 * Added header/log
 */

