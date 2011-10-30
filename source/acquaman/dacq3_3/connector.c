/* $Header: epicsConnectApp/src/connector.c 1.2 2009/12/23 09:58:54CST Glen Wright (wrightg) Exp  $
 * Copyright Canadian Light Source, Inc. All rights reserved.
 *
 * Description:
 *
 */


#include <stdlib.h>
#include <string.h>
#include "epicsConnect.h"


Connector *
build_connector(void *private, const char *pvname)
{
	Channel *chan;
	Connector *conp;
	char expandPV[300];

	if( pvname == NULL || *pvname == '\0' )
		return NULL;
	conp = (Connector *)calloc(1, sizeof *conp);
	if( !conp)
		return NULL;
	macro_expand( pvname, expandPV, NULL);
	conp->chan = get_channel( expandPV);
	if( !conp->chan)
	{
		free(conp);
		return NULL;
	}
	chan = conp->chan->chan;
	if( !chan)
	{
		free(conp);
		return NULL;
	}
	conp->unexpanded_pv = strdup( pvname);

	epicsConnectLock();
	conp->next_chan = chan->first_connector;
	chan->first_connector = conp;
	epicsConnectUnlock();

	if( chan->valid)
		conp->alarmSeverity = chan->severity;
	else
		conp->alarmSeverity = 4;			/* no connection */

	conp->private_data = private;
	return conp;
}

/*
 * go through the list of connectors on a channel, and disconnect the channel from this connector.
 */
void
connector_disconnect( Connector *conp)
{
	/*Channel *chanp;*/
	Connector *prev=NULL, *cur;

	if( conp == NULL)
		return;
	if( conp->chan == NULL || conp->chan->chan == NULL)
	{
		free( conp->unexpanded_pv);
		free(conp);
		return;
	}
	epicsConnectLock();
	conp->update = conp->newAccess = conp->newState = NULL;
	for( cur=conp->chan->chan->first_connector; cur; cur = cur->next_chan)
	{
		if( cur == conp)
		{
			if( prev)
				prev->next_chan = cur->next_chan;
			else
				conp->chan->chan->first_connector = cur->next_chan;
			unref_channel( conp->chan);
			free( conp->unexpanded_pv);
			free( conp);
			break;
		}
		prev = cur;
	}
	epicsConnectUnlock();
}

/* 
 * $Log: epicsConnectApp/src/connector.c  $
 * Revision 1.2 2009/12/23 09:58:54CST Glen Wright (wrightg) 
 * Compilation messages cleanup;
 * more cautious cleanup on removal of a connector and channel;
 * bug fix when monitor callback structure doesn't have ECA_NORMAL as a status.
 * Revision 1.1 2008/05/08 10:19:19CST David Beauregard (beaured0) 
 * Initial revision
 * Member added to project e:/MKS_Home/archive/cs/epics_local/epics_tools/epicsConnect/project.pj
 * Revision 1.3 2007/05/19 12:39:20CST Elder Matias (matiase) 
 * Added header/footer
 *
 */
