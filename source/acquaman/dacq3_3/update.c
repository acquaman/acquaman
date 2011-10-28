/* $Header: epicsConnectApp/src/update.c 1.4 2008/12/03 14:30:44CST Glen Wright (wrightg) Exp  $
 * Copyright Canadian Light Source, Inc. All rights reserved.
 *
 * Description:
 */

#include "epicsConnect.h"
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* simple extraction of number of declared elements in
 * an array.
 */
#define NELEMENT(x) ( sizeof x / sizeof x[0])


static void val_to_string_precision(Channel *, char *, int, int, int, char *format);

void
val_to_string(Channel *chan, char *result, int result_size, int idx)
{
	if( result == NULL)
		return;
	if( chan == NULL)
	{
		result[0] = '\0';
		return;
	}
	
	val_to_string_precision(chan, result, result_size, idx, chan->cg.prec, NULL);
}

void
val_to_string_fmt(Channel *chan, char *result, int result_size, int idx, char *fmt)
{
	if( result == NULL)
		return;
	if( chan == NULL)
	{
		result[0] = '\0';
		return;
	}
	
	/* check if this has a at least one usable format directive */
	if( fmt && index( fmt, '%'))
		val_to_string_precision(chan, result, result_size, idx, chan->cg.prec, fmt);
	else
		val_to_string_precision(chan, result, result_size, idx, chan->cg.prec, NULL);
}

void
con_val_to_string( Connector *conp, char *result, int result_size, int idx)
{
	Channel *chan;
	int precision;
	char *format = NULL;

	if( result == NULL)
		return;
	if( !conp || !conp-> chan)
	{
		result[0] = '\0';
		return;
	}
	chan = conp->chan->chan;
	precision = chan->cg.prec;
	if( conp->useCG)
		precision = conp->cg.prec;
	val_to_string_precision(chan, result, result_size, idx, precision, format);
}

void
con_val_to_string_fmt( Connector *conp, char *result, int result_size, int idx, char *format)
{
	Channel *chan;
	int precision;

	if( result == NULL)
		return;

	if( !conp || !conp-> chan || !conp->chan->chan)
	{
		result[0] = '\0';
		return;
	}
	chan = conp->chan->chan;
	precision = chan->cg.prec;
	if( conp->useCG)
		precision = conp->cg.prec;
	/* check if this has a at least one usable format directive */
	if( format && index( format, '%'))
		val_to_string_precision(chan, result, result_size, idx, precision, format);
	else
		con_val_to_string( conp, result, result_size, idx);
}

/*
 * called internally only: both chan and result must have been checked to be non-null.
 */
static void
val_to_string_precision(Channel *chan, char *result, int result_size, int idx, int precision, char *format)
{
	switch(chan->chan_type)
	{
	case DBF_STRING:
		if( format == NULL)
			strncpy( result, &((char *)(chan->dataval))[MAX_STRING_SIZE*idx], result_size);
		else
			snprintf( result, result_size, format, &((char *)(chan->dataval))[MAX_STRING_SIZE*idx], precision);
		break;
	case DBF_SHORT:
		if( format == NULL)
			format = "%d";
		snprintf(result, result_size, format, ((short *) chan->dataval)[idx] );
		break;
	case DBF_FLOAT:
		if ( format == NULL)
			format = "%1$.*2$f";

		snprintf(result, result_size, format, ((float *) chan->dataval)[idx], precision );
		break;
	case DBF_ENUM:
		{
			int i;
			i = ((short *) chan->dataval)[idx] ;
			if( i >= chan->cg.lopr && i <= chan->cg.hopr )
			{
				if( chan->cg.states[i] && chan->cg.states[i][0])
					strncpy( result, chan->cg.states[i], result_size) ;
				else
					snprintf( result, result_size, "%d", ((short*)chan->dataval)[idx] );
			}
			else
				strncpy( result, "OUT_OF_RANGE", result_size);
		}
		break;
	case DBF_CHAR:
		if( format == NULL)
			format = "%d";
		snprintf(result, result_size, format, ((char *) chan->dataval)[idx] );
		break;
	case DBF_LONG:
		if( format == NULL)
			format = "%ld";
		snprintf(result, result_size, format, ((long *) chan->dataval)[idx] );
		break;
	case DBF_DOUBLE:
		if ( format == NULL)
			format = "%1$.*2$f";

		snprintf(result, result_size, format, ((double *) chan->dataval)[idx], precision );
		break;
	default:
		snprintf(result, result_size, "type %d unknown", (int)chan->chan_type);
		break;
	}
}

/*
 * simple conversion to int of the channel's value
 */
int
val_to_int( Channel *chan, int index)
{
	int result;

	if( chan == NULL || index >= chan->maxelement)
		return 0.0;

	switch( chan->chan_type)
	{
	case DBF_STRING:
		result = atoi( chan->dataval);
		break;
	case DBF_SHORT:
		result = ((short *) chan->dataval)[index];
		break;
	case DBF_FLOAT:
		result = ((float *) chan->dataval)[index];
		break;
	case DBF_ENUM:
		result = ((short *) chan->dataval)[index];
		break;
	case DBF_CHAR:
		result = ((char *) chan->dataval)[index];
		break;
	case DBF_LONG:
		result = ((long *) chan->dataval)[index];
		break;
	case DBF_DOUBLE:
		result = ((double *) chan->dataval)[index];
		break;
	default:
		result = 0;
		break;
	}

	return result;
}
/*
 * simple conversion to double of the channel's value
 */
double
val_to_double( Channel *chan, int index)
{
	double result;

	if( chan == NULL || index >= chan->maxelement)
		return 0.0;

	switch( chan->chan_type)
	{
	case DBF_STRING:
		result = atof( chan->dataval);
		break;
	case DBF_SHORT:
		result = ((short *) chan->dataval)[index];
		break;
	case DBF_FLOAT:
		result = ((float *) chan->dataval)[index];
		break;
	case DBF_ENUM:
		result = ((short *) chan->dataval)[index];
		break;
	case DBF_CHAR:
		result = ((char *) chan->dataval)[index];
		break;
	case DBF_LONG:
		result = ((long *) chan->dataval)[index];
		break;
	case DBF_DOUBLE:
		result = ((double *) chan->dataval)[index];
		break;
	default:
		result = 0;
		break;
	}

	return result;
}

/*
 * given an integer value, convert the type and send it out
 */
int
channel_put_int(Channel *chan, int val)
{
	if( chan == NULL || chan->dataval == NULL)
		return -1;

	switch(chan->chan_type)
	{
	case DBF_STRING:
		snprintf(chan->dataval, 40, "%d", val);
		break;
	case DBF_SHORT:
		*(short *) chan->dataval = val;
		break;
	case DBF_FLOAT:
		*(float *) chan->dataval = val;
		break;
	case DBF_ENUM:
		*(short *) chan->dataval = val;
		break;
	case DBF_CHAR:
		*(char *) chan->dataval = val;
		break;
	case DBF_LONG:
		*(long *) chan->dataval = val;
		break;
	case DBF_DOUBLE:
		*(double *) chan->dataval = val;
		break;
	default:
		return -1;
	}

	if( channel_put( chan, 1) >= 0)
		return 0;
	return -1;
}

/*
 * given a double value, convert the type and send it out
 */
int
channel_put_double(Channel *chan, double val)
{
	if( chan == NULL || chan->dataval == NULL)
		return -1;

	if( double_to_val( chan, val, 0) >= 0)
	{
		if( channel_put( chan, 1) >= 0)
			return 0;
	}
	return -2;
}

/*
 * given a double value, convert the type, send it out, and set callback
 */
int
channel_put_double_callback(Channel *chan, double val, CHANNEL_USERFUNC func, void *arg)
{
	if( chan == NULL || chan->dataval == NULL)
		return -1;

	if( double_to_val( chan, val, 0) >= 0)
	{
		if( channel_put_callback( chan, 1, func, arg) >= 0)
			return 0;
	}
	return -1;
}

/*
 * given a string value, convert the type and send it out
 */
int
channel_put_string(Channel *chan, const char * val)
{
	if( string_to_val(chan, val, 0) >= 0)
	{
		channel_put( chan, 1);
		return 0;
	}
	return -1;

}
int
string_to_val(Channel *chan, const char *val, int idx)
{
	char *eptr = NULL;
	void *dataptr = NULL;
	char mydatastorage[40];		/* string size - max # bytes for a single element */
	void *mydata = &mydatastorage;
	int dataSize=0;

	if( chan == NULL || chan->dataval == NULL || val == NULL || idx >= chan->maxelement)
		return -1;

	switch(chan->chan_type)
	{
	case DBF_STRING:
		strncpy((char *)mydata,  val, 40);
		dataptr = chan->dataval;
		dataSize = 40;
		break;
	case DBF_SHORT:
		*(short *) mydata = strtol(val, &eptr, 10);
		dataptr= &((short *) chan->dataval)[idx];
		dataSize = sizeof (short);
		break;
	case DBF_FLOAT:
		*(float *) mydata = strtod(val, &eptr);
		dataptr = &((float *) chan->dataval)[idx];
		dataSize = sizeof (float);
		break;
	case DBF_ENUM:
		{
		int i;
		/* check for a case-sensitive match */
		for(i=chan->cg.lopr; i <= chan->cg.hopr; i++)
			if(chan->cg.states[i] && strcmp( val, chan->cg.states[i]) == 0)
				break;
		if( i <= chan->cg.hopr)
		{
			((short *) chan->dataval)[idx] = i;
			break;
		}
		/* check for a case-insensitive match */
		for(i=chan->cg.lopr; i <= chan->cg.hopr; i++)
			if(chan->cg.states[i] && strcasecmp( val, chan->cg.states[i]) == 0)
				break;
		/* check for a numeric string match */
		if( i > chan->cg.hopr)
		{
			i = strtol( val, &eptr, 10);
			if( i < chan->cg.lopr || i > chan->cg.hopr)
				return -1;
		}
		*(short *) mydata = i;
		 dataptr = &((short *) chan->dataval)[idx];
		dataSize = sizeof (short);
		}
		break;
	case DBF_CHAR:
		*(char *) mydata = val[0];
		dataptr = &((char *) chan->dataval)[idx];
		dataSize = sizeof (char);
		break;
	case DBF_LONG:
		*(long *) mydata = strtol(val, &eptr, 10);
		dataptr = &((long *) chan->dataval)[idx];
		dataSize = sizeof (long);
		break;
	case DBF_DOUBLE:
		*(double *) mydata = strtod(val, &eptr);
		dataptr = &((double *) chan->dataval)[idx];
		dataSize = sizeof (double);
		break;
	default:
		return -1;
	}

	/*
	 * error: type conversion, but EOS or blank not encountered to end conversion
	 */
	if( eptr && *eptr && *eptr != ' ')
		return -2;
	if( dataSize <= 0)
		return -3;
	memcpy( (unsigned char *)dataptr, (unsigned char *)mydata, dataSize);
	return 0;
}

int
double_to_val( Channel *chan, double val, int idx)
{
	if( chan == NULL || chan->dataval == NULL || idx >= chan->maxelement)
		return -1;

	switch(chan->chan_type)
	{
	case DBF_STRING:
		snprintf( chan->dataval, 40, "%.*f", chan->cg.prec, val);
		break;
	case DBF_SHORT:
		((short *) chan->dataval)[idx] = val;
		break;
	case DBF_FLOAT:
		((float *) chan->dataval)[idx] = val;
		break;
	case DBF_ENUM:
		((short *) chan->dataval)[idx] = val;
		break;
	case DBF_CHAR:
		((char *) chan->dataval)[idx] = val;
		break;
	case DBF_LONG:
		((long *) chan->dataval)[idx] = val;
		break;
	case DBF_DOUBLE:
		((double *) chan->dataval)[idx] = val;
		break;
	default:
		return -1;
	}

	return 0;
}

/* 
 * $Log: epicsConnectApp/src/update.c  $
 * Revision 1.4 2008/12/03 14:30:44CST Glen Wright (wrightg) 
 * 
 * Revision 1.3 2008/11/28 11:09:26CST Glen Wright (wrightg) 
 * 
 * Revision 1.2 2008/07/16 16:50:07CST Glen Wright (wrightg) 
 * 
 * Revision 1.1 2008/05/08 10:19:21CST David Beauregard (beaured0) 
 * Initial revision
 * Member added to project e:/MKS_Home/archive/cs/epics_local/epics_tools/epicsConnect/project.pj
 * Revision 1.4 2007/05/19 12:37:49CST Elder Matias (matiase) 
 * Added header/log
 */
