/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMAcqScanSpectrumOutput.h"

#include "acquaman/dacq3_2/OutputHandler/acqFactory.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

static acqOutputHandlerFactoryRegister registerMe( "AMScanSpectrum", AMAcqScanSpectrumOutput::new_AMAcqScanSpectrumOutput);


AMAcqScanSpectrumOutput::AMAcqScanSpectrumOutput() : acqTextSpectrumOutput()
{
	handler.startRecord_cb = startRecord;
	handler.endRecord_cb = endRecord;
	handler.nextOutput_cb = nextOutput;
	handler.pvValue_cb = putValue;
	dataDelay_ = true;
	lockHash_ = false;
	colNo_ = 0;
	specColNo_ = 0;
}

AMAcqScanSpectrumOutput::~AMAcqScanSpectrumOutput(){
}

acqBaseOutput *
AMAcqScanSpectrumOutput::new_AMAcqScanSpectrumOutput()
{
		return new AMAcqScanSpectrumOutput;
}

void AMAcqScanSpectrumOutput::setScan(AMScan *scan){
	scan_ = scan;
}

void AMAcqScanSpectrumOutput::setScanController(QObject *scanController){
	scanController_ = scanController;
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

	// flag that some output is occuring
	acqTextOutput::startRecord(key, eventno);

	to->dataPackage_.clear();
	to->spectraPackage_.clear();
	to->dataDelayList_.clear();
	to->spectraDelayList_.clear();
	to->dataDelay_ = true;

	to->recordCount++;
	if( to->spectrumSplit == SS_BY_RECORD)
		to->nextSpectrumFile(to->recordCount);

	return 0;
}


int AMAcqScanSpectrumOutput::endRecord( acqKey_t key, int eventno)
{
	AMAcqScanSpectrumOutput *to = (AMAcqScanSpectrumOutput *)key;
	if( (eventno == 1) && !to->lockHash_ )
		to->lockHash_ = true;

	/// \todo Handle spectrum output like this too, fix up the other (unused right now) output handler
	AMAcqEvent *ae = new AMAcqEvent();
	QMap<int, double>::const_iterator i = to->dataPackage_.constBegin();
	while(i != to->dataPackage_.constEnd()){
		ae->dataPackage_.insert(i.key(), i.value());
		i++;
	}
	QMap<int, QList<double> >::const_iterator j = to->spectraPackage_.constBegin();
	while(j != to->spectraPackage_.constEnd()){
		ae->spectraPackage_.insert(j.key(), j.value());
		j++;
	}
	QCoreApplication::postEvent(to->scanController_, ae);
	return acqTextSpectrumOutput::endRecord(key, eventno);

}


int AMAcqScanSpectrumOutput::putValue( acqKey_t key, int eventno, int pvno, const void *value, int count)
{
	AMAcqScanSpectrumOutput *to = (AMAcqScanSpectrumOutput *)key;
	acqTextSpectrumOutput::pvTSPrivate *pvpr = (acqTextSpectrumOutput::pvTSPrivate*)to->pvInfo[makeuid(eventno, pvno)];
	if(pvpr){
		pvpr->output(key, pvpr->colp->columnType, value, count);
	}

	if( (eventno == 1) && (pvno != 0) && !to->lockHash_){
		#warning "David: check if this still needs to be done"
		//if(pvpr->isSpectrum)
		//	to->pvnoToColumn_[pvno] = to->specColNo_++;
		//else
			to->pvnoToColumn_[pvno] = to->colNo_++;
	}

	double dataVal;
	double specMax = 0;
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
			spectraVal.append(dataVal);
			if(dataVal > specMax)
				specMax = dataVal;
			value = (char  *)value + pvpr->colp->dataSize;
		}
	}


	if((eventno == 1) && !pvpr->isSpectrum){
		if( (pvno == 0) && (eventno == 1) )
			to->dataPackage_.insert(0, dataVal);
		else
			to->dataPackage_.insert(to->pvnoToColumn_[pvno]+1, dataVal);
	}
	else if( (eventno == 1) && pvpr->isSpectrum ){
		to->spectraPackage_.insert(to->pvnoToColumn_[pvno]+1, spectraVal);
	}
	/*
	if(!to->dataDelay_){
		if(!pvpr->isSpectrum)
			to->scan_->d_->setLastValue(to->pvnoToColumn_[pvno], dataVal);
		else{
			QStringList myList = to->scan_->d_->deeper(0, to->scan_->d_->count()-1)->yColumnNames();
			for(int x = 0; x < spectraVal.count(); x++){
				to->scan_->d_->deeper(to->pvnoToColumn_[pvno], to->scan_->d_->count()-1)->setValue(0, x, spectraVal[x]);
			}
		}
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
	*/
	return 0;
}
