#include "AMAcqScanSpectrumOutput.h"

#include "acquaman/dacq3_2/OutputHandler/acqFactory.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

static acqOutputHandlerFactoryRegister registerMe( "AMScanSpectrum", AMAcqScanSpectrumOutput::new_AMAcqScanSpectrumOutput);


AMAcqScanSpectrumOutput::AMAcqScanSpectrumOutput() : acqTextSpectrumOutput()
{
	handler.startRecord_cb = startRecord;
//	handler.endRecord_cb = endRecord;
	handler.nextOutput_cb = nextOutput;
	handler.pvValue_cb = putValue;
	dataDelay_ = true;

	plotWindow = new MPlotWidget();
	plot = new MPlot();
	plotWindow->setPlot(plot);
	series1 = new MPlotSeriesBasic();
	data1 = new MPlotRealtimeModel();
}

AMAcqScanSpectrumOutput::~AMAcqScanSpectrumOutput(){
}

acqBaseOutput *
AMAcqScanSpectrumOutput::new_AMAcqScanSpectrumOutput()
{
		return new AMAcqScanSpectrumOutput;
}

/// C interface to the Constructor.
/// these are important for dynamic library use!
acqKey_t new_AMAcqScanSpectrumOutput(void)
{
	AMAcqScanSpectrumOutput *to = new AMAcqScanSpectrumOutput();
	return (acqKey_t )to;
}

int AMAcqScanSpectrumOutput::startRecord( acqKey_t key, int eventno)
{
	AMAcqScanSpectrumOutput *to = (AMAcqScanSpectrumOutput *)key;

//	DEBUG(to) printf("startRecord(%p, %d)\n", key, eventno);

	// flag that some output is occuring
	acqTextOutput::startRecord(key, eventno);

	to->dataDelayList_.clear();
	to->spectraDelayList_.clear();
	to->dataDelay_ = true;
	qDebug() << "In startRecord, just cleared lists";

	to->recordCount++;
	if( to->spectrumSplit == SS_BY_RECORD)
		to->nextSpectrumFile(to->recordCount);

	return 0;
}

/*
int AMAcqScanSpectrumOutput::endRecord( acqKey_t key, int eventno)
{
	AMAcqScanSpectrumOutput *to = (AMAcqScanSpectrumOutput *)key;

	acqTextOutput::endRecord( key, eventno);

	if( to->spectrumStream && to->needSpectrumDelimiter )	// only true if there is a spectrum file in text format
	{
		to->sendSpectrumLine( "\n");
		to->spectrumStream->flush();
		to->needSpectrumDelimiter = 0;
	}

	return 0;
}
*/

int AMAcqScanSpectrumOutput::putValue( acqKey_t key, int eventno, int pvno, const void *value, int count)
{
	AMAcqScanSpectrumOutput *to = (AMAcqScanSpectrumOutput *)key;
	//ScanPVPrivate *pvpr = (ScanPVPrivate *)to->pvInfo[makeuid(eventno, pvno)];
	acqTextSpectrumOutput::pvTSPrivate *pvpr = (acqTextSpectrumOutput::pvTSPrivate*)to->pvInfo[makeuid(eventno, pvno)];
	if(pvpr){
		pvpr->output(key, pvpr->colp->columnType, value, count);
	}


	double dataVal;
	QList<double> spectraVal;

	if(!pvpr->isSpectrum){
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
	}
	else{
//		spectraVal = (double *)malloc((count+1) * sizeof(double));
//		spectraVal[0] = count;
		for(int x = 0; x < count; x++){
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
//			spectraVal[x+1] = dataVal;
			spectraVal.append(dataVal);
			value = (char  *)value + pvpr->colp->dataSize;
		}
	}

	qDebug() << "Currently dataDelay is " << to->dataDelay_ << " and pvno is " << pvno << " and eventno is " << eventno;
	if(pvno != 0)
		if(!pvpr->isSpectrum)
			qDebug() << "Looking at column " << to->scan_->d_->yColumnNames().at(pvno-1);
	if(!to->dataDelay_){
		if(!pvpr->isSpectrum)
			to->scan_->d_->setLastValue(pvno-1, dataVal);
		else{
			QStringList myList = to->scan_->d_->deeper(0, to->scan_->d_->count()-1)->yColumnNames();
			qDebug() << "Count for spectrum would be " << spectraVal.count() << " 0th column is " << myList;
			for(int x = 0; x < spectraVal.count(); x++){
////		for(int x = 0; x < 64; x++){
//				to->scan_->d_->deeper(pvno-1, to->scan_->d_->count()-1)->setValue(0, x, spectraVal[x]);
////			to->scan_->d_->deeper(0, to->scan_->d_->count()-1)->setValue(0, x, spectraVal[x]);
				std::cout << spectraVal[x] << " (" << to->scan_->d_->deeper(0, to->scan_->d_->count()-1)->count() << ") ";
				fflush(stdout);
//				qDebug() << "Spectrum immediate setting tCol " << pvno-1 << " tRow " << to->scan_->d_->count()-1 << " deeper index " << x << " and value " << spectraVal[x];
			}
			std::cout << "\nOver\n";
			fflush(stdout);
		}
		if(pvno == 2)
			to->data1->insertPointBack(to->scan_->d_->x(to->scan_->d_->count()-1), dataVal);
	}
	else if( (pvno != 0) && (eventno == 1) ){
		if(pvpr->isSpectrum)
			to->spectraDelayList_[pvno] = spectraVal;
		else
			to->dataDelayList_[pvno] = dataVal;
	}
	else if( (pvno == 0) && (eventno == 1) ){
		to->dataDelay_ = false;

		// append a new datapoint to the data tree (supply primary eV value here)
		to->scan_->d_->append(dataVal);	// insert eV

		QMap<int, double>::const_iterator i = to->dataDelayList_.constBegin();
		while (i != to->dataDelayList_.constEnd()) {
			to->scan_->d_->setLastValue(i.key()-1, i.value());
			if(i.key() == 2)
				to->data1->insertPointBack(dataVal, i.value());
			++i;
		}
		int tCol;
		QList<double> localSpectrum;
		int colCount;
		int tRow;
		QMap<int, QList<double> >::const_iterator dI = to->spectraDelayList_.constBegin();
		while(dI != to->spectraDelayList_.constEnd()){
			tCol = dI.key()-1;
			localSpectrum = dI.value();
			colCount = localSpectrum.count();
			tRow = to->scan_->d_->count()-1;
			for(int x = 0; x < colCount; x++){
				to->scan_->d_->deeper(tCol, tRow)->setValue(0, x, localSpectrum[x]);
				qDebug() << "Spectrum setting tCol " << tCol << " tRow " << tRow << " deeper index " << x << " and value " << localSpectrum[x];
			}
			++dI;
		}
	}
	cout << "Totally done put call\n";
	fflush(stdout);
	return 0;
}
