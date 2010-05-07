#include "AMAcqScanOutput.h"

#include "acquaman/dacq3_2/OutputHandler/acqFactory.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

static acqOutputHandlerFactoryRegister registerMe( "AMScan", AMAcqScanOutput::new_AMAcqScanOutput);


AMAcqScanOutput::AMAcqScanOutput(){
	handler.startRecord_cb = startRecord;
	handler.endRecord_cb = endRecord;
	handler.nextOutput_cb = nextOutput;
	handler.pvValue_cb = putValue;
	handler.shutdown_cb = shutdown;
	dataDelay_ = true;
}

AMAcqScanOutput::~AMAcqScanOutput(){
}

acqBaseOutput *
AMAcqScanOutput::new_AMAcqScanOutput()
{
		return new AMAcqScanOutput;
}

/// C interface to the Constructor.
/// these are important for dynamic library use!
acqKey_t new_AMAcqScanOutput(void)
{
	AMAcqScanOutput *to = new AMAcqScanOutput();
	return (acqKey_t )to;
}

/*
///
// output the file header prefix.
///
int AMAcqScanOutput::fileHeaderStart()
{
		//
		// output standard headers
		//
		char timebuf[100];
		time_t now = time(0);
		// note: ctime_r() adds a newline to the output
		return sendOutputLine( "<head><!-- CLS Data Acquisition -->\n<!-- start %s -->\n", ctime_r(&now,timebuf));
}

int AMAcqScanOutput::fileHeaderEnd()
{
		return sendOutputLine( "</head>\n<body>\n");
}

///
// output the comment to the file
///
int AMAcqScanOutput::fileHeaderComment()
{
		if( ! getComment().empty() )
		{
				sendOutputLine( "<!-- COMMENT -->\n");
				vector<string> commentLines;
				commentLines = takeApart( getComment(), "\r\n");
				for( unsigned int Ncomment=0; Ncomment < commentLines.size(); Ncomment++)
						sendOutputLine( "  <!-- %s -->\n", commentLines[Ncomment].c_str() );
				sendOutputLine( "<!-- END COMMENT -->\n");
		}
		return 1;
}

///
// give a verbose description of event columns
///
int AMAcqScanOutput::fileHeaderVerboseEvents()
{
		for( acqOutputEvent *ev=first(); ev; ev=next(ev) )
		{
				int column = 1;
				eventPrivate *evpr = (eventPrivate *)ev->private_data;
				sendOutputLine( "<!-- Event: %s ID: %d>\n", ev->eventName, ev->eventNo);
				if( evpr->putEventID)
						sendOutputLine( "<!-- column %d: Event-ID -->\n", column++);
				if( evpr->timeStamp)
						sendOutputLine( "<!-- column %d: Absolute-Time-Stamp -->\n", column++);
				if( evpr->rel0TimeStamp)
						sendOutputLine( "<!-- column %d: Relative-Start-Time -->\n", column++);
				if( evpr->relTimeStamp)
						sendOutputLine( "<!-- column %d: Relative-Last-Event -->\n", column++);


				for(int col=0; col < ev->nColumn; col++)
				{
						acqOutputColumn *colp = &ev->column[col];
						// without the PV structure, there is no connection information. How should this be fixed?
						sendOutputLine( "<!-- column %d: %s %s %s -->\n", column++, colp->description?colp->description:colp->columnName, colp->columnName, colp->valid?"":"NO CONNECTION");

				}
		}
		return 1;
}

///
// give a terse description of event columns
///
int AMAcqScanOutput::fileHeaderTerseEvents()
{
		// put out the highly-verbose section of the header
		// summary header
		for( acqOutputEvent *ev=first(); ev; ev=next(ev) )
		{

				eventPrivate *evpr = (eventPrivate *)ev->private_data;
				sendOutputLine( "<!-- event %d:", ev->eventNo);
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
				sendOutputLine( " -->\n");
		}
		return 1;
}

int AMAcqScanOutput::fileHeaderDescribeEvents()
{
		// description summary
		for( acqOutputEvent *ev=first(); ev; ev=next(ev) )
		{

				eventPrivate *evpr = (eventPrivate *)ev->private_data;
				sendOutputLine( "<!-- event %d:", ev->eventNo);
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
				sendOutputLine( " -->\n");
		}
		return 1;
}
*/

// put out flagged header entries: event ID, timestamps, comment prefix ...
int AMAcqScanOutput::startRecord( acqKey_t key, int eventno)
{
		AMAcqScanOutput *to = (AMAcqScanOutput *)key;
		acqOutputEvent_t *event;
		struct timeval curTime;

//tx		acqTextOutput *to = (acqTextOutput *)key;
//tx		acqOutputEvent_t *event;
		const char *prefix = "";
//tx		struct timeval curTime;

		/*XML STUFF
		//DEBUG(to) printf("startRecord(%p, %d)\n", key, eventno);
		// flag that some output is occuring
		to->outputState = TOH_HAS_CONTENT;
		event = to->find_event_number(eventno);
		if( event == NULL )
				return -1;
		eventPrivate *evpr;
		evpr = (eventPrivate *)event->private_data;
		if( evpr == NULL)
				return -1;

		gettimeofday( &curTime, 0);

		// note: comment prefix is not used for XML output

		to->sendOutputLine( "<event>\n");
		to->outputCol=0;
		if( evpr->putEventID)
		{
				to->sendOutputLine( "<datum n=%d>%d</datum>", to->outputCol++, eventno);
		}
		if( evpr->timeStamp)
		{
				to->sendOutputLine( "<datum n=%d>%.6f</datum>", to->outputCol++, doubleTime(curTime));
		}
		if( evpr->rel0TimeStamp)
		{
				to->sendOutputLine( "<datum n=%d>%.6f</datum>", to->outputCol++, timeDiff(curTime, evpr->startTime) );
		}
		if( evpr->relTimeStamp)
		{
				to->sendOutputLine( "<datum n=%d>%.6f</datum>", to->outputCol++, timeDiff(curTime, evpr->prevTime) );
		}
		evpr->prevTime = curTime;
		END XML STUFF*/

		qDebug() << "NEW READINGS";
		to->dataDelayList_.clear();
		to->dataDelay_ = true;
		qDebug() << "Detectors are " << to->scan_->detectors();
//		return 1;



		//DEBUG(to) printf("startRecord(%p, %d)\n", key, eventno);
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
		return 1;
}

int AMAcqScanOutput::endRecord( acqKey_t key, int eventno)
{
		AMAcqScanOutput *to = (AMAcqScanOutput *)key;

		/*XML STUFF
		//DEBUG(to) printf("endRecord(%p)\n", key);
		to->sendOutputLine( "</event>\n");
		to->acq_flush();
		END XML STUFF*/

//		qDebug() << "DONE READING\n\n";
//		return 1;

//		acqTextOutput *to = (acqTextOutput *)key;

		//DEBUG(to) printf("endRecord(%p)\n", key);
		to->sendOutputLine( "\n");
		to->acq_flush();
		to->needDelimiter = 0;

		return 1;
}
int AMAcqScanOutput::putValue( acqKey_t key, int eventno, int pvno, const void *value, int count)
{
	qDebug() << "putValue has eventno: " << eventno << " pvno: " << pvno << " and count: " << count;
	AMAcqScanOutput *to = (AMAcqScanOutput *)key;
	ScanPVPrivate *pvpr = (ScanPVPrivate *)to->pvInfo[makeuid(eventno, pvno)];
	if(pvpr){
		pvpr->output(key, pvpr->colp->columnType, value, count);
	}


	double dataVal;

	switch( pvpr->colp->columnType)
	{
	case DBF_STRING:
		qDebug() << "It's a string: " << (char *)value;
		break;
	case DBF_ENUM:	// ENUM support is difficult if the value may be passed through multiple
			// processes before being handled. It is the responsibility of the inheriting class
			// to determine how to convert enums to strings, probably with support from the
			// calling program.
	case DBF_SHORT:
		qDebug() << "It's a short: " << *(short *)value;
		dataVal = (double)*(short *)value;
		break;
	case DBF_FLOAT:
		qDebug() << "It's a float: " << *(float *)value;
		dataVal = (double)*(float *)value;
		break;
	case DBF_CHAR:
		qDebug() << "It's a char: " << *(char *)value;
		dataVal = (double)*(char *)value;
		break;
	case DBF_LONG:
		qDebug() << "It's a long: " << *(long *)value;
		dataVal = (double)*(long *)value;
		break;
	case DBF_DOUBLE:
		qDebug() << "It's a double: " << *(double *)value;
		dataVal = *(double *)value;
		break;
	default:
//					handlerSignal(acqBaseOutput_BadType);
		return -1;
	}


	if(!to->dataDelay_){
		qDebug() << "Delaying done, entering " << dataVal << " at " << pvno;
		to->scan_->d_.setLastValue(pvno-1, dataVal);
		qDebug() << "Done with " << dataVal << " at " << pvno;
	}
	else if( (pvno != 0) && (eventno == 1) ){
		qDebug() << "Still delaying for " << dataVal << " at " << pvno;
		to->dataDelayList_[pvno] = dataVal;
		qDebug() << "Done with " << dataVal << " delayed at " << pvno;
	}
	else if( (pvno == 0) && (eventno == 1) ){
		to->dataDelay_ = false;

		qDebug() << "Found ev " << dataVal << " at " << pvno;
		// append a new datapoint to the data tree (supply primary eV value here)
		to->scan_->d_.append(dataVal);	// insert eV
		qDebug() << "Done with ev " << dataVal << " at " << pvno;

		QMap<int, double>::const_iterator i = to->dataDelayList_.constBegin();
		while (i != to->dataDelayList_.constEnd()) {
			qDebug() << "Undelaying " << i.value() << " at " << i.key();
			to->scan_->d_.setLastValue(i.key()-1, i.value());
			qDebug() << "Done undelaying " << i.value() << " at " << i.key();
//			 cout << i.key() << ": " << i.value() << endl;
			++i;
		}
	}


	return 0;

/*
		AMAcqScanOutput *to = (AMAcqScanOutput *)key;
		acqOutputColumn_t *col;
		acqOutputEvent_t *event;
		char result[100];
		pvPrivate *pvpr;
		eventPrivate *evpr;

		//DEBUG(to) printf("putValue( %p, %d, %d, %p, %d)\n", key, eventno, pvno, value, count);
		event = to->find_event_number(eventno);
		if( event == NULL )
				return -1;
		if( pvno < 0 || pvno >= event->nColumn)
				return -1;
		col = &event->column[pvno];
		pvpr = (pvPrivate *)col->private_data;
		evpr = (eventPrivate *)event->private_data;

		//
		// normal value
		//
		if( value == NULL)
		{
				to->sendOutputLine( "<datum n=%d connect=fail></datum>\n", to->outputCol++);
				return 1;
		}
		to->sendOutputLine( "<datum n=%d>", to->outputCol++);
		for(int i=0; i < count; i++)
		{
			qDebug() << "Looping in putValue on count " << count;
				if (to->value_to_string(value, col->columnType, pvpr->format, result, sizeof result) < 0)
				{
						to->sendOutputLine( "<Error/>");
						continue;
				}
				to->sendOutputLine( "%s%s", result,(i<count-1?" ":""));

		}
		to->sendOutputLine( "</datum>\n");
		return 1;
*/
}

int AMAcqScanOutput::shutdown( acqKey_t key)
{
		AMAcqScanOutput *to = (AMAcqScanOutput *)key;
//XML STUFF		to->sendOutputLine( "</body>\n");
		to->acqTextOutput::shutdown(key);
		return 0;
}

//
// set properties for the XML handler
// Currently, XML doesn't have any more properties than the text output handler
//
void AMAcqScanOutput::setProperty( const std::string name , const std::string val)
{
		this->acqTextOutput::setProperty( name, val);
}

int AMAcqScanOutput::ScanPVPrivate::output(acqKey_t key, int dataType, const void *value, int count){
	acqTextOutput *to = (acqTextOutput *)key;
	char result[100];

	for(int i=0; i < count; i++)
	{
		if( to->needDelimiter)
			to->sendOutputLine( "%s", to->delimiter.c_str() );

		if( value == NULL)
		{
			to->sendOutputLine( "<No Connection>");
			to->needDelimiter = true;
			continue;
		}
		qDebug() << "Trying to switch on type";
		switch( dataType)
		{
		case DBF_STRING:
			qDebug() << "It's a string: " << (char *)value;
//					strncpy( result, (char *)value, result_max);
			break;
		case DBF_ENUM:	// ENUM support is difficult if the value may be passed through multiple
				// processes before being handled. It is the responsibility of the inheriting class
				// to determine how to convert enums to strings, probably with support from the
				// calling program.
		case DBF_SHORT:
			qDebug() << "It's a short: " << *(short *)value;
//					snprintf(result, result_max, format?format:"%d", *(short *)value);
			break;
		case DBF_FLOAT:
			qDebug() << "It's a float: " << *(float *)value;
//					snprintf(result, result_max, format?format:"%g", *(float *)value);
			break;
		case DBF_CHAR:
			qDebug() << "It's a char: " << *(char *)value;
//					snprintf(result, result_max, format?format:"%d", *(char *)value);
			break;
		case DBF_LONG:
			qDebug() << "It's a long: " << *(long *)value;
//					snprintf(result, result_max, format?format:"%ld", *(long *)value);
			break;
		case DBF_DOUBLE:
			qDebug() << "It's a double: " << *(double *)value;
//					snprintf(result, result_max, format?format:"%g", *(double *)value);
			break;
		default:
//					handlerSignal(acqBaseOutput_BadType);
			return -1;
		}

		if (to->value_to_string(value, dataType, format, result, sizeof result) < 0)
		{
			to->sendOutputLine( "<No Data>");
			to->needDelimiter = true;
			continue;
		}
		to->sendOutputLine( "%s", result);
		to->needDelimiter = true;

	}
	return 0;
}

void AMAcqScanOutput::getPrivate(int eventID, int pvID){
	int uid = makeuid(eventID, pvID);

	if( pvInfo[uid] == NULL)
		pvInfo[uid] = new ScanPVPrivate;
}
