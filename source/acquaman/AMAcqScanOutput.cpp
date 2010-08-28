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

void AMAcqScanOutput::setScan(AMScan *scan){
	scan_ = scan;
}

/// C interface to the Constructor.
/// these are important for dynamic library use!
acqKey_t new_AMAcqScanOutput(void)
{
	AMAcqScanOutput *to = new AMAcqScanOutput();
	return (acqKey_t )to;
}


// put out flagged header entries: event ID, timestamps, comment prefix ...
int AMAcqScanOutput::startRecord( acqKey_t key, int eventno)
{
	AMAcqScanOutput *to = (AMAcqScanOutput *)key;
	acqOutputEvent_t *event;
	struct timeval curTime;

	const char *prefix = "";


	to->dataDelayList_.clear();
	to->dataDelay_ = true;

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
	Q_UNUSED(eventno)

	AMAcqScanOutput *to = (AMAcqScanOutput *)key;

	//DEBUG(to) printf("endRecord(%p)\n", key);
	to->sendOutputLine( "\n");
	to->acq_flush();
	to->needDelimiter = 0;

	return 1;
}
int AMAcqScanOutput::putValue( acqKey_t key, int eventno, int pvno, const void *value, int count)
{
	AMAcqScanOutput *to = (AMAcqScanOutput *)key;
	ScanPVPrivate *pvpr = (ScanPVPrivate *)to->pvInfo[makeuid(eventno, pvno)];
	if(pvpr){
		pvpr->output(key, pvpr->colp->columnType, value, count);
	}


	double dataVal;

	switch( pvpr->colp->columnType)
	{
	case DBF_STRING:
		break;
	case DBF_ENUM:	// ENUM support is difficult if the value may be passed through multiple
		// processes before being handled. It is the responsibility of the inheriting class
		// to determine how to convert enums to strings, probably with support from the
		// calling program.
		break;
	case DBF_SHORT:
		dataVal = (double)*(short *)value;
		break;
	case DBF_FLOAT:
		dataVal = (double)*(float *)value;
		break;
	case DBF_CHAR:
		dataVal = (double)*(char *)value;
		break;
	case DBF_LONG:
		dataVal = (double)*(long *)value;
		break;
	case DBF_DOUBLE:
		dataVal = *(double *)value;
		break;
	default:
		return -1;
	}


	if(!to->dataDelay_){
		to->scan_->d_->setLastValue(pvno-1, dataVal);
	}
	else if( (pvno != 0) && (eventno == 1) ){
		to->dataDelayList_[pvno] = dataVal;
	}
	else if( (pvno == 0) && (eventno == 1) ){
		to->dataDelay_ = false;

		// append a new datapoint to the data tree (supply primary eV value here)
		to->scan_->d_->append(dataVal);	// insert eV

		QMap<int, double>::const_iterator i = to->dataDelayList_.constBegin();
		while (i != to->dataDelayList_.constEnd()) {
			to->scan_->d_->setLastValue(i.key()-1, i.value());
			++i;
		}
	}


	return 0;
}

int AMAcqScanOutput::shutdown( acqKey_t key)
{
	AMAcqScanOutput *to = (AMAcqScanOutput *)key;
	to->acqTextOutput::shutdown(key);
	return 0;
}

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
