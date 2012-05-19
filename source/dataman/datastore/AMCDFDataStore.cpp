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
	if(!createVarForMeasurement(measurementDetails, scanPointsCount_))
		return false;

	// add this measurement to our registry
	measurements_.append(measurementDetails);

	/// \todo Make attribute with names of measurements.

	return true;
}

#include <QDebug>
bool AMCDFDataStore::createVarForMeasurement(const AMMeasurementInfo &mi, int numInitialRecords)
{
	// create CDF variable:
	int varNum;
	AMnDIndex dimSizes(mi.size());
	AMnDIndex dimVariances(dimSizes.rank(), false);
	for(int mu=dimVariances.rank()-1; mu>=0; --mu)
		dimVariances[mu] = VARY;

	long numDims = mi.rank();
	qDebug() << "Size of int: " << sizeof(int) << "Size of long:" << sizeof(long);

	QByteArray variableName = mi.name.toAscii();
	CDFstatus s = CDFcreatezVar(cdfId_, variableName.data(), CDF_DOUBLE, 1, numDims, dimSizes.constData(), VARY, dimVariances.constData(), &varNum);
	if(s != CDF_OK) {
		AMErrorMon::debug(0, -102, QString("AMCDFDataStore: Could not create CDF variable for measurement '%1'. The CDF error was: %2. Num dims: %3. Size of int: %4. Size of long: %5.").arg(mi.name).arg(s).arg(mi.rank()).arg(sizeof(int)).arg(sizeof(long)));
		return false;
	}

	measurementVarNums_ << varNum;

	// set pad value
	double padValue = AMNUMBER_INVALID_FLOATINGPOINT;
	s = CDFsetzVarPadValue(cdfId_, varNum, &padValue);
	if(s != CDF_OK)
		AMErrorMon::debug(0, -102, QString("AMCDFDataStore: Could not set CDF pad value for measurement '%1'.").arg(mi.name));


	// create initial records: 1 if the scan axes are still empty (for null scan space), or the total number of points in scan space.
	s = CDFsetzVarInitialRecs(cdfId_, varNum, numInitialRecords);
	if(s != CDF_OK)
		AMErrorMon::debug(0, -102, QString("AMCDFDataStore: Could not allocate space for measurement '%1'.").arg(mi.name)); // not a critical error... We have already created the variable.


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
		int varNum;

		// create a scalar variable for holding this axis's independent values. There will be just one number per record, and as many records as points along the axis.
		CDFstatus s = CDFcreatezVar(cdfId_, measurementName.toAscii().data(), CDF_DOUBLE, 1, 0, 0, VARY, 0, &varNum);
		if(s != CDF_OK) {
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
			int allocateRecords = axisInfo.size ? axisInfo.size : 500; // for subsequent axes, we already know how long they're going to be. otherwise, pre-allocate room for at least 500, since we're sure we'll likely fill this up, and access will be faster if it's alloc'd in large blocks.
			s = CDFsetzVarAllocRecords(cdfId_, varNum, allocateRecords);
			if(s != CDF_OK)
				AMErrorMon::debug(0, -105, "AMCDFDataStore: Could not allocate scan axis value records.");
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

	int recordIndex = axes_.count() ? scanIndex.flatIndexInArrayOfSize(scanSize_) : 0;

	// read value:
	int varNum = measurementVarNums_.at(measurementId);
	double rv;
	CDFstatus s = CDFgetzVarData(cdfId_, varNum, recordIndex, measurementIndex.constData(), &rv);
	if(s != CDF_OK) {
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
	int varNum = measurementVarNums_.at(measurementId);

	// the size of a measurement block to read: size from measurementIndexStart to measurementIndexEnd.
	AMnDIndex measurementBlockSize(measurementIndexEnd);
	measurementBlockSize -= measurementIndexStart;
	measurementBlockSize += 1;
	AMnDIndex measurementInterval(measurementIndexStart.rank(), true, 1);	// an array of 1's

	CDFstatus s = CDF_OK;
	// Handle specific cases of scan rank:
	switch(scanIndexStart.rank()) {
	case 0:
		s = CDFhyperGetzVarData(cdfId_, varNum, 0, 1, 1, measurementIndexStart.constData(), measurementBlockSize.constData(), measurementInterval.constData(), outputValues);
		return (s == CDF_OK);

	case 1:
		s = CDFhyperGetzVarData(cdfId_, varNum, scanIndexStart.i(), scanIndexEnd.i()-scanIndexStart.i()+1, 1, measurementIndexStart.constData(), measurementBlockSize.constData(), measurementInterval.constData(), outputValues);
		return (s == CDF_OK);

	case 2: {
		// hyper-reading one j-axis row x one measurement block at a time. Need to figure out how big that is:
		int flatReadSize = measurementIndexStart.totalPointsTo(measurementIndexEnd) * (scanIndexEnd.j()-scanIndexStart.j()+1);
		for(int i=scanIndexStart.i(); i<=scanIndexEnd.i(); ++i) {
			int ic = i*scanSize_.j();
			s |= CDFhyperGetzVarData(cdfId_, varNum, ic+scanIndexStart.j(), scanIndexEnd.j()-scanIndexStart.j()+1, 1, measurementIndexStart.constData(), measurementBlockSize.constData(), measurementInterval.constData(), outputValues);
			outputValues += flatReadSize;
		}
		return (s == CDF_OK);
	}

	case 3: {
		// reading one k-axis row x one measurement block at a time. Need to figure out how big that is.
		int flatReadSize = measurementIndexStart.totalPointsTo(measurementIndexEnd) * (scanIndexEnd.k()-scanIndexStart.k()+1);
		for(int i=scanIndexStart.i(); i<=scanIndexEnd.i(); ++i) {
			int ic = i*scanSize_.j()*scanSize_.k();
			for(int j=scanIndexStart.j(); j<=scanIndexEnd.j(); ++j) {
				int jc = j*scanSize_.k();
				s |= CDFhyperGetzVarData(cdfId_, varNum, ic+jc+scanIndexStart.k(), scanIndexEnd.k()-scanIndexStart.k()+1, 1, measurementIndexStart.constData(), measurementBlockSize.constData(), measurementInterval.constData(), outputValues);
				outputValues += flatReadSize;
			}
		}
		return (s == CDF_OK);
	}

	case 4: {
		int flatReadSize = measurementIndexStart.totalPointsTo(measurementIndexEnd) * (scanIndexEnd.l() - scanIndexStart.l() + 1);
		for(int i=scanIndexStart.i(); i<=scanIndexEnd.i(); ++i) {
			int ic = i*scanSize_.j()*scanSize_.k()*scanSize_.l();
			for(int j=scanIndexStart.j(); j<=scanIndexEnd.j(); ++j) {
				int jc = j*scanSize_.k()*scanSize_.l();
				for(int k=scanIndexStart.k(); k<=scanIndexEnd.k(); ++k) {
					int kc = k*scanSize_.l();
					s |= CDFhyperGetzVarData(cdfId_, varNum, ic+jc+kc+scanIndexStart.l(), scanIndexEnd.l()-scanIndexStart.l()+1, 1, measurementIndexStart.constData(), measurementBlockSize.constData(), measurementInterval.constData(), outputValues);
					outputValues += flatReadSize;
				}
			}
		}
		return (s == CDF_OK);
	}

	default: {
		// reading blocks at a time: will have size of the measurement block x the span of the last axis:
		int lastIndex = scanIndexStart.rank()-1;
		int flatReadSize = measurementIndexStart.totalPointsTo(measurementIndexEnd) * (scanIndexEnd.at(lastIndex)-scanIndexStart.at(lastIndex)+1);

		return valuesImplementationRecursive(scanIndexStart, scanIndexEnd, varNum, measurementIndexStart.constData(), measurementBlockSize.constData(), measurementInterval.constData(), &outputValues, flatReadSize, 0, 0);
	}
	}
}

bool AMCDFDataStore::valuesImplementationRecursive(const AMnDIndex &siStart, const AMnDIndex &siEnd, int varNum, const int* miStart, const int* miSize, const int* miInterval, double **outputValues, int flatReadSize, int currentDimension, int scanSpaceOffset) const
{
	if(currentDimension == axes_.count()-1) {	// base case: hyper read over last scan dimension records and measurement blocks
		CDFstatus s = CDFhyperGetzVarData(cdfId_, varNum, scanSpaceOffset+siStart.at(currentDimension), siEnd.at(currentDimension)-siStart.at(currentDimension)+1, 1, miStart, miSize, miInterval, *outputValues);
		*outputValues += flatReadSize;
		return (s == CDF_OK);
	}

	else {
		for(int i=siStart.at(currentDimension); i<=siEnd.at(currentDimension); ++i) {
			// get product of all higher dimensions:
			int multiplier = 1;
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

	int recordIndex = axes_.count() ? scanIndex.flatIndexInArrayOfSize(scanSize_) : 0;

	// read value:
	int varNum = measurementVarNums_.at(measurementId);
	double dValue = newValue;
	CDFstatus s = CDFputzVarData(cdfId_, varNum, recordIndex, measurementIndex.constData(), &dValue);
	if(s != CDF_OK) {
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

	int recordIndex = axes_.count() ? scanIndex.flatIndexInArrayOfSize(scanSize_) : 0;

	int varNum = measurementVarNums_.at(measurementId);

	CDFstatus s = CDFputzVarRecordData(cdfId_, varNum, recordIndex, (void*)inputData);
	if(s != CDF_OK) {
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
	int flatSize = measurements_.at(measurementId).size().product();

	QVector<double> d(flatSize);
	double* pd = d.data();
	double* pmax = pd + flatSize;
	while(pd < pmax)
		*(pd++) = *(inputData++);
	///////////////////////

	return setValue(scanIndex, measurementId, d.constData());
}

AMNumber AMCDFDataStore::axisValue(int axisId, int axisIndex) const
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
		int varNum = axisValueVarNums_.at(axisId);
		if(varNum < 0) {
			AMErrorMon::debug(0, -109, "AMCDFDataStore: Could not access axis value: there was no CDF variable stored for that axis. Please report this bug to the Acquaman developers.");
			return AMNumber(AMNumber::InvalidError);
		}

		double rv;
		CDFstatus s = CDFgetzVarData(cdfId_, varNum, axisIndex, 0, &rv);
		if(s != CDF_OK) {
			AMErrorMon::debug(0, -109, "AMCDFDataStore: Could not retrieve axis value. Please report this bug to the Acquaman developers.");
			return AMNumber(AMNumber::InvalidError);
		}

		return rv;
	}
}

bool AMCDFDataStore::setAxisValue(int axisId, int axisIndex, AMNumber newValue)
{
	if((unsigned)axisId >= (unsigned)axes_.count())
		return false;	// invalid axis specified.

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if((unsigned)axisIndex >= (unsigned)axes_.at(axisId).size)
		return false;
#endif

	if(axes_.at(axisId).isUniform)
		return false;

	int varNum = axisValueVarNums_.at(axisId);
	if(varNum < 0) {
		AMErrorMon::debug(0, -109, "AMCDFDataStore: Could not set axis value: there was no CDF variable stored for that axis. Please report this bug to the Acquaman developers.");
		return false;
	}

	double dValue = newValue;
	CDFstatus s = CDFputzVarData(cdfId_, varNum, axisIndex, 0, &dValue);
	if(s != CDF_OK) {
		AMErrorMon::debug(0, -108, "AMCDFDataStore: could not put to variable. Please report this bug to the Acquaman developers.");
		return false;
	}

	return true;
}

bool AMCDFDataStore::beginInsertRowsImplementation(int numRows, int atRowIndex)
{
	/// \todo Temporary: inserts are not yet supported. Only append.
	if(atRowIndex != scanSize_.i())
		return false;

	// number of scan points to add, for every row of the first axis
	int pointsPerRow = 1;
	for(int mu=axes_.count()-1; mu>=1; --mu)
		pointsPerRow *= scanSize_.at(mu);
	int numberOfNewRecords = pointsPerRow*numRows;

	CDFstatus s;
	// for all measurements, allocate these records:
	for(int measurementId=0; measurementId<measurements_.count(); ++measurementId) {
		int varNum = measurementVarNums_.at(measurementId);
		s = CDFsetzVarAllocBlockRecords(cdfId_, varNum, atRowIndex*pointsPerRow, atRowIndex*pointsPerRow+numberOfNewRecords-1);
		if(s != CDF_OK)
			AMErrorMon::debug(0, -110, "AMCDFDataStore: Could not allocate records for beginInsertRows.");
	}

	// for the first scan axis: allocate more records:
	/// \todo Also move down existing records on insert.
	int varNum = axisValueVarNums_.at(0);
	if(varNum >= 0) {	// valid, non-uniform
		s = CDFsetzVarAllocBlockRecords(cdfId_, varNum, atRowIndex, atRowIndex+numRows-1);
		if(s != CDF_OK)
			AMErrorMon::debug(0, -111, "AMCDFDataStore: Could not allocate records for axis values in beginInsertRows.");
	}

	return true;
}

void AMCDFDataStore::clearScanDataPointsImplementation()
{
	CDFstatus s;
	if(axes_.count() >= 1) {
		if(scanSize_.i() == 0)
			return;	// nothing to do; already cleared.

		// clear all records for all measurements:
		int totalRecords = scanSize_.product();
		if(totalRecords > 1) {
			for(int i=0; i<measurements_.count(); ++i) {
				int varNum = measurementVarNums_.at(i);
				s = CDFdeletezVarRecords(cdfId_, varNum, 1, totalRecords-1);
				if(s != CDF_OK)
					AMErrorMon::debug(0, -112, QString("AMCDFDataStore: Could not delete records to clear scan points for measurement '%1'. Please report this bug to the Acquaman developers.").arg(measurements_.at(i).name));
			}
		}


		// clear first scan axis values:
		int varNum = axisValueVarNums_.at(0);
		if(varNum >= 0) {
			s = CDFdeletezVarRecords(cdfId_, varNum, 0, scanSize_.i()-1);
			if(s != CDF_OK)
				AMErrorMon::debug(0, -113, "AMCDFDataStore: Could not delete axis values when clearing scan data points. Please report this bug to the Acquaman developers.");
		}

		// reset axis size:
		axes_[0].size = 0;
		scanSize_[0] = 0;
	}

	// if no scan axes: nothing to do. Leave the first record for each measurement for storing scalar scan-space values.
}

void AMCDFDataStore::clearMeasurementsImplementation()
{
	CDFstatus s;
	// delete all the measurement variables:
	for(int i=0; i<measurements_.count(); ++i) {
		int varNum = CDFgetVarNum(cdfId_, measurements_.at(i).name.toAscii().data());
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
			QString variableName = "AxisValues::" % axes_.at(i).name;
			int newVarNum = CDFgetVarNum(cdfId_, variableName.toAscii().data());
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
			QString variableName = "AxisValues::" % axes_.at(i).name;
			int varNum = CDFgetVarNum(cdfId_, variableName.toAscii().data());
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
		int newVarNum = CDFgetVarNum(cdfId_, measurements_.at(i).name.toAscii().data());
		if(newVarNum >= 0)
			measurementVarNums_[i] = newVarNum;
		else {
			measurementVarNums_[i] = -1;
			AMErrorMon::debug(0, -117, QString("Could not find the CDF variable for measurement '%1', after deleting the scan axes. Please report this bug to the Acquaman developers.").arg(measurements_.at(i).name));
		}
	}
}





