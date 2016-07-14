/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "AMCDFDataStore.h"

#include <QTemporaryFile>
#include <QStringBuilder>
#include <QDir>
#include "util/AMErrorMonitor.h"
#include "cdf.h"

AMCDFDataStore::AMCDFDataStore(QObject* parent) : AMDataStore(parent) {

	Q_UNUSED(TT2000NULL);

	cdfId_ = 0;
	readOnly_ = false;
	cdfFilePath_ = tempFileName("amData");
	fileIsTemporary_ = true;

	CDFstatus s = CDFcreateCDF(cdfFilePath_.toAscii().data(), &cdfId_);
	if(s != CDF_OK) {
		AMErrorMon::report(0, AMErrorReport::Serious, -101, "AMCDFDataStore: Could not create temporary file for CDF data store. Please report this bug to the Acquaman developers.");
	}

	else {
		// create standard attributes for our self-descriptive CDF layout.
		if(!createAttributes()) {
			AMErrorMon::debug(0, -1001, "AMCDFDataStore: Could not create attributes within new CDF file.");
		}
	}
}

AMCDFDataStore::AMCDFDataStore(const QString &newFilePath, bool isTemporary, QObject* parent) : AMDataStore(parent) {
	cdfId_ = 0;
	readOnly_ = false;

	if(!newFilePath.endsWith(".cdf")) {
		AMErrorMon::report(0, AMErrorReport::Serious, -100, QString("AMCDFDataStore: Could not create file '%1' for the CDF data store: The file name must end in '.cdf'.  The CDF data store is now invalid.").arg(newFilePath));
		cdfFilePath_ = QString();
		fileIsTemporary_ = false;
	}

	CDFstatus s = CDFcreateCDF(newFilePath.toAscii().data(), &cdfId_);
	if(s != CDF_OK) {
		AMErrorMon::report(0, AMErrorReport::Serious, -101, QString("AMCDFDataStore: Could not create file '%1' for the CDF data store. Maybe the file exists already? The CDF data store is now invalid.").arg(newFilePath));
		cdfFilePath_ = QString();
		fileIsTemporary_ = false;
	}
	else {
		cdfFilePath_ = newFilePath;
		fileIsTemporary_ = isTemporary;
		// create standard attributes for our self-descriptive CDF layout.
		if(!createAttributes()) {
			AMErrorMon::debug(0, -1001, "AMCDFDataStore: Could not create attributes within new CDF file.");
		}
	}
}

AMCDFDataStore::AMCDFDataStore(const QString &existingFilePath, bool createTemporaryCopy, bool setReadOnly, QObject* parent) : AMDataStore(parent) {
	cdfId_ = 0;
	readOnly_ = false;
	cdfFilePath_ = QString();
	fileIsTemporary_ = false;

	if(!initializeFromExistingCDF(existingFilePath, createTemporaryCopy, setReadOnly)) {
		AMErrorMon::report(0, AMErrorReport::Serious, -101, QString("AMCDFDataStore: Could not open file '%1' for the CDF data store. It's possible that the file does not exist or is corrupted. The CDF data store is now invalid.").arg(existingFilePath));
	}
}

AMCDFDataStore::~AMCDFDataStore() {
	if(cdfId_) {
		CDFcloseCDF(cdfId_);

		if(fileIsTemporary_)
			QFile::remove(cdfFilePath_);
	}
}

QString AMCDFDataStore::tempFileName(const QString& basename) const
{
	QString rv = QDir::tempPath() % "/" % basename;

	QFileInfo fi;
	do {
		fi.setFile(rv % QString(".%1.cdf").arg(int(qrand() % 1000000), 6, 10, QChar('0')));
	} while(fi.exists());

	return fi.filePath();
}

bool AMCDFDataStore::addMeasurement(const AMMeasurementInfo &measurementDetails)
{
	if(readOnly_) {
		AMErrorMon::debug(0, -4000, "AMCDFDataStore: Modifications are not allowed in read-only mode.");
		return false;
	}
	// already a measurement with this name?
	for(int i=measurements_.count()-1; i>=0; i--)
		if(measurements_.at(i).name == measurementDetails.name)
			return false;

	// create a CDF variable for this measurement:
	long totalScanPoints = axes_.count() ? scanSize_.product() : 1;	// if no scan axes yet, just have one record for the null scan space.
	if(!createVarForMeasurement(measurementDetails, totalScanPoints))
		return false;

	// add this measurement to our registry
	measurements_.append(measurementDetails);

	// update attributes to describe this measurement in the CDF.
	updateAttributesForMeasurement(measurements_.count()-1);

	return true;
}

bool AMCDFDataStore::createVarForMeasurement(const AMMeasurementInfo &mi, long numInitialRecords)
{
	// create CDF variable:
	long varNum;
	AMnDIndex dimSizes(mi.size());
	AMnDIndex dimVariances(dimSizes.rank(), AMnDIndex::DoInit, VARY);

	QByteArray variableName = mi.name.toAscii();
	CDFstatus s = CDFcreatezVar(cdfId_, variableName.data(), CDF_DOUBLE, 1L, long(mi.rank()), dimSizes.constData(), VARY, dimVariances.constData(), &varNum);
	if(s < CDF_OK) {
		AMErrorMon::debug(0, -102, QString("AMCDFDataStore: Could not create CDF variable for measurement '%1'. The CDF error was: %2.").arg(mi.name).arg(s));
		return false;
	}

	measurementVarNums_ << varNum;

	// set pad value
	double padValue = AMNUMBER_INVALID_FLOATINGPOINT;
	s = CDFsetzVarPadValue(cdfId_, varNum, &padValue);
	if(s != CDF_OK)
		AMErrorMon::debug(0, -102, QString("AMCDFDataStore: Could not set CDF pad value for measurement '%1'.").arg(mi.name));


	// create initial records: 1 if the scan axes are still empty (for null scan space), or the total number of points in scan space.
	if(numInitialRecords > 0) {
		s = CDFsetzVarInitialRecs(cdfId_, varNum, numInitialRecords);
		if(s != CDF_OK)
			AMErrorMon::debug(0, -102, QString("AMCDFDataStore: Could not allocate space for measurement '%1'. CDF error was: %2.").arg(mi.name).arg(s)); // not a critical error... We have already created the variable.
	}

	return true;
}

int AMCDFDataStore::idOfMeasurement(const QString &measurementName) const
{
	for(int i=measurements_.count()-1; i>=0; i--)
		if(measurements_.at(i).name == measurementName)
			return i;
	return -1;
}

bool AMCDFDataStore::addScanAxis(const AMAxisInfo &axisDetails)
{
	if(readOnly_) {
		AMErrorMon::debug(0, -4001, "AMCDFDataStore: Modifications are not allowed in read-only mode.");
		return false;
	}

	// axis already exists with this name... Not allowed.
	for(int i=axes_.count()-1; i>=0; --i)
		if(axes_.at(i).name == axisDetails.name) {
			AMErrorMon::debug(0, -721, QString("AMCDFDataStore: Could not add this scan axis to the data store; an axis with the name %1 already exists.").arg(axisDetails.name));
			return false;
		}


	AMAxisInfo axisInfo = axisDetails;
	if(axes_.count() == 0) {	// if this is the first axis to be added...
		axisInfo.size = 0;
	}

	else {	// there are axes already.
		// check 1: the size of this axis must not be 0, they must provide a final size
		if(axisInfo.size < 1) {
			AMErrorMon::debug(0, -722, QString("AMCDFDataStore: Could not add a scan axis '%1' with a size of 0 to the data store; all axes except for the first must specify their final size.").arg(axisInfo.name));
			return false;
		}

		// check 2: there cannot be rows already in the data store.
		if(axes_.at(0).size != 0) {
			AMErrorMon::debug(0, -723, QString("AMCDFDataStore: Could not add scan axis '%1' to the data store because there are already rows/scan points present.").arg(axisInfo.name));
			return false;
		}
	}

	axes_.append(axisInfo);
	scanSize_.append(axisInfo.size);

	// Make space for axis values if needed:
	/////////////////////////////////
	if(axisInfo.isUniform) {
		axisValueVarNums_ << -1;	// don't need to store it, it's uniform.
	}
	else {
		// make space in CDF to store axis values:
		QString measurementName = "AxisValues::" % axisInfo.name;
		QByteArray measurementNameBA = measurementName.toAscii();
		long varNum;

		// create a scalar variable for holding this axis's independent values. There will be just one number per record, and as many records as points along the axis.
		CDFstatus s = CDFcreatezVar(cdfId_, measurementNameBA.data(), CDF_DOUBLE, 1L, 0L, 0, VARY, 0, &varNum);
		if(s < CDF_OK) {
			AMErrorMon::debug(0, -724, QString("AMCDFDataStore: Could not create axis values CDF variable for axis '%1'. The CDF error was: %2").arg(axisDetails.name).arg(s));
			axisValueVarNums_ << -1;
			axes_.last().isUniform = true;	// save us so we don't go trying to access this spot that we couldn't create.
		}
		else {
			axisValueVarNums_ << varNum;
			double padValue = AMNUMBER_INVALID_FLOATINGPOINT;
			s = CDFsetzVarPadValue(cdfId_, varNum, &padValue);
			if(s != CDF_OK)
				AMErrorMon::debug(0, -104, "AMCDFDataStore: Could not set pad value for scan axis values.");

			// if this is the first scan axis, set a large blocking factor of (say, 500) since we're going to be appending in possibly single rows, and we want the CDF library to automatically allocate large blocks for us when we write to unallocated records.
			if(axisInfo.size == 0) {
				s = CDFsetzVarBlockingFactor(cdfId_, varNum, 500L);
				if(s != CDF_OK)
					AMErrorMon::debug(0, -125, "AMCDFDataStore: Could not set blocking factor for first scan axis value records.");
			}
			else {
				s = CDFsetzVarAllocRecords(cdfId_, varNum, long(axisInfo.size));
				if(s != CDF_OK)
					AMErrorMon::debug(0, -105, "AMCDFDataStore: Could not allocate scan axis value records.");
			}
		}
	}
	////////////////////////////////////


	// create attributes for this scan axis
	updateAttributesForScanAxis(axes_.count()-1);


	return true;
}

int AMCDFDataStore::idOfScanAxis(const QString &axisName) const
{
	for(int i=axes_.count()-1; i>=0; --i)
		if(axes_.at(i).name == axisName)
			return i;
	return -1;
}

AMNumber AMCDFDataStore::value(const AMnDIndex &scanIndex, int measurementId, const AMnDIndex &measurementIndex) const
{
	// scan axis index doesn't provide enough / too many dimensions
	if(scanIndex.rank() != axes_.count())
		return AMNumber(AMNumber::DimensionError);

	if((unsigned)measurementId >= (unsigned)measurements_.count())
		return AMNumber(AMNumber::InvalidError);	// invalid measurement specified

	if(measurementIndex.rank() != measurements_.at(measurementId).rank())
		return AMNumber(AMNumber::DimensionError);

	if(!measurementIndex.validInArrayOfSize(measurements_.at(measurementId).size()))
		return AMNumber(AMNumber::OutOfBoundsError);
	if(!scanIndex.validInArrayOfSize(scanSize_))
		return AMNumber(AMNumber::OutOfBoundsError);

	long recordIndex = axes_.count() ? scanIndex.flatIndexInArrayOfSize(scanSize_) : 0;

	// read value:
	long varNum = measurementVarNums_.at(measurementId);
	double rv;
	CDFstatus s = CDFgetzVarData(cdfId_, varNum, recordIndex, measurementIndex.constData(), &rv);
	if(s < CDF_OK) {
		AMErrorMon::debug(0, -107, "AMCDFDataStore: Error retrieving single value from data store.");
		return AMNumber(AMNumber::InvalidError);
	}
	return rv;
}

bool AMCDFDataStore::values(const AMnDIndex &scanIndexStart, const AMnDIndex &scanIndexEnd, int measurementId, const AMnDIndex &measurementIndexStart, const AMnDIndex &measurementIndexEnd, double *outputValues) const
{
	if(scanIndexStart.rank() != axes_.count() || scanIndexEnd.rank() != axes_.count())
		return false;
	if(measurementId >= measurements_.count())
		return false;

	const AMMeasurementInfo& mi = measurements_.at(measurementId);
	if(measurementIndexStart.rank() != mi.rank() || measurementIndexEnd.rank() != mi.rank())
		return false;

	// check bounds for scan axes
	for(int mu=axes_.count()-1; mu>=0; --mu) {
		if(scanIndexEnd.at(mu) < scanIndexStart.at(mu))
			return false;
		if(scanIndexEnd.at(mu) >= scanSize_.at(mu))
			return false;
	}
	// check bounds for measurement axes
	for(int mu=mi.rank()-1; mu >= 0; --mu) {
		if(measurementIndexEnd.at(mu) < measurementIndexStart.at(mu))
			return false;
		if(measurementIndexEnd.at(mu) >= mi.size(mu))
			return false;
	}

	// Which CDF variable corresponds to this measurement:
	long varNum = measurementVarNums_.at(measurementId);

	// the size of a measurement block to read: size from measurementIndexStart to measurementIndexEnd.
	AMnDIndex measurementBlockSize(measurementIndexEnd);
	measurementBlockSize -= measurementIndexStart;
	measurementBlockSize += 1;
	AMnDIndex measurementInterval(measurementIndexStart.rank(), AMnDIndex::DoInit, 1);	// an array of 1's

	CDFstatus s;
	// Handle specific cases of scan rank:
	switch(scanIndexStart.rank()) {
	case 0:
		s = CDFhyperGetzVarData(cdfId_, varNum, 0L, 1L, 1L, measurementIndexStart.constData(), measurementBlockSize.constData(), measurementInterval.constData(), outputValues);
		return (s >= CDF_OK);

	case 1:
		s = CDFhyperGetzVarData(cdfId_, varNum, scanIndexStart.i(), scanIndexEnd.i()-scanIndexStart.i()+1L, 1L, measurementIndexStart.constData(), measurementBlockSize.constData(), measurementInterval.constData(), outputValues);
		return (s >= CDF_OK);

	case 2: {
		// hyper-reading one j-axis row x one measurement block at a time. Need to figure out how big that is:
		long flatReadSize = measurementIndexStart.totalPointsTo(measurementIndexEnd) * (scanIndexEnd.j()-scanIndexStart.j()+1);
		for(long i=scanIndexStart.i(); i<=scanIndexEnd.i(); ++i) {
			long ic = i*scanSize_.j();
			s = CDFhyperGetzVarData(cdfId_, varNum, ic+scanIndexStart.j(), scanIndexEnd.j()-scanIndexStart.j()+1L, 1L, measurementIndexStart.constData(), measurementBlockSize.constData(), measurementInterval.constData(), outputValues);
			outputValues += flatReadSize;
			if(s < CDF_OK)
				return false;
		}
		return true;
	}

	case 3: {
		// reading one k-axis row x one measurement block at a time. Need to figure out how big that is.
		long flatReadSize = measurementIndexStart.totalPointsTo(measurementIndexEnd) * (scanIndexEnd.k()-scanIndexStart.k()+1);
		for(long i=scanIndexStart.i(); i<=scanIndexEnd.i(); ++i) {
			long ic = i*scanSize_.j()*scanSize_.k();
			for(long j=scanIndexStart.j(); j<=scanIndexEnd.j(); ++j) {
				long jc = j*scanSize_.k();
				s = CDFhyperGetzVarData(cdfId_, varNum, ic+jc+scanIndexStart.k(), scanIndexEnd.k()-scanIndexStart.k()+1L, 1L, measurementIndexStart.constData(), measurementBlockSize.constData(), measurementInterval.constData(), outputValues);
				outputValues += flatReadSize;
				if(s < CDF_OK)
					return false;
			}
		}
		return true;
	}

	case 4: {
		long flatReadSize = measurementIndexStart.totalPointsTo(measurementIndexEnd) * (scanIndexEnd.l() - scanIndexStart.l() + 1);
		for(long i=scanIndexStart.i(); i<=scanIndexEnd.i(); ++i) {
			long ic = i*scanSize_.j()*scanSize_.k()*scanSize_.l();
			for(long j=scanIndexStart.j(); j<=scanIndexEnd.j(); ++j) {
				long jc = j*scanSize_.k()*scanSize_.l();
				for(long k=scanIndexStart.k(); k<=scanIndexEnd.k(); ++k) {
					long kc = k*scanSize_.l();
					s = CDFhyperGetzVarData(cdfId_, varNum, ic+jc+kc+scanIndexStart.l(), scanIndexEnd.l()-scanIndexStart.l()+1L, 1L, measurementIndexStart.constData(), measurementBlockSize.constData(), measurementInterval.constData(), outputValues);
					outputValues += flatReadSize;
					if(s < CDF_OK)
						return false;
				}
			}
		}
		return true;
	}

	default: {
		// reading blocks at a time: will have size of the measurement block x the span of the last axis:
		int lastIndex = scanIndexStart.rank()-1;
		long flatReadSize = measurementIndexStart.totalPointsTo(measurementIndexEnd) * (scanIndexEnd.at(lastIndex)-scanIndexStart.at(lastIndex)+1);

		return valuesImplementationRecursive(scanIndexStart, scanIndexEnd, varNum, measurementIndexStart.constData(), measurementBlockSize.constData(), measurementInterval.constData(), &outputValues, flatReadSize, 0, 0);
	}
	}
}

bool AMCDFDataStore::valuesImplementationRecursive(const AMnDIndex &siStart, const AMnDIndex &siEnd, long varNum, const long* miStart, const long* miSize, const long* miInterval, double **outputValues, long flatReadSize, int currentDimension, long scanSpaceOffset) const
{
	if(currentDimension == axes_.count()-1) {	// base case: hyper read over last scan dimension records and measurement blocks
		CDFstatus s = CDFhyperGetzVarData(cdfId_, varNum, scanSpaceOffset+siStart.at(currentDimension), siEnd.at(currentDimension)-siStart.at(currentDimension)+1L, 1L, miStart, miSize, miInterval, *outputValues);
		*outputValues += flatReadSize;
		return (s >= CDF_OK);
	}

	else {
		for(long i=siStart.at(currentDimension); i<=siEnd.at(currentDimension); ++i) {
			// get product of all higher dimensions:
			long multiplier = 1;
			for(int mu=currentDimension+1; mu<siStart.rank(); ++mu)
				multiplier *= scanSize_.at(mu);

			// recurse:
			bool success = valuesImplementationRecursive(siStart, siEnd, varNum, miStart, miSize, miInterval, outputValues, flatReadSize, currentDimension+1, scanSpaceOffset + i*multiplier);
			if(!success)
				return false;
		}
		return true;
	}
}

bool AMCDFDataStore::setValue(const AMnDIndex &scanIndex, int measurementId, const AMnDIndex &measurementIndex, const AMNumber &newValue)
{
	if(readOnly_) {
		AMErrorMon::debug(0, -4002, "AMCDFDataStore: Modifications are not allowed in read-only mode.");
		return false;
	}

	if(scanIndex.rank() != axes_.count())
		return false; // scan axis index doesn't provide enough / too many dimensions

	if((unsigned)measurementId >= (unsigned)measurements_.count())
		return false;	// invalid measurement specified

	if(measurementIndex.rank() != measurements_.at(measurementId).rank())
		return false;

	if(!measurementIndex.validInArrayOfSize(measurements_.at(measurementId).size()))
		return false;
	if(!scanIndex.validInArrayOfSize(scanSize_))
		return false;

	long recordIndex = axes_.count() ? scanIndex.flatIndexInArrayOfSize(scanSize_) : 0L;

	// read value:
	long varNum = measurementVarNums_.at(measurementId);
	double dValue = newValue;
	CDFstatus s = CDFputzVarData(cdfId_, varNum, recordIndex, measurementIndex.constData(), &dValue);
	if(s < CDF_OK) {
		AMErrorMon::debug(0, -108, "AMCDFDataStore: could not put to variable. Please report this bug to the Acquaman developers.");
		return false;
	}

	emitDataChanged(scanIndex, scanIndex, measurementId);
	return true;
}


bool AMCDFDataStore::setValue(const AMnDIndex &scanIndex, int measurementId, const double *inputData) {
	if(readOnly_) {
		AMErrorMon::debug(0, -4004, "AMCDFDataStore: Modifications are not allowed in read-only mode.");
		return false;
	}

	if(scanIndex.rank() != axes_.count())
		return false; // scan axis index doesn't provide enough / too many dimensions

	if((unsigned)measurementId >= (unsigned)measurements_.count())
		return false;	// invalid measurement specified

	if(!scanIndex.validInArrayOfSize(scanSize_))
		return false;

	long recordIndex = axes_.count() ? scanIndex.flatIndexInArrayOfSize(scanSize_) : 0L;

	long varNum = measurementVarNums_.at(measurementId);

	CDFstatus s = CDFputzVarRecordData(cdfId_, varNum, recordIndex, (void*)inputData);
	if(s < CDF_OK) {
		AMErrorMon::debug(0, -109, "AMCDFDataStore: could not put measurement block to variable. Please report this bug to the Acquaman developers.");
		return false;
	}

	emitDataChanged(scanIndex, scanIndex, measurementId);
	return true;
}

bool AMCDFDataStore::setValue(const AMnDIndex &scanIndex, int measurementId, const int *inputData) {

	if(readOnly_) {
		AMErrorMon::debug(0, -4003, "AMCDFDataStore: Modifications are not allowed in read-only mode.");
		return false;
	}

	if((unsigned)measurementId >= (unsigned)measurements_.count())
		return false;	// invalid measurement specified

	// need to convert into double format of the data store
	long flatSize = measurements_.at(measurementId).size().product();

	QVector<double> d(flatSize);
	double* pd = d.data();
	double* pmax = pd + flatSize;
	while(pd < pmax)
		*(pd++) = *(inputData++);
	///////////////////////

	return setValue(scanIndex, measurementId, d.constData());
}

AMNumber AMCDFDataStore::axisValue(int axisId, long axisIndex) const
{
	if((unsigned)axisId >= (unsigned)axes_.count())
		return AMNumber(AMNumber::InvalidError);	// invalid axis specified.

	if((unsigned)axisIndex >= (unsigned)axes_.at(axisId).size)
		return AMNumber(AMNumber::OutOfBoundsError);

	const AMAxisInfo& ai = axes_.at(axisId);
	if(ai.isUniform)
		return (double)ai.start + axisIndex*(double)ai.increment;
	else {
		// find out the CDF variable we're storing this in:
		long varNum = axisValueVarNums_.at(axisId);
		if(varNum < 0) {
			AMErrorMon::debug(0, -109, "AMCDFDataStore: Could not access axis value: there was no CDF variable stored for that axis. Please report this bug to the Acquaman developers.");
			return AMNumber(AMNumber::InvalidError);
		}

		double rv;
		CDFstatus s = CDFgetzVarData(cdfId_, varNum, axisIndex, 0, &rv);
		if(s < CDF_OK) {
			AMErrorMon::debug(0, -109, "AMCDFDataStore: Could not retrieve axis value. Please report this bug to the Acquaman developers.");
			return AMNumber(AMNumber::InvalidError);
		}

		return rv;
	}
}

bool AMCDFDataStore::axisValues(int axisId, long axisStartIndex, long axisEndIndex, double *outputValues) const
{
	if((unsigned)axisId >= (unsigned)axes_.count())
		return false;	// invalid axis specified.

	if((unsigned)axisStartIndex >= (unsigned)axes_.at(axisId).size)
		return false;

	if((unsigned)axisEndIndex >= (unsigned)axes_.at(axisId).size)
		return false;

	const AMAxisInfo &axisInfo = axes_.at(axisId);

	if (axisInfo.isUniform){

		for (int i = 0, size = axisEndIndex-axisStartIndex+1; i < size; i++)
            outputValues[i] = double(axisInfo.start) +(axisStartIndex+i)*double(axisInfo.increment);
	}

	else {

		// Which CDF variable corresponds to this measurement:
		long varNum = axisValueVarNums_.at(axisId);
		long axisOffset = 1L;
		long size = axisEndIndex-axisStartIndex+1L;

		for (int i = scanSize_.rank()-1; i > axisId; i--)
			axisOffset *= scanSize_.at(i);

        CDFstatus s = CDFhyperGetzVarData(cdfId_, varNum, axisOffset*axisStartIndex, size, 1L, 0, 0, 0, outputValues);

		return (s >= CDF_OK);
	}

	return true;
}

bool AMCDFDataStore::setAxisValue(int axisId, long axisIndex, AMNumber newValue)
{
	if(readOnly_) {
		AMErrorMon::debug(0, -4005, "AMCDFDataStore: Modifications are not allowed in read-only mode.");
		return false;
	}

	if((unsigned)axisId >= (unsigned)axes_.count())
		return false;	// invalid axis specified.

	if((unsigned)axisIndex >= (unsigned)axes_.at(axisId).size)
		return false;

	if(axes_.at(axisId).isUniform)
		return false;

	long varNum = axisValueVarNums_.at(axisId);
	if(varNum < 0) {
		AMErrorMon::debug(0, -109, "AMCDFDataStore: Could not set axis value: there was no CDF variable stored for that axis. Please report this bug to the Acquaman developers.");
		return false;
	}

	double dValue = newValue;
	CDFstatus s = CDFputzVarData(cdfId_, varNum, axisIndex, 0, &dValue);
	if(s < CDF_OK) {
		AMErrorMon::debug(0, -108, "AMCDFDataStore: could not put to variable. Please report this bug to the Acquaman developers.");
		return false;
	}

	return true;
}

bool AMCDFDataStore::beginInsertRowsImplementation(long numRows, long atRowIndex)
{
	if(readOnly_) {
		AMErrorMon::debug(0, -4006, "AMCDFDataStore: Modifications are not allowed in read-only mode.");
		return false;
	}

	// number of scan points (records) to add, for every row of the first axis
	long recordsPerRow = 1;
	for(int mu=axes_.count()-1; mu>=1; --mu)
		recordsPerRow *= scanSize_.at(mu);
	long numberOfNewRecords = recordsPerRow*numRows;

	// we're always going to have to allocate new records at the END of the existing ones. If this is an insert, then we'll have to copy existing records down.
	long beginningOfNewRecords = scanSize_.i()*recordsPerRow;	// record index of the first soon-to-be newly-allocated record.
	long insertRecordNumber = atRowIndex*recordsPerRow;
	long recordsToMoveDown = beginningOfNewRecords - insertRecordNumber;

	CDFstatus s;
	// for all measurements, allocate these records AT THE END (even if inserting)
	for(int measurementId=0; measurementId<measurements_.count(); ++measurementId) {
		long varNum = measurementVarNums_.at(measurementId);
		s = CDFsetzVarAllocBlockRecords(cdfId_, varNum, beginningOfNewRecords, beginningOfNewRecords+numberOfNewRecords-1L);
		if(s != CDF_OK)
			AMErrorMon::debug(0, -110, QString("AMCDFDataStore: Warning while allocating records for beginInsertRows. CDF error or information was: %1. Beginning of new records = %2. Number of records = %3").arg(s).arg(beginningOfNewRecords).arg(numberOfNewRecords));

		// if not appending:
		if(insertRecordNumber != beginningOfNewRecords) {
			if(!cdfCopyZRecords(cdfId_, varNum, insertRecordNumber, recordsToMoveDown, insertRecordNumber+numberOfNewRecords))
				return false;
			/// \todo set the value of the "newly inserted" records to the null value. (They will still have old data values right now.)
		}
	}


	// for the first scan axis: move down existing records on insert.
	if(atRowIndex != scanSize_.i()) {
		long varNum = axisValueVarNums_.at(0);

		if(varNum >= 0) {	// valid, non-uniform scan axis.
			// don't worry about allocation. We let the CDF library auto-alloc this variable in blocks of 500. Just need to write to the new values at the end, as we shift the existing values down.
			if(!cdfCopyZRecords(cdfId_, varNum, atRowIndex, scanSize_.i()-atRowIndex, atRowIndex+numRows))
				return false;
		}
	}

	axes_[0].size += numRows;
	scanSize_[0] += numRows;

	// update Axis::Size attribute (entry 0, for first scan axis).
	qint32 newSize = scanSize_.i();
	if(!cdfPutIntAttribute(cdfId_, "Axis::Size", 0, &newSize))
		AMErrorMon::debug(0, -1021, "AMCDFDataStore: Could not update CDF attribute Axis::Size when inserting rows. Please report this bug to the Acquaman developers.");

	return true;
}

void AMCDFDataStore::clearScanDataPointsImplementation()
{
	if(readOnly_) {
		AMErrorMon::debug(0, -4007, "AMCDFDataStore: Modifications are not allowed in read-only mode.");
		return;
	}

	CDFstatus s;
	if(axes_.count() >= 1) {
		if(scanSize_.i() == 0)
			return;	// nothing to do. Already cleared.

		// clear all records of all measurements (except leave one record, which we use to store a the null scan space)
		long totalRecords = scanSize_.product();
		if(totalRecords > 1) {
			for(int i=0; i<measurements_.count(); ++i) {
				long varNum = measurementVarNums_.at(i);
				s = CDFdeletezVarRecords(cdfId_, varNum, 1L, totalRecords-1L);
				if(s != CDF_OK)
					AMErrorMon::debug(0, -112, QString("AMCDFDataStore: Could not delete records to clear scan points for measurement '%1'. Please report this bug to the Acquaman developers.").arg(measurements_.at(i).name));
			}
		}


		// clear first scan axis values:
		long varNum = axisValueVarNums_.at(0);
		if(varNum >= 0) {
			s = CDFdeletezVarRecords(cdfId_, varNum, 0L, scanSize_.i()-1L);
			if(s != CDF_OK)
				AMErrorMon::debug(0, -113, "AMCDFDataStore: Could not delete axis values when clearing scan data points. Please report this bug to the Acquaman developers.");
		}

		// reset axis size:
		axes_[0].size = 0L;
		scanSize_[0] = 0L;

		// Set the first scan axis Axis::Size attribute to 0.
		qint32 newSize = 0;
		if(!cdfPutIntAttribute(cdfId_, "Axis::Size", 0L, &newSize))
			AMErrorMon::debug(0, -1013, "AMCDFDataStore: Could not set scan axis size CDF attribute Axis::Size to 0 when clearing the scan data points. Please report this bug to the Acquaman developers.");
	}

	// if no scan axes: nothing to do. Leave the first record for each measurement for storing scalar scan-space values.
}

void AMCDFDataStore::clearMeasurementsImplementation()
{
	if(readOnly_) {
		AMErrorMon::debug(0, -4008, "AMCDFDataStore: Modifications are not allowed in read-only mode.");
		return;
	}

	CDFstatus s;
	// delete all the measurement variables:
	for(int i=measurements_.count()-1; i>=0; --i) {
		QByteArray variableName = measurements_.at(i).name.toAscii();
		long varNum = CDFgetVarNum(cdfId_, variableName.data());
		s = CDFdeletezVar(cdfId_, varNum);
		if(s != CDF_OK)
			AMErrorMon::debug(0, -114, QString("AMCDFDataStore: Could not delete CDF variable to clear measurement '%1'. Please report this bug to the Acquaman developers.").arg(measurements_.at(i).name));

		// Delete measurement attribute entries: "Measurement::Name".  All other measurement attributes are variable-scope, and should have been deleted with the variables.
		if(!cdfDeleteAttributeEntry(cdfId_, "Measurement::Name", long(i)))
			AMErrorMon::debug(0, -1014, QString("AMCDFDataStore: Could not delete CDF attribute entry Measurement::Name when clearing measurements. Please report this bug to the Acquaman developers."));

	}

	// clear measurements
	measurements_.clear();
	measurementVarNums_.clear();

	// the remaining CDF variables (particularly, the axis values for the scan axes) will now be messed up, because the varNums will possibly move down as we delete. Let's fix that now...
	for(int i=0; i<axes_.count(); ++i) {

		if(axisValueVarNums_.at(i) >= 0) {	// if this axis is supposed to have an axisValue variable:
			QByteArray variableName = QString("AxisValues::" % axes_.at(i).name).toAscii();
			long newVarNum = CDFgetVarNum(cdfId_, variableName.data());
			if(newVarNum<0) {
				AMErrorMon::debug(0, -115, QString("AMCDFDataStore: Could not find the CDF variable number for axis '%1' after clearing the measurements. Please report this bug to the Acquaman developers.").arg(axes_.at(i).name));
				axisValueVarNums_[i] = -1;
				axes_[i].isUniform = true;	// if we've lost its real axis values, somehow.
			}
			else {
				axisValueVarNums_[i] = newVarNum;
			}
		}
	}
}

void AMCDFDataStore::clearScanAxesImplementation()
{
	if(readOnly_) {
		AMErrorMon::debug(0, -4009, "AMCDFDataStore: Modifications are not allowed in read-only mode.");
		return;
	}

	if(axes_.count() == 0)
		return;	// nothing to do.

	CDFstatus s;

	// delete the variables for the scan axis values:
	for(int i=axes_.count()-1; i>=0; --i) {
		if(axisValueVarNums_.at(i) >= 0) {	// supposed to have an axisValue variable.
			QByteArray variableName = QString("AxisValues::" % axes_.at(i).name).toAscii();
			long varNum = CDFgetVarNum(cdfId_, variableName.data());
			if(varNum >= 0) {
				s = CDFdeletezVar(cdfId_, varNum);
				if(s != CDF_OK)
					AMErrorMon::debug(0, -116, QString("AMCDFDataStore: Could not delete the CDF variable number for axis '%1' after clearing the scan axes. Please report this bug to the Acquaman developers.").arg(axes_.at(i).name));
			}
		}

		// delete the attribute entries for these scan axes
		try {
			if(!cdfDeleteAttributeEntry(cdfId_, "Axis::Name", long(i))) throw QString("Axis::Name");
			if(!cdfDeleteAttributeEntry(cdfId_, "Axis::Size", long(i))) throw QString("Axis::Size");
			if(!cdfDeleteAttributeEntry(cdfId_, "Axis::Description", long(i))) throw QString("Axis::Description");
			if(!cdfDeleteAttributeEntry(cdfId_, "Axis::Units", long(i))) throw QString("Axis::Units");
			if(!cdfDeleteAttributeEntry(cdfId_, "Axis::IsUniform", long(i))) throw QString("Axis::IsUniform");
			if(!cdfDeleteAttributeEntry(cdfId_, "Axis::Start", long(i))) throw QString("Axis::Start");
			if(!cdfDeleteAttributeEntry(cdfId_, "Axis::Increment", long(i))) throw QString("Axis::Increment");
		}
		catch(QString errAttribute) {
			AMErrorMon::debug(0, -1016, QString("AMCDFDataStore: Could not delete the CDF attribute entry '%1' while clearing the scan axes.").arg(errAttribute));
		}
	}

	axes_.clear();
	scanSize_ = AMnDIndex();
	axisValueVarNums_.clear();


	// if we have any measurements, we'll have messed up the variable numbers of the measurements stored in measurementVarNums_.  Let's fix that now...
	for(int i=0; i<measurements_.count(); ++i) {
		QByteArray variableName = measurements_.at(i).name.toAscii();
		long newVarNum = CDFgetVarNum(cdfId_, variableName.data());
		if(newVarNum >= 0)
			measurementVarNums_[i] = newVarNum;
		else {
			measurementVarNums_[i] = -1;
			AMErrorMon::debug(0, -117, QString("Could not find the CDF variable for measurement '%1', after deleting the scan axes. Please report this bug to the Acquaman developers.").arg(measurements_.at(i).name));
		}
	}
}

bool AMCDFDataStore::cdfCopyZRecords(void *cdfId, long varNum, long sourceRecordStart, long recordCount, long destinationRecordStart)
{
	CDFstatus s;

	// Moving up, down, or not at all?
	if(sourceRecordStart == destinationRecordStart)
		return true;	// nothing to do!

	if(recordCount == 0)
		return true; // nothing to do!

	// find out size of variable:
	AMnDIndex varDims = cdfGetVarDimensions(cdfId, varNum);
	if(varDims.rank()==1 and varDims.i()<0) {	// error
		return false;
	}

	// Moving DOWNWARD
	if(destinationRecordStart > sourceRecordStart) {
		// Overlapping?
		if(sourceRecordStart+recordCount-1 >= destinationRecordStart) {

			int varByteSize = varDims.product()*sizeof(double);
			QByteArray buffer(varByteSize, Qt::Uninitialized);

			// for now, copy one-by-one from last record in source to last record in destination, moving upward.
			// Could optimize this in some cases by doing a block copy of the non-overlapping block after using this process on the overlapping block.
			for(long recI=recordCount-1; recI>=0; --recI) {
				long si = sourceRecordStart+recI;
				long di = destinationRecordStart+recI;

				s = CDFgetzVarRecordData(cdfId, varNum, si, buffer.data());
				if(s < CDF_OK)
					return false;
				s = CDFputzVarRecordData(cdfId, varNum, di, buffer.data());
				if(s < CDF_OK)
					return false;
			}
		}
		// Non-overlapping
		else {
			// Can copy the whole thing without overwriting source. Let's do lots of records at once.  How many records to copy at once depends on how many we can fit in memory.
			long varByteSize = varDims.product()*sizeof(double);
			// Use a maximum of 10MB of memory to copy:
			long recordsAtOnce = AM_CDFDATASTORE_MOVERECORDS_MEM_BYTES / varByteSize;
			recordsAtOnce = qBound(1L, recordsAtOnce, recordCount);

			QByteArray buffer(recordsAtOnce*sizeof(double), Qt::Uninitialized);

			for(long recI=0; recI<recordCount; recI+=recordsAtOnce) {
				long count = qMin(recordsAtOnce, recordCount-recI);	// the last block we copy might have less than recordsAtOnce

				long si = sourceRecordStart + recI;
				long di = destinationRecordStart + recI;

				s = CDFgetzVarRangeRecordsByVarID(cdfId, varNum, si, si+count-1, buffer.data());
				if(s < CDF_OK)
					return false;
				s = CDFputzVarRangeRecordsByVarID(cdfId, varNum, di, di+count-1, buffer.data());
				if(s < CDF_OK)
					return false;
			}
		}
	}


	// Moving UPWARD:
	else {
		// overlapping?
		if(destinationRecordStart+recordCount-1 >= sourceRecordStart) {
			// For now, copy record-by-record, starting with the first record in source. This makes sure the source is only overwriten after it's been copied out of.  Could optimize by moving the overlapping region record-by-record, and then moving the non-overlapping block all at once.
			int varByteSize = varDims.product()*sizeof(double);
			QByteArray buffer(varByteSize, Qt::Uninitialized);

			for(long recI=0; recI<recordCount; ++recI) {
				long si = sourceRecordStart+recI;
				long di = destinationRecordStart+recI;

				s = CDFgetzVarRecordData(cdfId, varNum, si, buffer.data());
				if(s < CDF_OK)
					return false;
				s = CDFputzVarRecordData(cdfId, varNum, di, buffer.data());
				if(s < CDF_OK)
					return false;
			}
		}
		// Not overlapping:
		else {
			// Can copy the whole thing without overwriting source. Let's do lots of records at once.  How many records to copy at once depends on how many we can fit in memory.
			long varByteSize = varDims.product()*sizeof(double);
			// Use a maximum of 10MB of memory to copy:
			long recordsAtOnce = AM_CDFDATASTORE_MOVERECORDS_MEM_BYTES / varByteSize;
			recordsAtOnce = qBound(1L, recordsAtOnce, recordCount);

			QByteArray buffer(recordsAtOnce*sizeof(double), Qt::Uninitialized);

			for(long recI=0; recI<recordCount; recI+=recordsAtOnce) {
				long count = qMin(recordsAtOnce, recordCount-recI);	// the last block we copy might have less than recordsAtOnce

				long si = sourceRecordStart + recI;
				long di = destinationRecordStart + recI;

				s = CDFgetzVarRangeRecordsByVarID(cdfId, varNum, si, si+count-1, buffer.data());
				if(s < CDF_OK)
					return false;
				s = CDFputzVarRangeRecordsByVarID(cdfId, varNum, di, di+count-1, buffer.data());
				if(s < CDF_OK)
					return false;
			}
		}
	}
	return true;
}

AMnDIndex AMCDFDataStore::cdfGetVarDimensions(void *cdfId, long varNum)
{
	long numDims;
	CDFstatus s = CDFgetzVarNumDims(cdfId, varNum, &numDims);
	if(s < CDF_OK) {
		AMErrorMon::debug(0, -300, "Could not retrieve number of dimensions from CDF for variable.");
		return AMnDIndex(-1);
	}

	if(numDims == 0)
		return AMnDIndex();	// scalar variable.

	QVector<long> dims(numDims);
	s = CDFgetzVarDimSizes(cdfId, varNum, dims.data());
	if(s < CDF_OK) {
		AMErrorMon::debug(0, -301, "Could not retrieve dimensions from CDF for variable.");
		return AMnDIndex(-1);
	}

	AMnDIndex rv(int(numDims), AMnDIndex::DoNotInit);
	for(int mu=rv.rank()-1; mu>=0; --mu)
		rv[mu] = dims.at(mu);

	return rv;
}

QString AMCDFDataStore::cdfGetStringAttribute(void *cdfId, const QString &attributeName, long entryNumber)
{
	CDFstatus s;
	QByteArray attrName = attributeName.toAscii();

	// Get attribute number / confirm existence
	long attrNum = CDFgetAttrNum(cdfId, attrName.data());
	if(attrNum < 0) return QString();

	// get attr scope
	long scope;
	s = CDFgetAttrScope(cdfId, attrNum, &scope);
	if(s < CDF_OK) return QString();

	// global attribute
	if(scope == GLOBAL_SCOPE) {
		// get data type / confirm entry existence
		long type, numChars;
		s = CDFinquireAttrgEntry(cdfId, attrNum, entryNumber, &type, &numChars);
		if(s < CDF_OK) return QString();
		if(type != CDF_CHAR) return QString();

		QByteArray rv(int(numChars), Qt::Uninitialized);
		s = CDFgetAttrgEntry(cdfId, attrNum, entryNumber, rv.data());
		if(s < CDF_OK) return QString();

		return QString(rv);
	}
	// variable-scope attribute
	else {
		// get data type / confirm entry existence
		long type, numChars;
		s = CDFinquireAttrzEntry(cdfId, attrNum, entryNumber, &type, &numChars);
		if(s < CDF_OK) return QString();
		if(type != CDF_CHAR) return QString();

		QByteArray rv(int(numChars), Qt::Uninitialized);
		s = CDFgetAttrzEntry(cdfId, attrNum, entryNumber, rv.data());
		if(s < CDF_OK) return QString();

		return QString(rv);
	}
}

bool AMCDFDataStore::cdfGetIntAttribute(void *cdfId, const QString &attributeName, long entryNumber, qint32 *out, long numElements)
{
	CDFstatus s;
	QByteArray attrName = attributeName.toAscii();

	// Get attribute number / confirm existence
	long attrNum = CDFgetAttrNum(cdfId, attrName.data());
	if(attrNum < 0) return false;

	// get attr scope
	long scope;
	s = CDFgetAttrScope(cdfId, attrNum, &scope);
	if(s < CDF_OK) return false;

	// global attribute
	if(scope == GLOBAL_SCOPE) {
		// get data type / confirm entry existence
		long type, num;
		s = CDFinquireAttrgEntry(cdfId, attrNum, entryNumber, &type, &num);
		if(s < CDF_OK) return false;
		if(type != CDF_INT4) return false;
		if(num != numElements) return false;

		s = CDFgetAttrgEntry(cdfId, attrNum, entryNumber, out);
		if(s < CDF_OK) return false;

		return true;
	}
	// variable-scope attribute
	else {
		// get data type / confirm entry existence
		long type, num;
		s = CDFinquireAttrzEntry(cdfId, attrNum, entryNumber, &type, &num);
		if(s < CDF_OK) return false;
		if(type != CDF_INT4) return false;
		if(num != numElements) return false;

		s = CDFgetAttrzEntry(cdfId, attrNum, entryNumber, out);
		if(s < CDF_OK) return false;

		return true;
	}
}

bool AMCDFDataStore::cdfGetDoubleAttribute(void *cdfId, const QString &attributeName, long entryNumber, double *out, long numElements)
{
	CDFstatus s;
	QByteArray attrName = attributeName.toAscii();

	// Get attribute number / confirm existence
	long attrNum = CDFgetAttrNum(cdfId, attrName.data());
	if(attrNum < 0) return false;

	// get attr scope
	long scope;
	s = CDFgetAttrScope(cdfId, attrNum, &scope);
	if(s < CDF_OK) return false;

	// global attribute
	if(scope == GLOBAL_SCOPE) {
		// get data type / confirm entry existence
		long type, num;
		s = CDFinquireAttrgEntry(cdfId, attrNum, entryNumber, &type, &num);
		if(s < CDF_OK) return false;
		if(type != CDF_DOUBLE) return false;
		if(num != numElements) return false;

		s = CDFgetAttrgEntry(cdfId, attrNum, entryNumber, out);
		if(s < CDF_OK) return false;

		return true;
	}
	// variable-scope attribute
	else {
		// get data type / confirm entry existence
		long type, num;
		s = CDFinquireAttrzEntry(cdfId, attrNum, entryNumber, &type, &num);
		if(s < CDF_OK) return false;
		if(type != CDF_DOUBLE) return false;
		if(num != numElements) return false;

		s = CDFgetAttrzEntry(cdfId, attrNum, entryNumber, out);
		if(s < CDF_OK) return false;

		return true;
	}
}

bool AMCDFDataStore::cdfPutStringAttribute(void *cdfId, const QString &attributeName, long entryNumber, const QString &newValue)
{
	CDFstatus s;
	QByteArray attrName = attributeName.toAscii();

	// CDF doesn't like empty string arguments. If value is empty, we'll add a blank space.
	QString value(newValue);
	if(value.isEmpty())
		value = " ";
	QByteArray stringValue = value.toAscii();

	// Get attribute number / confirm existence.
	long attrNum = CDFgetAttrNum(cdfId, attrName.data());
	if(attrNum < 0) return false;

	// get attr scope
	long scope;
	s = CDFgetAttrScope(cdfId, attrNum, &scope);
	if(s < CDF_OK) return false;

	// global attribute
	if(scope == GLOBAL_SCOPE) {
		s = CDFputAttrgEntry(cdfId, attrNum, entryNumber, CDF_CHAR, value.count(), stringValue.data());
		return (s >= CDF_OK);
	}
	// variable-scope attribute
	else {
		s = CDFputAttrzEntry(cdfId, attrNum, entryNumber, CDF_CHAR, value.count(), stringValue.data());
		return (s >= CDF_OK);
	}
}

bool AMCDFDataStore::cdfPutIntAttribute(void *cdfId, const QString &attributeName, long entryNumber, const qint32 *value, long numElements)
{
	CDFstatus s;
	QByteArray attrName = attributeName.toAscii();

	// Get attribute number / confirm existence.
	long attrNum = CDFgetAttrNum(cdfId, attrName.data());
	if(attrNum < 0) {
		AMErrorMon::debug(0, -1201, QString("AMCDFDataStore: Error while trying to set attribute %1. The CDF attribute was not found.").arg(attributeName));
		return false;
	}

	// get attr scope
	long scope;
	s = CDFgetAttrScope(cdfId, attrNum, &scope);
	if(s < CDF_OK) {
		AMErrorMon::debug(0, -1201, QString("AMCDFDataStore: Could not get scope for CDF attribute %1. The CDF error was %2.").arg(attributeName).arg(s));
		return false;
	}

	// global attribute
	if(scope == GLOBAL_SCOPE) {
		s = CDFputAttrgEntry(cdfId, attrNum, entryNumber, CDF_INT4, numElements, (void*)value);
	}
	// variable-scope attribute
	else {
		s = CDFputAttrzEntry(cdfId, attrNum, entryNumber, CDF_INT4, numElements, (void*)value);
	}
	if(s != CDF_OK)
		AMErrorMon::debug(0, -1201, QString("AMCDFDataStore: Warning while trying to set attribute entry %1. The CDF error was %2.").arg(attributeName).arg(s));

	return (s >= CDF_OK);
}

bool AMCDFDataStore::cdfPutDoubleAttribute(void *cdfId, const QString &attributeName, long entryNumber, const double* value, long numElements)
{
	CDFstatus s;
	QByteArray attrName = attributeName.toAscii();

	// Get attribute number / confirm existence.
	long attrNum = CDFgetAttrNum(cdfId, attrName.data());
	if(attrNum < 0) {
		AMErrorMon::debug(0, -1201, QString("AMCDFDataStore: Error while trying to set attribute %1. The CDF attribute was not found.").arg(attributeName));
		return false;
	}

	// get attr scope
	long scope;
	s = CDFgetAttrScope(cdfId, attrNum, &scope);
	if(s < CDF_OK) {
		AMErrorMon::debug(0, -1201, QString("AMCDFDataStore: Could not get scope for CDF attribute %1. The CDF error was %2.").arg(attributeName).arg(s));
		return false;
	}

	// global attribute
	if(scope == GLOBAL_SCOPE) {
		s = CDFputAttrgEntry(cdfId, attrNum, entryNumber, CDF_DOUBLE, numElements, (void*)value);
	}
	// variable-scope attribute
	else {
		s = CDFputAttrzEntry(cdfId, attrNum, entryNumber, CDF_DOUBLE, numElements, (void*)value);
	}
	if(s != CDF_OK)
		AMErrorMon::debug(0, -1201, QString("AMCDFDataStore: Warning while trying to set attribute entry %1. The CDF error was %2.").arg(attributeName).arg(s));

	return (s >= CDF_OK);
}

bool AMCDFDataStore::createAttributes()
{
	CDFstatus s;

	long attrNum;
	s = CDFcreateAttr(cdfId_, "AMCDF::Version", GLOBAL_SCOPE, &attrNum); if( s < CDF_OK) return false;
	s = CDFcreateAttr(cdfId_, "Measurement::Name", GLOBAL_SCOPE, &attrNum); if( s < CDF_OK) return false;
	s = CDFcreateAttr(cdfId_, "Axis::Name", GLOBAL_SCOPE, &attrNum); if(s < CDF_OK) return false;
	s = CDFcreateAttr(cdfId_, "Axis::Size", GLOBAL_SCOPE, &attrNum); if(s < CDF_OK) return false;
	s = CDFcreateAttr(cdfId_, "Axis::Description", GLOBAL_SCOPE, &attrNum); if(s < CDF_OK) return false;
	s = CDFcreateAttr(cdfId_, "Axis::Units", GLOBAL_SCOPE, &attrNum); if(s < CDF_OK) return false;
	s = CDFcreateAttr(cdfId_, "Axis::IsUniform", GLOBAL_SCOPE, &attrNum); if(s < CDF_OK) return false;
	s = CDFcreateAttr(cdfId_, "AxisValues::Start", GLOBAL_SCOPE, &attrNum); if(s < CDF_OK) return false;
	s = CDFcreateAttr(cdfId_, "AxisValues::Increment", GLOBAL_SCOPE, &attrNum); if(s < CDF_OK) return false;

	s = CDFcreateAttr(cdfId_, "Measurement::Description", VARIABLE_SCOPE, &attrNum); if(s < CDF_OK) return false;
	s = CDFcreateAttr(cdfId_, "Measurement::Units", VARIABLE_SCOPE, &attrNum); if(s < CDF_OK) return false;
	s = CDFcreateAttr(cdfId_, "Measurement::Axes::Names", VARIABLE_SCOPE, &attrNum); if(s < CDF_OK) return false;
	s = CDFcreateAttr(cdfId_, "Measurement::Axes::Descriptions", VARIABLE_SCOPE, &attrNum); if(s < CDF_OK) return false;
	s = CDFcreateAttr(cdfId_, "Measurement::Axes::Units", VARIABLE_SCOPE, &attrNum); if(s < CDF_OK) return false;
	s = CDFcreateAttr(cdfId_, "Measurement::AxisValues::Start", VARIABLE_SCOPE, &attrNum); if(s < CDF_OK) return false;
	s = CDFcreateAttr(cdfId_, "Measurement::AxisValues::Increment", VARIABLE_SCOPE, &attrNum); if(s < CDF_OK) return false;

	if(!cdfPutStringAttribute(cdfId_, "AMCDF::Version", 0L, "AMCDFDataStore_1")) return false;

	return true;
}

bool AMCDFDataStore::updateAttributesForMeasurement(int measurementId)
{
	const AMMeasurementInfo& measurementDetails = measurements_.at(measurementId);

	try {
		// global attributes:
		if(!cdfPutStringAttribute(cdfId_, "Measurement::Name", long(measurementId), measurementDetails.name)) throw QString("Measurement::Name");

		// variable-scope attributes: the entryNumber must be the CDF variable number.
		long varNum = measurementVarNums_.at(measurementId);
		if(!cdfPutStringAttribute(cdfId_, "Measurement::Description", varNum, measurementDetails.description)) throw QString("Measurement::Description");
		if(!cdfPutStringAttribute(cdfId_, "Measurement::Units", varNum, measurementDetails.units)) throw QString("Measurement::Units");
		if(!cdfPutStringAttribute(cdfId_, "Measurement::Description", varNum, measurementDetails.description)) throw QString("Measurement::Description");

		if(measurementDetails.rank()) {	// Only if we have measurement axes...
			// Make lists of the following (one item for each measurement axis)
			QStringList mAxisNames, mAxisDescriptions, mAxisUnits;
			QVector<double> mAxisValueStart, mAxisValueIncrement;
			for(int mu=0, cc=measurementDetails.rank(); mu<cc; ++mu) {
				const AMAxisInfo& ai = measurementDetails.axes.at(mu);
				mAxisNames << ai.name;
				mAxisDescriptions << ai.description;
				mAxisUnits << ai.units;
				mAxisValueStart << double(ai.start);
				mAxisValueIncrement << double(ai.increment);
			}

			if(!cdfPutStringAttribute(cdfId_, "Measurement::Axes::Names", varNum, mAxisNames.join(AM_CDFDATSTORE_STRING_DELIMITER))) throw QString("Measurement::Axes::Names");
			if(!cdfPutStringAttribute(cdfId_, "Measurement::Axes::Descriptions", varNum, mAxisDescriptions.join(AM_CDFDATSTORE_STRING_DELIMITER))) throw QString("Measurement::Axes::Descriptions");
			if(!cdfPutStringAttribute(cdfId_, "Measurement::Axes::Units", varNum, mAxisUnits.join(AM_CDFDATSTORE_STRING_DELIMITER))) throw QString("Measurement::Axes::Units");

			if(!cdfPutDoubleAttribute(cdfId_, "Measurement::AxisValues::Start", varNum, mAxisValueStart.constData(), mAxisValueStart.count())) throw QString("Measurement::AxisValues::Start");
			if(!cdfPutDoubleAttribute(cdfId_, "Measurement::AxisValues::Increment", varNum, mAxisValueIncrement.constData(), mAxisValueIncrement.count())) throw QString("Measurement::AxisValues::Increment");
		}
	}
	catch (QString errAttribute) {
		AMErrorMon::debug(0, -1002, QString("AMCDFDataStore: could not set attribute %1 for measurement.").arg(errAttribute));
		return false;
	}
	return true;
}

bool AMCDFDataStore::updateAttributesForScanAxis(int axisIdi)
{
	long axisId = axisIdi;

	const AMAxisInfo& ai = axes_.at(axisIdi);

	try {
		if(!cdfPutStringAttribute(cdfId_, "Axis::Name", axisId, ai.name)) throw QString("Axis::Name");
		qint32 size = ai.size;
		if(!cdfPutIntAttribute(cdfId_, "Axis::Size", axisId, &size)) throw QString("Axis::Size");
		if(!cdfPutStringAttribute(cdfId_, "Axis::Description", axisId, ai.description)) throw QString("Axis::Description");
		if(!cdfPutStringAttribute(cdfId_, "Axis::Units", axisId, ai.units)) throw QString("Axis::Units");
		qint32 isUniform = ai.isUniform;
		if(!cdfPutIntAttribute(cdfId_, "Axis::IsUniform", axisId, &isUniform)) throw QString("Axis::IsUniform");
		double start = double(ai.start), increment = double(ai.increment);
		if(!cdfPutDoubleAttribute(cdfId_, "AxisValues::Start", axisId, &start)) throw QString("AxisValues::Start");
		if(!cdfPutDoubleAttribute(cdfId_, "AxisValues::Increment", axisId, &increment)) throw QString("AxisValues::Increment");
	}
	catch(QString errAttribute) {
		AMErrorMon::debug(0, -1003, QString("AMCDFDataStore: could not set attribute %1 for axis info.").arg(errAttribute));
		return false;
	}
	return true;
}

bool AMCDFDataStore::cdfDeleteAttributeEntry(void *cdfId, const QString &attributeName, long entryNumber)
{
	CDFstatus s;
	QByteArray attrName = attributeName.toAscii();

	// Get attribute number / confirm existence.
	long attrNum = CDFgetAttrNum(cdfId, attrName.data());
	if(attrNum < 0) return false;

	// get attr scope
	long scope;
	s = CDFgetAttrScope(cdfId, attrNum, &scope);
	if(s < CDF_OK) return false;

	// global attribute
	if(scope == GLOBAL_SCOPE) {
		s = CDFdeleteAttrgEntry(cdfId, attrNum, entryNumber);
	}
	// variable-scope attribute
	else {
		s = CDFdeleteAttrzEntry(cdfId, attrNum, entryNumber);
	}

	return (s >= CDF_OK);
}

void AMCDFDataStore::setReadOnlyMode(bool readOnlyOn)
{
	readOnly_ = readOnlyOn;

	CDFstatus s = CDFsetReadOnlyMode(cdfId_, readOnly_ ? READONLYon : READONLYoff);
	if(s != CDF_OK)
		AMErrorMon::debug(0, -4040, "AMCDFDataStore: Warning: Could not change Read-Only state of the CDF file.");
}

bool AMCDFDataStore::initializeFromExistingCDF(const QString &filePath, bool createTemporaryCopy, bool setReadOnly)
{
	if(!QFile::exists(filePath))
		return false;

	QString newCDFFilePath = filePath;
	CDFid newCDFid = 0;

	// create copy?
	////////////////////////
	if(createTemporaryCopy) {
		QFileInfo originalFileInfo(filePath);
		newCDFFilePath = tempFileName(originalFileInfo.fileName());
		if(!QFile::copy(filePath, newCDFFilePath)) {
			AMErrorMon::debug(0, -5000, QString("AMCDFDataStore: Could not copy file '%1' to '%2' when opening in temporary file mode.").arg(filePath).arg(newCDFFilePath));
			return false;
		}
	}

	// Open new CDF:
	/////////////////////////
	QByteArray filePathBA = newCDFFilePath.toAscii();
	CDFsetValidate(VALIDATEFILEon);
	CDFstatus s = CDFopenCDF(filePathBA.constData(), &newCDFid);
	if(s < CDF_OK) {
		AMErrorMon::debug(0, -5001, QString("AMCDFDataStore: Could not open CDF file '%1'.").arg(newCDFFilePath));
		return false;
	}

	// Check that the format matches:
	QString format = cdfGetStringAttribute(newCDFid, "AMCDF::Version", 0L);
	if(format.isNull() || format != "AMCDFDataStore_1") {
		AMErrorMon::debug(0, -5001, QString("AMCDFDataStore: Could not open CDF file '%1'.  The file format is not correct, or the CDF file is damaged.").arg(newCDFFilePath));
		CDFcloseCDF(newCDFid);
		return false;
	}

	// Theoretically, if the file validated during opening, and the format string matches, it will contain everything we need to figure out the axes and measurements.  If not, then the writing of the file was messed up, and that's a bug somewhere else in AMCDFDataStore. Still, we'll gather this information into newMeasurements and newAxes for now, and only change everything over once we know we're fully successful.

	// access and fill newMeasurements (will become measurements_ if everything successful)
	/////////////////////////////////
	QList<AMMeasurementInfo> newMeasurements;
	QVector<long> newMeasurementVarNums;
	long numMeasurements;
	try {
		if(CDFgetNumAttrgEntries(newCDFid, CDFgetAttrNum(newCDFid, (char*)("Measurement::Name")), &numMeasurements) < CDF_OK) throw QString("Measurement::Name");
		for(long i=0; i<numMeasurements; ++i) {
			// Grab measurement name:
			QString measurementName = cdfGetStringAttribute(newCDFid, "Measurement::Name", i);
			QByteArray measurementNameBA = measurementName.toAscii();

			// find out variable number for this measurement.
			long varNum = CDFgetVarNum(newCDFid, measurementNameBA.data());
			if(varNum < 0) throw QString("Measurement '%1' not found.").arg(measurementName);
			newMeasurementVarNums << varNum;

			// Find out rank and dimensions
			long rank;
			if(CDFgetzVarNumDims(newCDFid, varNum, &rank) < CDF_OK) throw QString("Cannot read measurement rank.");

			// Get measurement axis information
			QList<AMAxisInfo> axes;
			if(rank > 0) {
				long dims[CDF_MAX_DIMS];
				if(CDFgetzVarDimSizes(newCDFid, varNum, dims) < CDF_OK) throw QString("Cannot read measurement axis sizes.");

				// Find out names, descriptions, units, start, and increment for each axis.
				QStringList axisNames = cdfGetStringAttribute(newCDFid, "Measurement::Axes::Names", varNum).split(AM_CDFDATSTORE_STRING_DELIMITER, QString::SkipEmptyParts);
				if(axisNames.count() != rank) throw QString("Cannot retrieve measurement axis names.");
				QStringList axisDescriptions = cdfGetStringAttribute(newCDFid, "Measurement::Axes::Descriptions", varNum).split(AM_CDFDATSTORE_STRING_DELIMITER, QString::SkipEmptyParts);
				if(axisDescriptions.count() != rank) throw QString("Cannot retrieve measurement axis description.");
				QStringList axisUnits = cdfGetStringAttribute(newCDFid, "Measurement::Axes::Units", varNum).split(AM_CDFDATSTORE_STRING_DELIMITER, QString::SkipEmptyParts);
				if(axisUnits.count() != rank) throw QString("Cannot retrieve measurement axis units.");
				QVector<double> axisValueStart(rank);
				QVector<double> axisValueIncrement(rank);
				if(!cdfGetDoubleAttribute(newCDFid, "Measurement::AxisValues::Start", varNum, axisValueStart.data(), rank)) throw QString("Cannot retrieve measurement axis starting value.");
				if(!cdfGetDoubleAttribute(newCDFid, "Measurement::AxisValues::Increment", varNum, axisValueIncrement.data(), rank)) throw QString("Cannot retrieve measurement axis increment value.");

				axes.reserve(rank);
				for(long i=0; i<rank; ++i) {

					AMAxisInfo axisInfo(axisNames.at(i), dims[i], axisDescriptions.at(i), axisUnits.at(i));
					axisInfo.start = axisValueStart.at(i);
					axisInfo.increment = axisValueIncrement.at(i);
					axes << axisInfo;
				}
			}

			// Get measurement description and units.
			QString description = cdfGetStringAttribute(newCDFid, "Measurement::Description", varNum);
			if(description.isNull()) throw QString("Cannot retrieve measurement description.");
			QString units = cdfGetStringAttribute(newCDFid, "Measurement::Units", varNum);
			if(units.isNull()) throw QString("Cannot retrieve measurement units.");

			// Finally, we have all the info for this measurement:
			newMeasurements << AMMeasurementInfo(measurementName, description, units, axes);
		}
	}
	catch(QString errMsg) {
		AMErrorMon::debug(0, -5003, "AMCDFDataStore: Error parsing measurement attribute in new CDF file: " % errMsg);
		CDFcloseCDF(newCDFid);
		return false;
	}


	// Access and fill newAxes (will become axes_ if everything is successful)
	////////////////////////////////////
	QList<AMAxisInfo> newAxes;
	QVector<long> newAxisValueVarNums;
	long numAxes;
	try {
		// How many scan axes?
		if(CDFgetNumAttrgEntries(newCDFid, CDFgetAttrNum(newCDFid, (char*)("Axis::Name")), &numAxes) < CDF_OK) throw QString("number of Axis::Name");
		for(long i=0; i<numAxes; ++i) {
			// Grab axis name
			QString axisName = cdfGetStringAttribute(newCDFid, "Axis::Name", i);
			if(axisName.isNull()) throw QString("Axis::Name.");
			// and axis size
			qint32 axisSize;
			if(!cdfGetIntAttribute(newCDFid, "Axis::Size", i, &axisSize)) throw QString("Axis::Size");

			AMAxisInfo ai(axisName, axisSize);
			ai.description = cdfGetStringAttribute(newCDFid, "Axis::Description", i);
			if(ai.description.isNull()) throw QString("Axis::Description");
			ai.units = cdfGetStringAttribute(newCDFid, "Axis::Units", i);
			if(ai.units.isNull()) throw QString("Axis::Units");

			qint32 isUniform;
			if(!cdfGetIntAttribute(newCDFid, "Axis::IsUniform", i, &isUniform)) throw QString("Axis::IsUniform");
			ai.isUniform = bool(isUniform);

			if(ai.isUniform) {
				// get start and increment values.
				double start, increment;
				if(!cdfGetDoubleAttribute(newCDFid, "AxisValues::Start", i, &start)) throw QString("AxisValues::Start");
				ai.start = start;
				if(!cdfGetDoubleAttribute(newCDFid, "AxisValues::Increment", i, &increment)) throw QString("AxisValues::Increment");
				ai.increment = increment;

				newAxisValueVarNums << -1;
			}
			else {
				// Find out variable number for the actual axis values for this axis.
				QByteArray scanAxisValueNameBA = QString("AxisValues::" % axisName).toAscii();
				long axisValueVarNum = CDFgetVarNum(newCDFid, scanAxisValueNameBA.data());
				if(axisValueVarNum < 0) throw QString("AxisValues::" % axisName);

				newAxisValueVarNums << axisValueVarNum;
			}

			newAxes << ai;
		}
	}
	catch(QString errMsg) {
		AMErrorMon::debug(0, -5004, "AMCDFDataStore: Error parsing scan axis attribute in new CDF file: " % errMsg);
		CDFcloseCDF(newCDFid);
		return false;
	}



	// OK, we're good to go. Make the switch-over.
	///////////////////////////////////////////
	// Existing CDF? close it, and delete it if in temporary mode.
	if(cdfId_) {
		CDFcloseCDF(cdfId_);

		if(fileIsTemporary_)
			QFile::remove(cdfFilePath_);
	}

	cdfId_ = newCDFid;
	cdfFilePath_ = newCDFFilePath;
	fileIsTemporary_ = createTemporaryCopy;

	axes_ = newAxes;

	for (int i = 0, numAxes = axes_.size(); i < numAxes; i++)
		scanSize_.append(axes_.at(i).size);

	axisValueVarNums_ = newAxisValueVarNums;
	measurements_ = newMeasurements;
	measurementVarNums_ = newMeasurementVarNums;


	// engage read-only mode?
	setReadOnlyMode(setReadOnly);

	return true;
}

bool AMCDFDataStore::flushToDisk()
{
	if(!isValid()) {
		AMErrorMon::debug(0, -6001, "AMCDFDataStore: Could not flush CDF because there is no valid CDF file open.");
		return false;
	}

	if(CDFcloseCDF(cdfId_) != CDF_OK) {
		AMErrorMon::debug(0, -6002, "AMCDFDataStore: Could not close CDF for flushing to disk.");
		return false;
	}
	cdfId_ = 0;
	CDFsetValidate(VALIDATEFILEoff);
	QByteArray cdfName = cdfFilePath_.toAscii();

	if(CDFopenCDF(cdfName.data(), &cdfId_) != CDF_OK) {
		AMErrorMon::debug(0, -6003, "AMCDFDataStore: Could not re-open CDF after flushing to disk.");
		return false;
	}

	// restore read-only mode set in previous instance of CDF access.
	setReadOnlyMode(readOnlyMode());

	return true;
}






