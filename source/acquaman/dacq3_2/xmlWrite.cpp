/**
 */
#include <stdio.h>
#include <string.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include "acquisitionLib.h"

#define MY_ENCODING "ISO-8859-1"

static xmlChar * ConvertInput(const char *, const char *);
static void xmlOutputScan( xmlTextWriterPtr writer, acqScan_t *scanp);
static void xmlOutputEvent( xmlTextWriterPtr writer, acqEvent_t *eventp);
static void xmlOutputControl( xmlTextWriterPtr writer, acqControl_t *ctlp);
static void xmlOutputActionList( xmlTextWriterPtr writer, acqAction_t *actp, int actionIndex);
#if ACQ_VERSION_LT(3,0,0)
static void xmlOutputFile( xmlTextWriterPtr writer, acqFile_t *filep);
#endif


static void
putStringAttribute(xmlTextWriterPtr writer, const char *name, const char * value)
{
	xmlTextWriterWriteAttribute( writer, ConvertInput(name, MY_ENCODING), ConvertInput(value, MY_ENCODING) );
}

static void
putBinaryAttribute(xmlTextWriterPtr writer, const char *name, int value)
{
	if (value)
		xmlTextWriterWriteAttribute( writer, (const xmlChar *)name, (const xmlChar *)"yes");
	else
		xmlTextWriterWriteAttribute( writer, (const xmlChar *)name, (const xmlChar *)"no");
}

static void
putIntegerAttribute(xmlTextWriterPtr writer, const char *name, int value)
{
	char strval[100];
	snprintf(strval, sizeof strval, "%d", value);
	xmlTextWriterWriteAttribute( writer, (const xmlChar *)name, (const xmlChar *)strval);
}

static void
putDoubleAttribute(xmlTextWriterPtr writer, const char *name, double value)
{
	char strval[100];
	snprintf(strval, sizeof strval, "%g", value);
	xmlTextWriterWriteAttribute( writer, (const xmlChar *)name, (const xmlChar *)strval);
}

///
/// externally callable routine
/// This is the major entry point to these routines.
/// given a file name and a master configuration, an xml file is written
///
void
acq_xmlOutputConfig(const char *filename, acqMaster_t *master)
{
    xmlTextWriterPtr writer;
	acqScan_t *scanp;
	acqEvent_t *eventp;

    writer = xmlNewTextWriterFilename(filename, 0);
	xmlTextWriterSetIndent( writer, 8);
	
    xmlTextWriterStartDocument( writer, NULL, NULL, "no");
    xmlTextWriterWriteDTD( writer, (const xmlChar *)"config", NULL, (const xmlChar *)"file://localhost/home/beamline/acqConfig.dtd", NULL);

	xmlTextWriterStartElement( writer, (const xmlChar *)"config");
	putIntegerAttribute( writer, "version", ACQ_VERSION_CURRENT );
#if ACQ_VERSION_LT(3,0,0)
	putBinaryAttribute( writer, "verbose", master->acqVerboseHeader);
#endif

	for( scanp=first_acqScan(master); scanp; scanp=next_acqScan(scanp) )
		xmlOutputScan( writer, scanp);

	for( eventp=first_acqEvent(master); eventp; eventp=next_acqEvent(eventp) )
		xmlOutputEvent( writer, eventp);

#if ACQ_VERSION_LT(3,0,0)
	xmlOutputFile( writer, master->acqFile);
#endif
	xmlTextWriterEndElement( writer);

    xmlTextWriterEndDocument(writer);

    xmlFreeTextWriter(writer);
}

static void
xmlOutputScan( xmlTextWriterPtr writer, acqScan_t *scanp)
{
	int i = 0;
	xmlTextWriterStartElement( writer, (const xmlChar *)"scan");
	putStringAttribute(writer, "name", scanp->scanName);
	putIntegerAttribute(writer, "id", scanp->scanNum);
	putIntegerAttribute(writer, "onStart", scanp->useStart);

	for( i=0; i < scanp->numControlPV; i++)
		xmlOutputControl( writer, &scanp->acqControlList[i]);

	for( i=0; i < NUM_SCAN_TRIGGERS; i++)
		xmlOutputActionList( writer, scanp->actions[i], i);

	xmlTextWriterEndElement( writer);
}

static void
xmlOutputControl( xmlTextWriterPtr writer, acqControl_t *ctlp)
{
	xmlTextWriterStartElement( writer, (const xmlChar *)"control");
	putStringAttribute(writer, "PVname", ctlp->controlPV);
	if( ctlp->haveStartVal)
		putDoubleAttribute( writer, "start", ctlp->startVal);
	if( ctlp->haveDeltaVal)
		putDoubleAttribute( writer, "delta", ctlp->deltaVal);
	if( ctlp->haveFinalVal)
		putDoubleAttribute( writer, "final", ctlp->finalVal);
	xmlTextWriterEndElement( writer);
}

static void
xmlOutputActionList( xmlTextWriterPtr writer, acqAction_t *actHead, int actionIndex)
{
	acqAction_t *actp;
	if( actHead == NULL)
		return;

	xmlTextWriterStartElement( writer, (const xmlChar *)"actionlist");
	putStringAttribute(writer, "type", tgTypeName(actionIndex) );

	for(actp=actHead; actp; actp=(actp->next==actHead?NULL:actp->next) )
	{
		switch( actp->type)
		{
		case AA_SET_CONTROL:
			xmlTextWriterStartElement( writer, (const xmlChar *)"SetControl");
			break;
		case AA_DELAY_TIME:
			xmlTextWriterStartElement( writer, (const xmlChar *)"Delay");
			putDoubleAttribute( writer, "value", actp->au.dt.delay);
			break;
		case AA_SET_PV:
			xmlTextWriterStartElement( writer, (const xmlChar *)"SetPV");
			putStringAttribute(writer, "name", actp->au.spv.name);
			putStringAttribute(writer, "value", actp->au.spv.value);
			break;
		case AA_WAIT_PV:
			xmlTextWriterStartElement( writer, (const xmlChar *)"WaitPV");
			putStringAttribute(writer, "name", actp->au.wpv.name);
			putStringAttribute(writer, "value", actp->au.wpv.value);
			break;
		case AA_WAIT_MOTOR:
			xmlTextWriterStartElement( writer, (const xmlChar *)"WaitMotor");
			putStringAttribute(writer, "name", actp->au.wm.name);
			break;
		case AA_CALL_EVENT:
			xmlTextWriterStartElement( writer, (const xmlChar *)"CallEvent");
			putStringAttribute(writer, "name", actp->au.ce.name);
			putBinaryAttribute( writer, "wait", actp->au.ce.autoWait);
			break;
		case AA_WAIT_EVENT:
			xmlTextWriterStartElement( writer, (const xmlChar *)"WaitEvent");
			putStringAttribute(writer, "name", actp->au.we.name);
			break;
		case AA_CALL_SCAN:
			xmlTextWriterStartElement( writer, (const xmlChar *)"CallScan");
			putStringAttribute(writer, "name", actp->au.cs.name);
			putBinaryAttribute( writer, "wait", actp->au.cs.autoWait);
			break;
		case AA_WAIT_SCAN:
			xmlTextWriterStartElement( writer, (const xmlChar *)"WaitScan");
			putStringAttribute(writer, "name", actp->au.ws.name);
			break;
		default:
			xmlTextWriterStartElement( writer, (const xmlChar *)"ERROR");
		}
		xmlTextWriterEndElement( writer);
	}
	xmlTextWriterEndElement( writer);
}

static void
xmlOutputEvent( xmlTextWriterPtr writer, acqEvent_t *eventp)
{
	int i;
	int min_scan;

	xmlTextWriterStartElement( writer, (const xmlChar *)"event");
	putStringAttribute(writer, "name", eventp->eventName);
	putIntegerAttribute( writer, "id", eventp->eventNum);
	xmlTextWriterStartElement( writer, (const xmlChar *)"flags");
	putBinaryAttribute( writer, "eventID", eventp->putEventId);
	putBinaryAttribute( writer, "absTime", eventp->putAbsTime);
	putBinaryAttribute( writer, "Rel0Time", eventp->putRel0Time);
	putBinaryAttribute( writer, "RelPTime", eventp->putRelPTime);
	putBinaryAttribute( writer, "commentPrefix", eventp->putCommentPrefix);
	xmlTextWriterEndElement( writer);

	min_scan = NUM_EVENT_PV;
	if( eventp->numPvList > min_scan)
		min_scan = eventp->numPvList;
	for( i=0; i < min_scan ; i++)
	{
		acqPv_t *pvp;
		pvp = &eventp->pvList[i];
		if( pvp->pvName == NULL)
			continue;
		xmlTextWriterStartElement(writer, (const xmlChar *)"pv");
		putStringAttribute(writer, "name", pvp->pvName);
		putBinaryAttribute(writer, "disable", pvp->noRecord);
		putBinaryAttribute(writer, "spectrum", pvp->isSpectrum);
		switch( pvp->ready)
		{
		case useCurrent:
			putStringAttribute(writer, "ready", "immediate");
			break;
		case usePvGet:
			putStringAttribute(writer, "ready", "readback");
			break;
		case waitForMonitor:
			putStringAttribute(writer, "ready", "wait");
			break;
		}
		if( pvp->outputFormat && *pvp->outputFormat)
			putStringAttribute(writer, "format", pvp->outputFormat);
		xmlTextWriterEndElement( writer);
	}

	xmlTextWriterEndElement( writer);
}

#if ACQ_VERSION_LT(3,0,0)
static void
xmlOutputFile( xmlTextWriterPtr writer, acqFile_t *filep)
{
	xmlTextWriterStartElement( writer, (const xmlChar *)"file");
	if( filep->dirname)
		putStringAttribute(writer, "path", filep->dirname);
	if( filep->fileTemplate)
		putStringAttribute(writer, "template", filep->fileTemplate);
	putBinaryAttribute(writer, "sequence", filep->appendSeq);
	putBinaryAttribute(writer, "time", filep->appendTime);
	putBinaryAttribute(writer, "datedir", filep->dateDirectory);
	putIntegerAttribute(writer, "seqno", filep->seqno);
	switch( filep->spectrumFileFormat)
	{
	case 0:
		putStringAttribute(writer, "spectrumFormat", "0");
		break;
	case 1:
		putStringAttribute(writer, "spectrumFormat", "text");
		break;
	case 2:
		putStringAttribute(writer, "spectrumFormat", "binary");
		break;
	}
	xmlTextWriterEndElement( writer);
}
#endif


/**
 * ConvertInput:
 * @in: string in a given encoding
 * @encoding: the encoding used
 *
 * Converts @in into UTF-8 for processing with libxml2 APIs
 *
 * Returns the converted UTF-8 string, or NULL in case of error.
 */
static xmlChar *
ConvertInput(const char *in, const char *encoding)
{
    xmlChar *out;
    int ret;
    int size;
    int out_size;
    int temp;
    xmlCharEncodingHandlerPtr handler;

    if (in == 0)
        return 0;

    handler = xmlFindCharEncodingHandler(encoding);

    if (!handler) {
        printf("ConvertInput: no encoding handler found for '%s'\n",
               encoding ? encoding : "");
        return 0;
    }

    size = (int) strlen(in) + 1;
    out_size = size * 2 - 1;
    out = (unsigned char *) xmlMalloc((size_t) out_size);

    if (out != 0) {
        temp = size - 1;
        ret = handler->input(out, &out_size, (const xmlChar *) in, &temp);
        if ((ret < 0) || (temp - size + 1)) {
            if (ret < 0) {
                printf("ConvertInput: conversion wasn't successful.\n");
            } else {
                printf
                    ("ConvertInput: conversion wasn't successful. converted: %i octets.\n",
                     temp);
            }

            xmlFree(out);
            out = 0;
        } else {
            out = (unsigned char *) xmlRealloc(out, out_size + 1);
            out[out_size] = 0;  /*null terminating out */
        }
    } else {
        printf("ConvertInput: no mem\n");
    }

    return out;
}
