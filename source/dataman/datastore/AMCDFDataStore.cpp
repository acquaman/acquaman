#include "AMCDFDataStore.h"

#include <QTemporaryFile>
#include <QStringBuilder>
#include <QDir>
#include "util/AMErrorMonitor.h"
#include "cdf.h"

AMCDFDataStore::AMCDFDataStore() {

	cdfFilePath_ = tempFileName();
	fileIsTemporary_ = true;

	CDFstatus s = CDFcreateCDF(cdfFilePath_.toAscii().data(), &cdfId_);
	if(s != CDF_OK) {
		AMErrorMon::report(0, AMErrorReport::Serious, -101, "Could not create temporary file for CDF data store. Please report this bug to the Acquaman developers.");
	}
}

AMCDFDataStore::~AMCDFDataStore() {
	CDFcloseCDF(cdfId_);

	if(fileIsTemporary_)
		QFile::remove(cdfFilePath_);
}

QString AMCDFDataStore::tempFileName() const
{
	QString rv = QDir::tempPath() % "/amData";

	QFileInfo fi;
	do {
		fi.setFile(rv % QString(".%1.cdf").arg(int(qrand() % 1000000), 6, 10, QChar('0')));
	} while(fi.exists());

	return fi.filePath();
}

bool AMCDFDataStore::addMeasurement(const AMMeasurementInfo &measurementDetails)
{
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

	/// \todo Make attribute with names of measurements.

	return true;
}

#include <QDebug>
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
		// check 1: the size of this axis must not be 0; they must provide a final size
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

	/// \todo Make attribute with names of scan axes.

	if(axisInfo.isUniform) {
		axisValueVarNums_ << -1;	// don't need to store it; it's uniform.
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
		return AMNumber(AMNumber::InvalidError);	// invalid measurement specified;

	if(measurementIndex.rank() != measurements_.at(measurementId).rank())
		return AMNumber(AMNumber::DimensionError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if(!measurementIndex.validInArrayOfSize(measurements_.at(measurementId).size()))
		return AMNumber(AMNumber::OutOfBoundsError);
	if(!scanIndex.validInArrayOfSize(scanSize_))
		return AMNumber(AMNumber::OutOfBoundsError);
#endif

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

#ifdef AM_ENABLE_BOUNDS_CHECKING
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
#endif

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
	if(scanIndex.rank() != axes_.count())
		return false; // scan axis index doesn't provide enough / too many dimensions

	if((unsigned)measurementId >= (unsigned)measurements_.count())
		return false;	// invalid measurement specified;

	if(measurementIndex.rank() != measurements_.at(measurementId).rank())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if(!measurementIndex.validInArrayOfSize(measurements_.at(measurementId).size()))
		return false;
	if(!scanIndex.validInArrayOfSize(scanSize_))
		return false;
#endif

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
	if(scanIndex.rank() != axes_.count())
		return false; // scan axis index doesn't provide enough / too many dimensions

	if((unsigned)measurementId >= (unsigned)measurements_.count())
		return false;	// invalid measurement specified;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if(!scanIndex.validInArrayOfSize(scanSize_))
		return false;
#endif

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

	if((unsigned)measurementId >= (unsigned)measurements_.count())
		return false;	// invalid measurement specified;

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

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if((unsigned)axisIndex >= (unsigned)axes_.at(axisId).size)
		return AMNumber(AMNumber::OutOfBoundsError);
#endif
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

bool AMCDFDataStore::setAxisValue(int axisId, long axisIndex, AMNumber newValue)
{
	if((unsigned)axisId >= (unsigned)axes_.count())
		return false;	// invalid axis specified.

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if((unsigned)axisIndex >= (unsigned)axes_.at(axisId).size)
		return false;
#endif

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
	/// \todo Temporary: inserts are not yet supported. Only append.
	if(atRowIndex != scanSize_.i())
		return false;

	// number of scan points to add, for every row of the first axis
	long pointsPerRow = 1;
	for(int mu=axes_.count()-1; mu>=1; --mu)
		pointsPerRow *= scanSize_.at(mu);
	long numberOfNewRecords = pointsPerRow*numRows;

	CDFstatus s;
	// for all measurements, allocate these records:
	for(int measurementId=0; measurementId<measurements_.count(); ++measurementId) {
		long varNum = measurementVarNums_.at(measurementId);
		s = CDFsetzVarAllocBlockRecords(cdfId_, varNum, atRowIndex*pointsPerRow, atRowIndex*pointsPerRow+numberOfNewRecords-1L);
		if(s != CDF_OK)
			AMErrorMon::debug(0, -110, QString("AMCDFDataStore: Could not allocate records for beginInsertRows. CDF error was: %2.").arg(s));
	}

	// for the first scan axis: allocate more records:
	/// \todo Also move down existing records on insert.
//	long varNum = axisValueVarNums_.at(0);
//	if(varNum >= 0) {	// valid, non-uniform
//		s = CDFsetzVarAllocBlockRecords(cdfId_, varNum, atRowIndex, atRowIndex+numRows-1);
//		if(s != CDF_OK)
//			AMErrorMon::debug(0, -111, QString("AMCDFDataStore: Could not allocate records for axis values in beginInsertRows. CDF error was: %2.").arg(s));
//	}

	axes_[0].size += numRows;
	scanSize_[0] += numRows;

	return true;
}

void AMCDFDataStore::clearScanDataPointsImplementation()
{
	CDFstatus s;
	if(axes_.count() >= 1) {
		if(scanSize_.i() == 0)
			return;	// nothing to do; already cleared.

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
	}

	// if no scan axes: nothing to do. Leave the first record for each measurement for storing scalar scan-space values.
}

void AMCDFDataStore::clearMeasurementsImplementation()
{
	CDFstatus s;
	// delete all the measurement variables:
	for(int i=0; i<measurements_.count(); ++i) {
		QByteArray variableName = measurements_.at(i).name.toAscii();
		long varNum = CDFgetVarNum(cdfId_, variableName.data());
		s = CDFdeletezVar(cdfId_, varNum);
		if(s != CDF_OK)
			AMErrorMon::debug(0, -114, QString("AMCDFDataStore: Could not delete CDF variable to clear measurement '%1'. Please report this bug to the Acquaman developers.").arg(measurements_.at(i).name));
		/// \todo Remove measurement from Measurements attribute
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
	if(axes_.count() == 0)
		return;	// nothing to do.

	CDFstatus s;

	// delete the variables for the scan axis values:
	for(int i=0; i<axes_.count(); ++i) {
		if(axisValueVarNums_.at(i) >= 0) {	// supposed to have an axisValue variable.
			QByteArray variableName = QString("AxisValues::" % axes_.at(i).name).toAscii();
			long varNum = CDFgetVarNum(cdfId_, variableName.data());
			if(varNum >= 0) {
				s = CDFdeletezVar(cdfId_, varNum);
				if(s != CDF_OK)
					AMErrorMon::debug(0, -116, QString("AMCDFDataStore: Could not delete the CDF variable number for axis '%1' after clearing the scan axes. Please report this bug to the Acquaman developers.").arg(axes_.at(i).name));
			}
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





