/**
 */

#include <stdio.h>
#include <libxml/xmlreader.h>
#include "acquisitionLib.h"

/*
 * return 0 for a '0' or 'no' value, 1 for a '1' or 'yes' value, and 2 for a '?' value. all other values return -1.
 */
static int getBinary(const char *str)
{
	if( str == NULL)
		return -1;
	if( strcmp( str, "0") == 0)
		return 0;
	if( strcmp( str, "1") == 0)
		return 1;
	if( strcasecmp( str, "no") == 0)
		return 0;
	if( strcasecmp( str, "yes") == 0)
		return 1;
	if( strcmp( str, "?") == 0)
		return 2;
	return -1;
}

static int getEnum( const char *str, char **list)
{
	int i;
	char *estr;

	i = strtol(str, &estr, 10);
	if( estr && *estr == '\0')
		return i;

	for( i=0; list[i]; i++)
		if( strcmp(str, list[i]) == 0)
			return i;
	return -1;
}

static int
getAttributes( xmlTextReaderPtr reader, char **list, char **value)
{
	int i;

	for(i=0; list[i]; i++)
	{
		value[i] = (char *)xmlTextReaderGetAttribute( reader, (const xmlChar *)list[i]);
	}
	return i;
}

static void
freeAttributes( char **list, char **value)
{
	int i;

	for(i=0; list[i]; i++)
		if( value[i])
			free( value[i]);
}

static int
getNextNode( xmlTextReaderPtr reader)
{
	int ret;

        ret = xmlTextReaderRead(reader);
	return ret;
}

static void
parseNode( xmlTextReaderPtr reader, const char** name, int *nodeType, int *isEmpty, const char**value)
{
	*name = (const char *)xmlTextReaderConstName(reader);
	if (*name == NULL)
		*name = "--";

	if( value)
		*value = (const char *)xmlTextReaderConstValue(reader);
	if( nodeType)
		*nodeType = xmlTextReaderNodeType(reader);
	if (isEmpty)
		*isEmpty = xmlTextReaderIsEmptyElement(reader);
}

static void process_config(xmlTextReaderPtr reader, acqMaster_t *);
static void process_scan(xmlTextReaderPtr reader, acqMaster_t *);
static void process_event(xmlTextReaderPtr reader, acqMaster_t *);
static void process_control(xmlTextReaderPtr reader, acqScan_t *);
static void process_actionList(xmlTextReaderPtr reader, acqScan_t *);
static void process_pv(xmlTextReaderPtr reader, acqEvent_t *);
static void process_flags(xmlTextReaderPtr reader, acqEvent_t *);
#if ACQ_VERSION_LT(3,0,0)
static void process_file(xmlTextReaderPtr reader, acqMaster_t *);
#endif
#if ACQ_VERSION_GE(3,0,0)
static void process_handler(xmlTextReaderPtr reader, acqMaster_t *);
#endif
/*
 * when called, the reader has just found 'config'
 * returns at end of 'config'
 */
static void
process_config( xmlTextReaderPtr reader, acqMaster_t *master)
{
	char * attributeList[2] = { "version", NULL };
	char * attributeValue[2];
	const char *name;
	int nodeType;
	int isEmpty;
	const char *value;
	char *err;

	parseNode( reader, &name, &nodeType, &isEmpty, &value);
	if( nodeType != 1)
		return;
	getAttributes( reader, attributeList, attributeValue);

	master->fileVersion = strtol( attributeValue[0], &err, 10);

	if( *err)
		printf("Illegal version string '%s'\n", attributeValue[0]);
	freeAttributes( attributeList, attributeValue);

	for( ;;)
	{
		getNextNode(reader);
		parseNode( reader, &name, &nodeType, &isEmpty, &value);
		if(strcmp( name, "config") == 0)
		{
			if( nodeType != 15)
			{
				/* error! */
				printf("process_config: expecting </config>\n");
			}
			return;
		}
		if(strcmp( name, "scan") == 0)
		{
			if( nodeType == 1)
				process_scan( reader, master);
			else
			{
				/* error! */
				printf("process_config: expecting <scan>\n");
			}
			continue;
		}
		if(strcmp( name, "event") == 0)
		{
			if( nodeType == 1)
				process_event( reader, master);
			else
			{
				/* error! */
				printf("process_config: expecting <event>\n");
			}
			continue;
		}
#if ACQ_VERSION_LT(3,0,0)
		if(strcmp( name, "file") == 0)
		{
			process_file( reader, master);
			continue;
		}
#endif
#if ACQ_VERSION_GE(3,0,0)
		if(strcmp( name, "handler") == 0)
		{
			process_handler( reader, master);
			continue;
		}
#endif
		if(strcmp( name, "#text") == 0)
			continue;

		/* Error! */
		printf("Unknown entry type '%s'\n", name);
	}
}

/*
 * called when a node of type '1' named 'scan' is read.
 * returns when /scan is read.
 */
static void
process_scan( xmlTextReaderPtr reader, acqMaster_t *master)
{
	char * attributeList[4] = { "name", "id", "onStart", NULL };
	char * attributeValue[4];
	const char *name;
	int nodeType;
	int isEmpty;
	const char *value;


	getAttributes( reader, attributeList, attributeValue);

	acqScan_t *scanp = new_acqScan(attributeValue[0], master);

	scanp->scanNum = strtol( attributeValue[1], NULL, 0);
	scanp->useStart = getBinary(attributeValue[2]);

	for( ;;)
	{
		getNextNode(reader);
		parseNode( reader, &name, &nodeType, &isEmpty, &value);

		if( strcmp(name, "scan") == 0)
		{
			return;
		}
		if( strcmp(name, "control") == 0)
		{
			process_control(reader, scanp);
			continue;
		}
		if( strcmp(name, "actionlist") == 0)
		{
			process_actionList(reader, scanp);
			continue;
		}
		if(strcmp( name, "#text") == 0)
			continue;

		/* Error! */
	}
}

static void
process_event( xmlTextReaderPtr reader, acqMaster_t *master)
{
	char * attributeList[3] = { "name", "id", NULL };
	char * attributeValue[3];
	const char *name;
	int nodeType;
	int isEmpty;
	const char *value;
	acqEvent_t *eventp;

	getAttributes( reader, attributeList, attributeValue);
	eventp = new_acqEvent(attributeValue[0], master);

	eventp->eventNum = strtol( attributeValue[1], NULL, 10);

	for( ;;)
	{
		getNextNode(reader);
		parseNode( reader, &name, &nodeType, &isEmpty, &value);

		if( strcmp(name, "event") == 0)
		{
			return;
		}
		if( strcmp(name, "flags") == 0)
		{
			process_flags(reader, eventp);
			continue;
		}
		if( strcmp(name, "pv") == 0)
		{
			process_pv(reader, eventp);
			continue;
		}
		if(strcmp( name, "#text") == 0)
			continue;

		/* Error! */
	}
}

#if ACQ_VERSION_LT(3,0,0)
static void
process_file(xmlTextReaderPtr reader, acqMaster_t *master)
{
	char * attributeList[8] = { "path", "template", "sequence", "time", "datedir", "seqno", "spectrumFormat", NULL };
	char * attributeValue[8];
	acqFile_t *afp;
	char *err;

	getAttributes( reader, attributeList, attributeValue);

	if( master->acqFile == NULL)
		master->acqFile = new_acqFile(master);

	afp = master->acqFile;
	if( strcmp(attributeValue[0], "") != 0 && strcmp(attributeValue[0], "(null)") != 0)
		setCharPointerFromEntry( &afp->dirname, attributeValue[0]);
	if( strcmp(attributeValue[1], "") != 0 && strcmp(attributeValue[1], "(null)") != 0)
		setCharPointerFromEntry( &afp->fileTemplate, attributeValue[1]);
	afp->appendSeq = getBinary(attributeValue[2]);
	afp->appendTime = getBinary(attributeValue[3]);
	afp->dateDirectory = getBinary(attributeValue[4]);
	afp->seqno = strtol(attributeValue[5], &err, 10);

	if( strcmp(attributeValue[6], "2") == 0 || strcmp(attributeValue[6], "text") == 0 )
		afp->requestSpectrumFormat = 2;
	else
	if( strcmp(attributeValue[6], "1") == 0 || strcmp(attributeValue[6], "binary") == 0 )
		afp->requestSpectrumFormat = 1;
	else
		afp->requestSpectrumFormat = 0;

	freeAttributes(attributeList, attributeValue);
}
#endif

static void
process_control(xmlTextReaderPtr reader, acqScan_t *scanp)
{
	char * attributeList[6] = { "PVname", "start", "delta", "final", "active", NULL };
	char * attributeValue[6];
	acqControl_t ctl;
	char *err;

	memset( (char *) &ctl, '\0', sizeof ctl);
	getAttributes( reader, attributeList, attributeValue);
	ctl.controlPV = attributeValue[0];
	if( strcmp(attributeValue[1],"") != 0)
	{
		ctl.startVal = strtod( attributeValue[1], &err);
		ctl.haveStartVal = 1;
#if ACQ_VERSION_GE(3,0,0)
		/* if 'err' is set, then try and set the value into the macro string */
		if( err && err == attributeValue[1] && attributeValue[1][0] == '$')
			ctl.startMacro = attributeValue[1];
#endif
	}
	if( strcmp(attributeValue[2],"") != 0)
	{
		ctl.deltaVal = strtod( attributeValue[2], &err);
		ctl.haveDeltaVal = 1;
#if ACQ_VERSION_GE(3,0,0)
		/* if 'err' is set, then try and set the value into the macro string */
		if( err && err == attributeValue[2] && attributeValue[2][0] == '$')
			ctl.deltaMacro = attributeValue[2];
#endif
	}
	if( strcmp(attributeValue[3],"") != 0)
	{
		ctl.finalVal = strtod( attributeValue[3], &err);
		ctl.haveFinalVal = 1;
#if ACQ_VERSION_GE(3,0,0)
		/* if 'err' is set, then try and set the value into the macro string */
		if( err && err == attributeValue[3] && attributeValue[3][0] == '$')
			ctl.deltaMacro = attributeValue[3];
#endif
	}
	addScanControl( scanp, &ctl);

	freeAttributes(attributeList, attributeValue);

}

static void
process_actionList(xmlTextReaderPtr reader, acqScan_t *scanp)
{
	char * attributeList[2] = { "type", NULL };
	char * attributeValue[2];
	const char *name;
	int nodeType;
	int isEmpty;
	const char *value;

	char *err;
	int actionIndex;

	getAttributes( reader, attributeList, attributeValue);
	actionIndex = tgTypeIndex( attributeValue[0]);

	freeAttributes(attributeList, attributeValue);

	for( ;;)
	{
		getNextNode(reader);
		parseNode( reader, &name, &nodeType, &isEmpty, &value);
		
		if( strcmp(name, "actionlist") == 0)
		{
			return;
		}

		if( actionIndex < 0)
			continue;

		if(strcmp( name, "#text") == 0)
			continue;

		if( strcmp( name, "Delay") == 0)
		{
			char *atList[2] = { "value", NULL};
			char *atValue[2];
			getAttributes( reader, atList, atValue);
			new_acqAction_delayTime( &scanp->actions[actionIndex], strtod(atValue[0],&err),  scanp->master);
			freeAttributes( atList, atValue);
			continue;
		}

		if( strcmp( name, "SetPV") == 0)
		{
			char *atList[3] = { "name", "value", NULL};
			char *atValue[3];
			getAttributes( reader, atList, atValue);
			new_acqAction_setPV( &scanp->actions[actionIndex], atValue[0], atValue[1], scanp->master);
			freeAttributes( atList, atValue);
			continue;
		}

		if( strcmp( name, "WaitPV") == 0)
		{
			char *atList[3] = { "name", "value", NULL};
			char *atValue[3];
			getAttributes( reader, atList, atValue);
			new_acqAction_waitPV( &scanp->actions[actionIndex], atValue[0], atValue[1], scanp->master);
			freeAttributes( atList, atValue);
			continue;
		}

		if( strcmp( name, "WaitMotor") == 0)
		{
			char *atList[2] = { "name", NULL};
			char *atValue[2];
			getAttributes( reader, atList, atValue);
			new_acqAction_waitMotor( &scanp->actions[actionIndex], atValue[0], scanp->master);
			freeAttributes( atList, atValue);
			continue;
		}

		if( strcmp( name, "CallEvent") == 0)
		{
			char *atList[3] = { "name", "wait", NULL};
			char *atValue[3];
			getAttributes( reader, atList, atValue);
			new_acqAction_callEvent(  &scanp->actions[actionIndex], atValue[0], getBinary(atValue[1]), scanp->master);
			freeAttributes( atList, atValue);
			continue;
		}

		if( strcmp( name, "CallScan") == 0)
		{
			char *atList[3] = { "name", "wait", NULL};
			char *atValue[3];
			getAttributes( reader, atList, atValue);
			new_acqAction_callScan(  &scanp->actions[actionIndex], atValue[0], getBinary(atValue[1]), scanp->master);
			freeAttributes( atList, atValue);
			continue;
		}

		if( strcmp( name, "WaitEvent") == 0)
		{
			char *atList[2] = { "name", NULL};
			char *atValue[2];
			getAttributes( reader, atList, atValue);
			new_acqAction_waitEvent(  &scanp->actions[actionIndex], atValue[0], scanp->master);
			freeAttributes( atList, atValue);
			continue;
		}

		if( strcmp( name, "WaitScan") == 0)
		{
			char *atList[2] = { "name", NULL};
			char *atValue[2];
			getAttributes( reader, atList, atValue);
			new_acqAction_waitScan(  &scanp->actions[actionIndex], atValue[0], scanp->master);
			freeAttributes( atList, atValue);
			continue;
		}

		if( strcmp( name, "SetControl") == 0)
		{
			new_acqAction_setControl(  &scanp->actions[actionIndex], scanp->master);
			continue;
		}

#if ACQ_VERSION_GE(3,0,0)
		if( strcmp( name, "NextOutput") == 0)
		{
			new_acqAction_nextOutput( &scanp->actions[actionIndex], scanp->master);
			continue;
		}
#endif
		printf("Unknown action list entry '%s'\n", name);
		xmlTextReaderNext(reader);
	}
}

static void
process_flags(xmlTextReaderPtr reader, acqEvent_t *eventp)
{
	char * attributeList[6] = { "eventID", "absTime", "Rel0Time", "RelPTime", "commentPrefix", NULL };
	char * attributeValue[6];
	int binval;

	getAttributes( reader, attributeList, attributeValue);
	binval = getBinary(attributeValue[0]);
	if( binval == 0 || binval == 1)
		eventp->putEventId = binval;
	binval = getBinary(attributeValue[1]);
	if( binval == 0 || binval == 1)
		eventp->putAbsTime = binval;
	binval = getBinary(attributeValue[2]);
	if( binval == 0 || binval == 1)
		eventp->putRel0Time = binval;
	binval = getBinary(attributeValue[3]);
	if( binval == 0 || binval == 1)
		eventp->putRelPTime = binval;
	binval = getBinary(attributeValue[4]);
	if( binval == 0 || binval == 1)
		eventp->putCommentPrefix = binval;
	freeAttributes( attributeList, attributeValue);
}

static void
process_pv(xmlTextReaderPtr reader, acqEvent_t *eventp)
{
	char * attributeList[6] = { "name", "disable", "spectrum", "ready", "format", NULL };
	char * attributeValue[6];
	char *enumList[4] = { "immediate", "readback", "wait", NULL};
	char *format = NULL;

	getAttributes( reader, attributeList, attributeValue);
	if( attributeValue[4] && *attributeValue[4])
		format = attributeValue[4];
	addEventPv( eventp, attributeValue[0], getBinary(attributeValue[1]), format, (acqPvReady_t)getEnum(attributeValue[3], &enumList[0]), getBinary(attributeValue[2]) );
	freeAttributes( attributeList, attributeValue);
}

#if ACQ_VERSION_GE(3,0,0)
/*
 * what is required is:
 *  locate and load the library (if available).
 *  set the instance identifier to the input name (if available)
 *  set the listed properties.
 */
static void
process_handler(xmlTextReaderPtr reader, acqMaster_t *master)
{
	const char *name;
	const char *value;
	int nodeType;
	int isEmpty;

	char * attributeList[3] = { "name", "library", NULL };
	char * attributeValue[3];

	getAttributes( reader, attributeList, attributeValue);
	/* TO DO:
	 * create an output handler and register it */
	freeAttributes( attributeList, attributeValue);

	for( ;;)
	{
		getNextNode(reader);
		parseNode( reader, &name, &nodeType, &isEmpty, &value);
		if( strcmp( name, "property") == 0)
		{
			static char *aList[2] = { "name", NULL };
			static char *aValue[2] ;
			getAttributes( reader, aList, aValue);
			/* TO DO:
			 * set the property of this output handler */
			freeAttributes( aList, aValue);
			continue;
		}
		if( strcmp( name, "handler") == 0)
		{
			if( nodeType == 15)
				return;
			/* error! */
			printf("Unexpected 'handler' node type %d\n", nodeType);
		}
		/* ERROR! */
	}
}
#endif


/**
 *
 * Parse and save configuration from an XML
 */
int
acq_ReadXMLconfig(const char *filename, acqMaster_t *master) {
    xmlTextReaderPtr reader;
    int ret;

    reader = xmlReaderForFile(filename, NULL, XML_PARSE_DTDVALID);
    if (reader != NULL) {
    	while( getNextNode(reader) == 1)
		{
			const char *name, *value;

			parseNode( reader, &name, NULL, NULL, NULL);

			if( strcmp(name, "config") == 0)
				process_config( reader, master);
		}
        xmlFreeTextReader(reader);
    }
	else
	{
        fprintf(stderr, "Unable to open %s\n", filename);
	return -1;
    }
    return 0;
}
