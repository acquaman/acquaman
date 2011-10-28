/*
 * $Header: acqMessage.c 1.2.1.2 2008/05/28 13:53:11CST Glen Wright (wrightg) Exp  $
 * Copyright Canadian Light Source, Inc. All rights reserved.
 *
 * Description:
 *   dummy functions; these should be defined in the user application to
 *   tie to display widgets.
 */

#include "acquisitionLib.internal.h"

// Added macro to avoid unused variable compiler warnings (David Chevrier, Oct 27 2011)
#define UNUSED(x) (void)(x)
void defaultMessageAdd(acqMaster_t *,char *fmt, ...);
static void defaultMessageBoxClear(acqMaster_t *);
static void defaultPutMode(acqMaster_t *, acqState mode);
/*
 * messageAdd(), messageBoxClear(), and putMode()
 * display status messages.
 */
void defaultMessageAdd(acqMaster_t *master, char *fmt, ...)
{
	//Added to fix compiler warning (David Chevrier, Aug 25 2011)
	UNUSED(master);
	va_list ap;

	va_start(ap, fmt);

	vprintf(fmt, ap);
	printf("\n");
}
static
void defaultMessageBoxClear(acqMaster_t *master)
{
	//Added to fix compiler warning (David Chevrier, Aug 25 2011)
	UNUSED(master);
}

static void
defaultPutMode(acqMaster_t *master, acqState mode)
{
	//Added to fix compiler warning (David Chevrier, Aug 25 2011)
	UNUSED(master);
	printf("Mode: %s", curState(mode) );
}

void
setDefaultMessage( acqMaster_t *master)
{
	if( master == NULL)
		return;
	master->messageAdd = (message_t)defaultMessageAdd;
	master->messageBoxClear = (messageboxclear_t)defaultMessageBoxClear;
	master->putMode = (putmode_t)defaultPutMode;
	master->acqStatusMessage = (message_t)defaultMessageAdd;
}

void
acqSetMessageAdd( message_t f, acqMaster_t *master)
{
	if( master == NULL)
		return;
	if( f == NULL)
		master->messageAdd = (message_t)defaultMessageAdd;
	else
		master->messageAdd = f;
}
void
acqSetMessageBoxClear( messageboxclear_t f, acqMaster_t *master)
{
	if( master == NULL)
		return;
	if( f == NULL)
		master->messageBoxClear = defaultMessageBoxClear;
	else
		master->messageBoxClear = f;
}
void
acqSetPutMode( putmode_t f, acqMaster_t *master)
{
	if( master == NULL)
		return;
	if( f == NULL)
		master->putMode = defaultPutMode;
	else
		master->putMode = f;
}
void
acqSetStatusMessage( message_t f, acqMaster_t *master)
{
	if( master == NULL)
		return;
	if( f == NULL)
		master->acqStatusMessage = (message_t)defaultMessageAdd;
	else
		master->acqStatusMessage = f;
}
/*
 * $Log: acqMessage.c  $
 * Revision 1.2.1.2 2008/05/28 13:53:11CST Glen Wright (wrightg) 
 * Initial 3.0 version
 * Revision 1.2.1.1 2007/03/18 15:01:15CST Glen Wright (wrightg) 
 * Duplicate revision
 */


