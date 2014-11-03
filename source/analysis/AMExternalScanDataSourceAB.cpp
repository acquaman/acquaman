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


#include "AMExternalScanDataSourceAB.h"
#include "dataman/AMScan.h"
#include "dataman/database/AMDbObjectSupport.h"

#include "util/AMErrorMonitor.h"
#include <QTimer>

 AMExternalScanDataSourceAB::~AMExternalScanDataSourceAB(){}
AMExternalScanDataSourceAB::AMExternalScanDataSourceAB(AMDatabase* sourceDatabase, int sourceScanId, const QString& sourceDataSourceName, const QString& outputDataSourceName, RefreshDataWhenSpec whenToLoadData, QObject *parent) :
	AMStandardAnalysisBlock(outputDataSourceName, parent)
{
	insideConstructor_ = true;
	sourceDb_ = sourceDatabase;
	sourceScanId_ = sourceScanId;
	sourceDataSourceName_ = sourceDataSourceName;

	// not valid until refreshData() is completed.
	setState(AMDataSource::InvalidFlag);
	scan_ = 0;

	// however, we should still get our rank, which means trying to load the scan
	AMDbObject* dbObject = 0;
	try {
		dbObject = AMDbObjectSupport::s()->createAndLoadObjectAt(sourceDb_,
														 AMDbObjectSupport::s()->tableNameForClass<AMScan>(),
														 sourceScanId_);
		/// \todo Once we have just one scan class, then we don't need to use the dynamic loader. Then we can do this without having to load the scan's raw data just to get our rank.

		scan_ = qobject_cast<AMScan*>(dbObject);
		if(!scan_)
			throw -1;

		int dataSourceIndex = scan_->indexOfDataSource(sourceDataSourceName_);
		if(dataSourceIndex < 0)
			throw -2;

		// good... now we'll have our rank. Requirement to not change rank once constructed is satisfied.
		axes_ = scan_->dataSourceAt(dataSourceIndex)->axes();

	}
	catch(int errCode) {
		if(dbObject) {
			dbObject->deleteLater();
		}
		scan_ = 0;
	}

	insideConstructor_ = false;

	switch(whenToLoadData) {
	case InConstructor:
		refreshData();
		break;
	case DeferAfterConstructor:
		QTimer::singleShot(0, this, SLOT(refreshData()));
		break;
	default:
	case Manually:
		break;
	}
}

AMExternalScanDataSourceAB::AMExternalScanDataSourceAB(AMDatabase* db, int id) : AMStandardAnalysisBlock("tempName")
{
	insideConstructor_ = true;

	setState(AMDataSource::InvalidFlag);
	scan_ = 0;
	sourceScanId_ = -1;

	loadFromDb(db, id);

	insideConstructor_ = false;
}

bool AMExternalScanDataSourceAB::areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const
{
	if(dataSources.isEmpty())
		return true;

	return false;
}

bool AMExternalScanDataSourceAB::refreshData()
{
	AMDbObject* dbObject = 0;
	AMnDIndex oldSize = size();

	try {
		// We might have a scan_ loaded already from the constructor. If not, attempt to load ourselves.
		if(!scan_) {
			dbObject = AMDbObjectSupport::s()->createAndLoadObjectAt(sourceDb_,
															 AMDbObjectSupport::s()->tableNameForClass<AMScan>(),
															 sourceScanId_);
			scan_ = qobject_cast<AMScan*>(dbObject);
			if(!scan_)
				throw -1;
		}

		int dataSourceIndex = scan_->indexOfDataSource(sourceDataSourceName_);
		if(dataSourceIndex < 0)
			throw -2;

		// get the axes from the source data. Since we're using AMStandardAnalysisBlock, this will automatically expose everything we need.
		axes_ = scan_->dataSourceAt(dataSourceIndex)->axes();

		// grab the data
		copyValues(dataSourceIndex);
		copyAxisValues(dataSourceIndex);

		setState(scan_->dataSourceAt(dataSourceIndex)->state());

		// delete the scan
		scan_->deleteLater();
		scan_ = 0;

		// signalling:

		emitAxisInfoChanged();
		if(oldSize != size())
			emitSizeChanged();
		emitValuesChanged();

		return true;
	}
	catch(int errCode) {
		if(dbObject)
			dbObject->deleteLater();
		if(scan_) {
			scan_->deleteLater();
			scan_ = 0;
		}
		setState(AMDataSource::InvalidFlag);
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, errCode, "Could not load external scan data."));
		return false;
	}
}

AMNumber AMExternalScanDataSourceAB::value(const AMnDIndex &indexes) const
{
	if(!isValid())
		return AMNumber::InvalidError;

	if(indexes.rank() != axes_.count())
		return AMNumber::DimensionError;

	switch(axes_.count()) {
	case 0:
		return values_.at(0);

	case 1:
#ifdef AM_ENABLE_BOUNDS_CHECKING
		if((unsigned)indexes.i() >= (unsigned)axes_.at(0).size)
				return AMNumber::OutOfBoundsError;
#endif
		return values_.at(indexes.i());

	case 2:
#ifdef AM_ENABLE_BOUNDS_CHECKING
		if(((unsigned)indexes.i() >= (unsigned)axes_.at(0).size ||
				(unsigned)indexes.j() >= (unsigned)axes_.at(1).size))
			return AMNumber::OutOfBoundsError;
#endif
		return values_.at(indexes.i()*axes_.at(1).size
						  + indexes.j());

	case 3: {
#ifdef AM_ENABLE_BOUNDS_CHECKING
		if(((unsigned)indexes.i() >= (unsigned)axes_.at(0).size ||
				(unsigned)indexes.j() >= (unsigned)axes_.at(1).size ||
				(unsigned)indexes.k() >= (unsigned)axes_.at(2).size))
			return AMNumber::OutOfBoundsError;
#endif

		int flatIndex = indexes.k();
		int stride = axes_.at(2).size;
		flatIndex += indexes.j()*stride;
		stride *= axes_.at(1).size;
		flatIndex += indexes.i()*stride;
		return values_.at(flatIndex);
	}

	case 4: {
#ifdef AM_ENABLE_BOUNDS_CHECKING
		if(((unsigned)indexes.i() >= (unsigned)axes_.at(0).size ||
				(unsigned)indexes.j() >= (unsigned)axes_.at(1).size ||
				(unsigned)indexes.k() >= (unsigned)axes_.at(2).size ||
				(unsigned)indexes.l() >= (unsigned)axes_.at(3).size))
			return AMNumber::OutOfBoundsError;
#endif

		int flatIndex = indexes.l();
		int stride = axes_.at(3).size;
		flatIndex += indexes.k()*stride;
		stride *= axes_.at(2).size;
		flatIndex += indexes.j()*stride;
		stride *= axes_.at(1).size;
		flatIndex += indexes.i();
		return values_.at(flatIndex);
	}

	case 5: {
#ifdef AM_ENABLE_BOUNDS_CHECKING
		if(((unsigned)indexes.i() >= (unsigned)axes_.at(0).size ||
				(unsigned)indexes.j() >= (unsigned)axes_.at(1).size ||
				(unsigned)indexes.k() >= (unsigned)axes_.at(2).size ||
				(unsigned)indexes.l() >= (unsigned)axes_.at(3).size ||
				(unsigned)indexes.m() >= (unsigned)axes_.at(4).size))
			return AMNumber::OutOfBoundsError;
#endif

		int flatIndex = indexes.m();
		int stride = axes_.at(4).size;
		flatIndex += indexes.l()*stride;
		stride *= axes_.at(3).size;
		flatIndex += indexes.k()*stride;
		stride *= axes_.at(2).size;
		flatIndex += indexes.j()*stride;
		stride *= axes_.at(1).size;
		flatIndex += indexes.i()*stride;

		return values_.at(flatIndex);
	}
	default:
		return AMNumber::InvalidError;
	}
}

AMNumber AMExternalScanDataSourceAB::axisValue(int axisNumber, int index) const
{
	if(axisNumber >= axes_.count())
		return AMNumber::DimensionError;

	const AMAxisInfo& axisInfo = axes_.at(axisNumber);

	if(axisInfo.isUniform)
		return (double)axisInfo.start + index*(double)axisInfo.increment;
	else {
#ifdef AM_ENABLE_BOUNDS_CHECKING
		if((unsigned)index >= (unsigned)axisInfo.size)
			return AMNumber::OutOfBoundsError;
#endif
		return axisValues_.at(axisNumber).at(index);
	}
}

bool AMExternalScanDataSourceAB::axisValues(int axisNumber, int startIndex, int endIndex, AMNumber *outputValues) const
{
	if(axisNumber >= axes_.count())
		return false;

	const AMAxisInfo& axisInfo = axes_.at(axisNumber);

	if(axisInfo.isUniform){

		for (int i = 0, size = endIndex-startIndex+1; i < size; i++)
			outputValues[i] = double(axisInfo.start) + (i+startIndex)*double(axisInfo.increment);

		return true;
	}

	else {

#ifdef AM_ENABLE_BOUNDS_CHECKING
		if((unsigned)startIndex >= (unsigned)axisInfo.size || (unsigned)endIndex >= (unsigned)axisInfo.size)
			return false;
#endif

		for (int i = 0, size = endIndex-startIndex+1; i < size; i++)
			outputValues[i] = axisValues_.at(axisNumber).at(i+startIndex);

		return true;
	}
}

void AMExternalScanDataSourceAB::copyValues(int dataSourceIndex)
{
	AMDataSource* ds = scan_->dataSourceAt(dataSourceIndex);
	const AMnDIndex size = ds->size();

	switch(ds->rank()) {
	case 0:
		values_.clear();
		values_ << ds->value(AMnDIndex());
		break;

	case 1: {
		values_.resize(size.i());
		for(int i=0; i<size.i(); i++)
			values_[i] = ds->value(i);
		break;
	}
	case 2: {
		values_.resize(size.i()*size.j());
		for(int i=0; i<size.i(); i++)
			for(int j=0; j<size.j(); j++)
				values_[i*size.j() + j] = ds->value(AMnDIndex(i,j));
		break;
	}
	case 3: {
		values_.resize(size.i()*size.j()*size.k());
		for(int i=0; i<size.i(); i++)
			for(int j=0; j<size.j(); j++)
				for(int k=0; k<size.k(); k++)
					values_[i*size.j()*size.k() + j*size.k() + k] = ds->value(AMnDIndex(i,j,k));
		break;
	}
	case 4: {
		values_.resize(size.i()*size.j()*size.k()*size.l());
		for(int i=0; i<size.i(); i++)
			for(int j=0; j<size.j(); j++)
				for(int k=0; k<size.k(); k++)
					for(int l=0; l<size.l(); l++)
						values_[i*size.j()*size.k()*size.l() + j*size.k()*size.l() + k*size.l() + l] = ds->value(AMnDIndex(i,j,k,l));
		break;
	}
	case 5: {
		values_.resize(size.i()*size.j()*size.k()*size.l()*size.m());
		for(int i=0; i<size.i(); i++)
			for(int j=0; j<size.j(); j++)
				for(int k=0; k<size.k(); k++)
					for(int l=0; l<size.l(); l++)
						for(int m=0; m<size.m(); m++)
							values_[i*size.j()*size.k()*size.l()*size.m() + j*size.k()*size.l()*size.m() + k*size.l()*size.m() + l*size.m() + m] = ds->value(AMnDIndex(i,j,k,l,m));
		/// \todo oh god, we really need a block copy or a multi-dimensional iterator for AMDataSource::value()...
		break;
	}
	}
}

void AMExternalScanDataSourceAB::copyAxisValues(int dataSourceIndex)
{
	AMDataSource* ds = scan_->dataSourceAt(dataSourceIndex);
	const AMnDIndex size = ds->size();

	axisValues_.clear();

	for(int mu=0; mu<size.rank(); mu++) {	// for each axis
		QVector<AMNumber> av;

		if(!axes_.at(mu).isUniform) {
			int axisLength = size.at(mu);
			for(int i=0; i<axisLength; i++)	// copy all the axis values
				av << axisValue(mu, i);
		}

		axisValues_ << av;
	}
}

bool AMExternalScanDataSourceAB::loadFromDb(AMDatabase *db, int id)
{
	// if we're not inside the constructor, verify that we have the same rank as the stored version
	if(!insideConstructor_) {
		QVariant storedRank = db->retrieve(id, dbTableName(), "rank");
		if(!storedRank.isValid() || rank() != storedRank.toInt() ) {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, "Not allowed to change the rank of an External Scan Data analysis block by re-loading it from the database."));
			return false;
		}
	}

	// load parameters from the db in the normal way.
	if(!AMStandardAnalysisBlock::loadFromDb(db, id))
		return false;

	AMDataSource::name_ = dbLoadOutputDataSourceName_;

	AMDatabase* sourceDb = AMDatabase::database(dbLoadConnectionName_);
	if(!sourceDb) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -2, "Couldn't locate the database containing the external scan data to load."));
		return false;
	}

	if(scan_) {	// don't want refreshData() to use an old scan object. This will ensure it loads a new one.
		scan_->deleteLater();
		scan_ = 0;
	}
	// from this point on, we've actually made permanent modifications to our parameters. Which means that we need to change the state to invalid if anything goes wrong from here. This will be taken care of by refreshData().
	sourceDb_ = sourceDb;
	sourceScanId_ = dbLoadScanId_;
	sourceDataSourceName_ = dbLoadSourceDataSourceName_;

	if(!refreshData()) {
		return false;
	}

	return true;
}



