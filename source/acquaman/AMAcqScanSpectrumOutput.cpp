/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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

#include "acquaman/dacq3_3/OutputHandler/acqFactory.h"

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
		//qDebug() << "In dacq " << j.value();
		ae->spectraPackage_.insert(j.key(), j.value());
		j++;
	}
	QMap<int, double>::const_iterator k = to->extraPackage_.constBegin();
	while(k != to->extraPackage_.constEnd()){
		ae->extraPackage_.insert(k.key(), k.value());
		k++;
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

	if( (eventno == 1) && (pvno != 0) && !to->lockHash_)
			to->pvnoToColumn_[pvno] = to->colNo_++;

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
		qDebug() << "Spectrum of count " << count;
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
			case DBF_LONG:{
				/* NTBA May 8th, 2011 David Chevrier
				   There seems to be an issue with the DBF_LONG being saved as int (size 4)
				   rather than as long (size 8). Probably a 64bit problem.
				*/
				//dataVal = (double)*(long *)value;
				dataVal = (double)*(int *)value;
				break;
			}
			case DBF_DOUBLE:
				dataVal = *(double *)value;
				break;
			default:
				return -1;
			}
			spectraVal.append(dataVal);
			if(dataVal > specMax)
				specMax = dataVal;
			/* NTBA May 8th, 2011 David Chevrier
			   There seems to be an issue with the DBF_LONG being saved as int (size 4)
			   rather than as long (size 8). Probably a 64bit problem.
			 */
			if(pvpr->colp->columnType == DBF_LONG)
				value = (char  *)value + 4;
			else
				value = (char  *)value + pvpr->colp->dataSize;
		}
	}
	if((eventno == 1) && !pvpr->isSpectrum){
		if( (pvno == 0) && (eventno == 1) )
			to->dataPackage_.insert(0, dataVal);
		else
			to->dataPackage_.insert(to->pvnoToColumn_[pvno]+1, dataVal);
	}
	else if( (eventno == 2) && (pvno == 1) && !pvpr->isSpectrum){
		to->extraPackage_.insert(2, dataVal);
	}
	else if( (eventno == 1) && pvpr->isSpectrum ){
		to->spectraPackage_.insert(to->pvnoToColumn_[pvno]+1, spectraVal);
	}
	return 0;
}
