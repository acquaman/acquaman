
#include "acqTextOutput.h"

#include "acqFactory.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define DEBUG(PTR) if((PTR)->getDebug())

using namespace std;
#include <sstream>

enum fileType { FT_FILE, FT_DIRECTORY, FT_NONE};

#define TRUE 1
#define FALSE 0

static acqOutputHandlerFactoryRegister registerMe( "SimpleText", acqTextOutput::new_acqTextOutput);

///
/// Properties recognized by this handler
///
#define TEXT_PROP_VERBOSE "Text Verbose Header"

/// Only constructor. This over-rides the default handler entries from a base class.
acqTextOutput::acqTextOutput()
	: comment(""), delimiter(", "),
	outputState(TOH_INIT), verbose(0)
{
	handler.start_cb = start;
	handler.pause_cb = pause;
	handler.resume_cb = resume;
	handler.stop_cb = stop;
	handler.shutdown_cb = shutdown;
	handler.pvValue_cb = putValue;
	handler.startRecord_cb = startRecord;
	handler.endRecord_cb = endRecord;
	handler.nextOutput_cb = nextOutput;
	handler.startrepeat_cb = startRepeat;
	handler.comment_cb = userComment;
	handler.pvFlags_cb = pvFlags;
	handler.eventFlags_cb = eventFlags;
	setPropertyList( TEXT_PROP_VERBOSE );
}

acqTextOutput::~acqTextOutput()
{
}

acqBaseOutput *
acqTextOutput::new_acqTextOutput()
{
	return new acqTextOutput();
}

/// C interface to the setComment method.
void
acqTextOutput_setComment(void *key, char *comment)
{
	acqTextOutput *to = (acqTextOutput *)key;
	to->setComment(comment);
}

/// C interface to the setVerbose method.
void
acqTextOutput_setVerbose( void *key, int verbose)
{
	
	acqTextOutput *to = (acqTextOutput *)key;
	to->setVerbose(verbose);
}

/// C interface to the Constructor.
/// these are important for dynamic library use!
acqKey_t new_acqTextOutput(void)
{
	acqTextOutput *to = new acqTextOutput();
	return (acqKey_t )to;
}

/// track the beginning of each pass
int acqTextOutput::startRepeat( acqKey_t key, int passno)
{
	acqTextOutput *to = (acqTextOutput *)key;
	to->curPassNo = passno;

	// build the column pointers: this is only safe if there are no calls to 'pvName' between
	// a 'startRepeat' and 'endRepeat'.
	for( acqOutputEvent_t *evp = to->first(); evp ; evp=to->next(evp) )
	{
		for(int colno=0; colno < evp->nColumn; colno++)
		{
			int uid = makeuid(evp->eventNo, colno);
			if( to->pvInfo.find(uid) != to->pvInfo.end() )
				to->pvInfo[uid]->colp = &evp->column[colno];
		}
	}
	
	return 0;
}

/// start of a run - may be multiple passes
/// convert the flag strings into instances in private structures, and update the private entries
/// for all the text fields.
int acqTextOutput::start( acqKey_t key)
{
	acqTextOutput *to = (acqTextOutput *)key;

	DEBUG(to) printf("acqTextOutput::start(%p)\n", key);
	return 0;
}

int acqTextOutput::eventFlags(acqKey_t key, int eventno, const char *eflags)
{
	acqTextOutput *to = (acqTextOutput *)key;
	acqBaseOutput::default_eventFlags( key, eventno, eflags);
	
	acqOutputEvent_t *aoep = to->find_event_number(eventno);
	if( aoep == NULL)
		return -1;
	
	if( aoep->private_data == NULL)
		aoep->private_data = calloc(1, sizeof (eventPrivate) );

	eventPrivate *evpr = (eventPrivate *)aoep->private_data;
	gettimeofday( &evpr->startTime, 0);
	evpr->prevTime = evpr->startTime;

	vector<string> flags = takeApart(aoep->outputFlags, " \t\r\n");
	for( unsigned int i=0; i < flags.size(); i++)
	{
		if( flags[i] == "putEventId")
			evpr->putEventID = TRUE;
		else if( flags[i] == "absTime")
			evpr->timeStamp = TRUE;
		else if( flags[i] == "relTime")
			evpr->relTimeStamp = TRUE;
		else if( flags[i] == "rel0Time")
			evpr->rel0TimeStamp = TRUE;
		else if( flags[i] == "commentPrefix")
			evpr->commentPrefix = 1;
		// ignore unknown flags: other handlers may use them
	}
	return 0;
	
}

// this is a virtual function - derived classes may build a derived pvPrivate entry
void acqTextOutput::getPrivate(int eventID, int pvID)
{
	int uid = makeuid(eventID, pvID);

	if( pvInfo.find(uid) == pvInfo.end() )
		pvInfo.insert( pair<int, pvPrivate*>(uid, new pvPrivate) );
}

int acqTextOutput::pvFlags( acqKey_t key, int eventno, int pvno, const char *eflags)
{
	acqBaseOutput::default_pvFlags( key, eventno, pvno, eflags);
	acqTextOutput *to = (acqTextOutput *)key;
	to->getPrivate(eventno, pvno);

	int uid = makeuid(eventno, pvno);
	if( to->pvInfo.find(uid) == to->pvInfo.end() )		// should be no-op: getPrivate must have created this!
		return -1;

	vector<string>flags = takeApart(eflags, " \t\r\n");
	for( unsigned int i=0; i < flags.size(); i++)
	{
		if( i+1 < flags.size() )
		{
			to->pvInfo[uid]->pvproperties[flags[i]] = flags[i+1];
			// allow quicker access to the format field.
			if(flags[i] == "format")
				to->pvInfo[uid]->format = strdup(flags[i+1].c_str() );
			i++;
		}
	}	
	return 0;
}

// Commented out because unused to avoid compiler warning (David Chevrier, Aug 25 2011)
int acqTextOutput::pause( acqKey_t /*key*/)
{
	return 0;
}
// Commented out because unused to avoid compiler warning (David Chevrier, Aug 25 2011)
int acqTextOutput::resume( acqKey_t /*key*/)
{
	return 0;
}
// Commented out because unused to avoid compiler warning (David Chevrier, Aug 25 2011)
// end of the run
int acqTextOutput::stop( acqKey_t /*key*/)
{
	return 0;
}
// system is shutting down. perform any necessary cleanup
int acqTextOutput::shutdown( acqKey_t key)
{
	acqTextOutput *to = (acqTextOutput *)key;
	DEBUG(to) printf("shutdown(%p)\n", key);
	to->acq_close();
	to->outputState = TOH_INIT;
	to->pvInfo.clear();
	return 0;
}

///
// output the file header prefix.
///
int acqTextOutput::fileHeaderStart()
{
	//
	// output standard headers
	//
	char timebuf[100];
	time_t now = time(0);
	// note: ctime_r() adds a newline to the output
	sendOutputLine( "# CLS Data Acquisition\n# %s", ctime_r(&now,timebuf));

	return 1;
}

///
// indicate the end of the header
///
int acqTextOutput::fileHeaderEnd()
{
	return 0;
}

///
// output the comment to the file
///
int acqTextOutput::fileHeaderComment()
{
	if( ! comment.empty() )
	{
		sendOutputLine( "# COMMENT\n");
		vector<string> commentLines;
		commentLines = takeApart( comment, "\r\n");
		for( unsigned int Ncomment=0; Ncomment < commentLines.size(); Ncomment++)
			sendOutputLine( "# %s\n", commentLines[Ncomment].c_str() );
		sendOutputLine( "# END COMMENT\n");
	}
	return 0;
}

///
// give a verbose description of event columns
///
int acqTextOutput::fileHeaderVerboseEvents()
{
	for( acqOutputEvent *ev=first(); ev; ev=next(ev) )
	{
		int column = 1;
		eventPrivate *evpr = (eventPrivate *)ev->private_data;
		sendOutputLine( "# Event: %s\n# ID: %d\n", ev->eventName, ev->eventNo);
		if( evpr == NULL)
			continue;
		if( evpr->putEventID)
			sendOutputLine( "# column %d: Event-ID\n", column++);
		if( evpr->timeStamp)
			sendOutputLine( "# column %d: Absolute-Time-Stamp\n", column++);
		if( evpr->rel0TimeStamp)
			sendOutputLine( "# column %d: Relative-Start-Time\n", column++);
		if( evpr->relTimeStamp)
			sendOutputLine( "# column %d: Relative-Last-Event\n", column++);
		

		for(int col=0; col < ev->nColumn; col++)
		{
			acqOutputColumn *colp = &ev->column[col];
			sendOutputLine( "# column %d: %s %s %s\n", column++, colp->description?colp->description:colp->columnName, colp->columnName, colp->valid?"":"NO CONNECTION");
			
		}
	}
	return 0;
}

///
// give a terse description of event columns
///
int acqTextOutput::fileHeaderTerseEvents()
{
	// put out the highly-verbose section of the header
	// summary header
	for( acqOutputEvent *ev=first(); ev; ev=next(ev) )
	{
		eventPrivate *evpr = (eventPrivate *)ev->private_data;
		if( evpr == NULL)
			continue;
		sendOutputLine( "#(%d)", ev->eventNo);
		if( evpr->putEventID)
			sendOutputLine( " Event-ID");
		if( evpr->timeStamp)
			sendOutputLine( " Absolute-Time-Stamp");
		if( evpr->rel0TimeStamp)
			sendOutputLine( " Relative-Start-Time");
		if( evpr->relTimeStamp)
			sendOutputLine( " Relative-Last-Event");
		for(int col=0; col < ev->nColumn; col++)
		{
			acqOutputColumn *colp = &ev->column[col];
			sendOutputLine( " %s", colp->columnName);
		}
		sendOutputLine( "\n");
	}
	return 0;
}

int acqTextOutput::fileHeaderDescribeEvents()
{
	// description summary
	for( acqOutputEvent *ev=first(); ev; ev=next(ev) )
	{
		eventPrivate *evpr = (eventPrivate *)ev->private_data;
		if( evpr == NULL)
			continue;
		sendOutputLine( "#(%d)", ev->eventNo);
		if( evpr->putEventID)
			sendOutputLine( " Event-ID");
		if( evpr->timeStamp)
			sendOutputLine( " Absolute-Time-Stamp");
		if( evpr->rel0TimeStamp)
			sendOutputLine( " Relative-Start-Time");
		if( evpr->relTimeStamp)
			sendOutputLine( " Relative-Last-Event");
		for(int col=0; col < ev->nColumn; col++)
		{
			acqOutputColumn *colp = &ev->column[col];
			sendOutputLine( " \"%s\"", (colp->description&&colp->description[0])?colp->description:colp->columnName);
		}
		sendOutputLine( "\n");
	}
	return 0;
}

//
// Generate the next output file
// generate all output file names.
//
int acqTextOutput::nextOutput( acqKey_t key)
{

	acqTextOutput *to = (acqTextOutput *)key;

	DEBUG(to) printf("acqTextOutput::nextOutput\n");
	if( to->outputState == TOH_EMPTY)
			return 0;		// OK, redundant call

	to->acq_next();

	to->outputState = TOH_EMPTY;

	to->fileHeaderStart();
	to->fileHeaderComment();

	if( to->verbose)
	{
		to->fileHeaderVerboseEvents();
	}
	to->fileHeaderTerseEvents();
	to->fileHeaderDescribeEvents();
	to->fileHeaderEnd();
	
	to->acq_flush();
	return 0;
}

// put out flagged header entries: event ID, timestamps, comment prefix ...
int acqTextOutput::startRecord( acqKey_t key, int eventno)
{
	acqTextOutput *to = (acqTextOutput *)key;
	acqOutputEvent_t *event;
	const char *prefix = "";
	struct timeval curTime;

	DEBUG(to) printf("startRecord(%p, %d)\n", key, eventno);
	// flag that some output is occuring
	to->outputState = TOH_HAS_CONTENT;
	event = to->find_event_number(eventno);
	if( event == NULL || ! to->isReady() )
		return -1;
	eventPrivate *evpr;
	evpr = (eventPrivate *)event->private_data;
	if( evpr == NULL)
		return 0;

	gettimeofday( &curTime, 0);

	if( evpr->commentPrefix)
	{
		to->sendOutputLine( "# ");
	}
	if( evpr->putEventID)
	{
		to->sendOutputLine( "%s%d", prefix, eventno);
		prefix = to->delimiter.c_str();
	}
	if( evpr->timeStamp)
	{
		to->sendOutputLine( "%s%.6f", prefix, doubleTime(curTime));
		prefix = to->delimiter.c_str();
	}
	if( evpr->rel0TimeStamp)
	{
		to->sendOutputLine( "%s%.6f", prefix, timeDiff(curTime, evpr->startTime) );
		prefix = to->delimiter.c_str();
	}
	if( evpr->relTimeStamp)
	{
		to->sendOutputLine( "%s%.6f", prefix, timeDiff(curTime, evpr->prevTime) );
		prefix = to->delimiter.c_str();
	}
	evpr->prevTime = curTime;
	if( *prefix)
		to->needDelimiter = TRUE;
	return 0;
}

// Commented out because unused to avoid compiler warning (David Chevrier, Aug 25 2011)
int acqTextOutput::endRecord( acqKey_t key, int /*eventno*/)
{
	acqTextOutput *to = (acqTextOutput *)key;

	DEBUG(to) printf("endRecord(%p)\n", key);
	to->sendOutputLine( "\n");
	to->acq_flush();
	to->needDelimiter = 0;
	
	return 0;
}
string
acqTextOutput::default_stream()
{
	return "File";
}

int acqTextOutput::putValue( acqKey_t key, int eventno, int pvno, const void *value, int count)
{
	acqTextOutput *to = (acqTextOutput *)key;
	int uid = makeuid(eventno, pvno);
	if( to->pvInfo.find(uid) == to->pvInfo.end() )
		return 0;

	pvPrivate *pvpr = (pvPrivate *)to->pvInfo[makeuid(eventno, pvno)];
	if(pvpr)
		pvpr->output(key, pvpr->colp->columnType, value, count);
	return 0;
}

// virtual function for "normal" text output
int acqTextOutput::pvPrivate::output( acqKey_t key, int dataType, const void *value, int count)
{
	acqTextOutput *to = (acqTextOutput *)key;
	char result[100];
	
	DEBUG(to) printf("output( %p, %d, %p, %d)\n", key, dataType, value, count);

	for(int i=0; i < count; i++)
	{
		if( to->needDelimiter)
			to->sendOutputLine( "%s", to->delimiter.c_str() );
			
		if( value == NULL)
		{
			to->sendOutputLine( "<No Connection>");
			to->needDelimiter = TRUE;
			continue;
		}
		if (to->value_to_string(value, dataType, format, result, sizeof result) < 0)
		{
			to->sendOutputLine( "<No Data>");
			to->needDelimiter = TRUE;
			continue;
		}
		to->sendOutputLine( "%s", result);
		to->needDelimiter = TRUE;

	}
	return 0;
}

// Output a string to the text file. send signal notification for errors
int
acqTextOutput::sendOutputLine( const char *format, ...)
{
	va_list argPtr;
	int err;
	char buffer[512];

	if( format == NULL || !isReady() )
	{
		handlerSignal( acqTextOutput_WriteError);
		return -1;
	}

	va_start( argPtr, format);

	vsprintf(buffer, format, argPtr);

	err = acq_write( buffer, strlen(buffer) );
	if( err < 0)
		handlerSignal( acqTextOutput_WriteError);

	va_end(argPtr);
	return err;
}


//
// set the comment string.
//
int
acqTextOutput::userComment( acqKey_t key, const char * newComment)
{
	acqTextOutput *to = (acqTextOutput *)key;
	to->setComment( newComment);
	return 0;
}

// 
///
// Property manipulation.
// If a property is not in this list, it is still assigned in the base property list.
// It is recommended that any function derived from acqTextOutput should
// override setProperty if it has its own property list, and explicitly call
// acqTextOutput::setProperty for inherited properties.
///
void acqTextOutput::setProperty(const string name, const string value)
{
	if( name == TEXT_PROP_VERBOSE)
	{
		verbose = atoi( value.c_str() );
	}

	setPropertyValue(name, value);
}

//
// get time difference as a double. Note that this code anticipates both
// the tv_sec and tv_usec as unsigned.
//
double acqTextOutput::timeDiff( const timeval &first, const timeval &second)
{
	double diff;
	if( first.tv_sec >= second.tv_sec)
		diff = first.tv_sec - second.tv_sec;
	else
		diff = -(double)(second.tv_sec - first.tv_sec);
	if( first.tv_usec >= second.tv_usec)
		diff += (first.tv_usec-second.tv_usec)/1000000.0;
	else
		diff -=  (second.tv_usec-first.tv_usec)/1000000.0;
	return diff;
}
//
// return a time as a double
//
double acqTextOutput::doubleTime( const timeval &intime)
{
	double tval;

	tval = intime.tv_sec;
	tval += intime.tv_usec/1000000.0;
	return tval;
}
