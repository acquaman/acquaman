#include "AMDataTreeDataStore.h"

AMDataTreeDataStore::AMDataTreeDataStore(const AMAxisInfo &initialAxis, QObject *parent) :
	AMDbObject(parent)
{
	baseTree_ = new AMDataTree(0, initialAxis.name, true, true);
	axes_.setAllowsDuplicateKeys(false);
	axes_.append(initialAxis, initialAxis.name);
	axes_[0].size = 0;
	//dataTree_ = baseTree_;
	dataTree_ = new AMDataTree(0, initialAxis.name, true, true);
	measurements_.setAllowsDuplicateKeys(false);
	emptyTree_ = true;
}

AMDataTreeDataStore::~AMDataTreeDataStore(){
	qDebug() << "In destructor";
}

#warning "Handling naming, but not yet handling insertions when data already exists"
bool AMDataTreeDataStore::addMeasurement(const AMMeasurementInfo &measurementDetails){
	if( !measurements_.append(measurementDetails, measurementDetails.name) )
		return false;
	if(measurementDetails.rank() == 0){
		baseTree_->createColumn(measurementDetails.name, measurementDetails.description, measurementDetails.units);
		dataTree_->createColumn(measurementDetails.name, measurementDetails.description, measurementDetails.units);
	}
	else{
		QList<AMAxisInfo> axes;
		foreach(AMAxisInfo axis, measurementDetails.axes)
			axes << AMAxisInfo(axis.name, axis.size, axis.description, axis.units);
		baseTree_->createSubtreeColumn(measurementDetails.name, measurementInfoToTree(measurementDetails, axes) );
		dataTree_->createSubtreeColumn(measurementDetails.name, measurementInfoToTree(measurementDetails, axes) );
	}
	return true;
}

int AMDataTreeDataStore::idOfMeasurement(const QString &measurementName) const{
	return measurements_.indexOf(measurementName);
}

AMMeasurementInfo AMDataTreeDataStore::measurementAt(int id) const{
	if( (id < 0) || (id >= measurements_.count()) ){
#warning "What to return here?"
	}
	else
		return measurements_.at(id);
}

int AMDataTreeDataStore::measurementCount() const{
	return measurements_.count();
}


bool AMDataTreeDataStore::addScanAxis(const AMAxisInfo &axisDetails){
	if( !axes_.append(axisDetails, axisDetails.name) )
		return false;
	if(emptyTree_)
		axes_[(axes_.count()-1)].size = 0;
	else
		axes_[(axes_.count()-1)].size = 1;

	AMDataTree *oldDataTree = dataTree_;
	dataTree_ = new AMDataTree(axes_[(axes_.count()-1)].size, axisDetails.name, true, true);
	dataTree_->createSubtreeColumn(oldDataTree->xName(), oldDataTree);

	AMDataTree *oldBaseTree = baseTree_;
	baseTree_ = new AMDataTree(axes_[(axes_.count()-1)].size, axisDetails.name, true, true);
	baseTree_->createSubtreeColumn(oldBaseTree->xName(), oldBaseTree);
}

int AMDataTreeDataStore::idOfScanAxis(const QString &axisName) const{
	return axes_.indexOf(axisName);
}

AMAxisInfo AMDataTreeDataStore::scanAxisAt(int id) const{
	if( (id < 0) || (id >= axes_.count()) ){
#warning "What to return here?"
	}
	else
		return axes_.at(id);
}

int AMDataTreeDataStore::scanAxesCount() const{
	return axes_.count();
}

AMnDIndex AMDataTreeDataStore::scanSize() const{
	AMnDIndex nDIndex;
	AMDataTree *axisLevel = dataTree_;
	for(int x= 0; x < axes_.count(); x++){
		if(axisLevel->count() == 0)
			nDIndex.append(0);
		else{
			nDIndex.append(axisLevel->count());
			axisLevel = axisLevel->deeper(0,0);
		}
	}
	return nDIndex;
}

int AMDataTreeDataStore::scanSize(int axisId) const{
	if( (axisId < 0) || (axisId >= axes_.count()) )
		return -1;
	AMDataTree *axisLevel = dataTree_;
	for(int x=0; x < axisId; x++){
		if(axisLevel->count() == 0)
			return 0;
		axisLevel = axisLevel->deeper(0,0);
	}
	return axisLevel->count();
}

AMNumber AMDataTreeDataStore::value(const AMnDIndex &scanIndex, int measurementId, const AMnDIndex &measurementIndex) const {
	if( !scanSize().dimensionsMatch(scanIndex) )
		return AMNumber(AMNumber::DimensionError);
	if( !scanSize().inBounds(scanIndex) )
		return AMNumber(AMNumber::OutOfBoundsError);
	if( !measurementIndexExists(measurementId) )
		return AMNumber(AMNumber::DimensionError);
	if( !measurementAt(measurementId).size().dimensionsMatch(measurementIndex) )
		return AMNumber(AMNumber::DimensionError);
	if( !measurementAt(measurementId).size().inBounds(measurementIndex) )
		return AMNumber(AMNumber::OutOfBoundsError);

	AMDataTree *treeLevel = dataTree_;
	for(int x=0; x < scanIndex.rank()-1; x++)
		treeLevel = treeLevel->deeper(0, scanIndex[x]);

	AMNumber retVal;
	QString measurementName = measurementAt(measurementId).name;
	if(measurementAt(measurementId).size().rank() == 0)
		return treeLevel->value(measurementName, scanIndex[scanIndex.rank()-1]);
	else{
		treeLevel = treeLevel->deeper(measurementName, scanIndex[scanIndex.rank()-1]);
		for(int x=0; x < measurementIndex.rank()-1; x++)
			treeLevel = treeLevel->deeper(0, measurementIndex[x]);
		return treeLevel->value(0, measurementIndex[measurementIndex.rank()-1]);
	}

	return AMNumber();
}

AMNumber AMDataTreeDataStore::axisValue(int axisId, int axisIndex) const {
	if( axisId >= axes_.count() )
		return AMNumber(AMNumber::DimensionError);

	AMDataTree *axisLevel = dataTree_;
	for(int x= 0; x < axisId; x++)
		axisLevel = axisLevel->deeper(0, 0);
	if( axisIndex >= axisLevel->count() )
		return AMNumber(AMNumber::OutOfBoundsError);
	return axisLevel->x(axisIndex);
}

bool AMDataTreeDataStore::setValue(const AMnDIndex &scanIndex, int measurementId, const AMnDIndex &measurementIndex, const AMNumber &newValue){
	if( !scanSize().dimensionsMatch(scanIndex) )
		return false;
	if( !scanSize().inBounds(scanIndex) )
		return false;
	if( !measurementIndexExists(measurementId) )
		return false;
	if( !measurementAt(measurementId).size().dimensionsMatch(measurementIndex) )
		return false;
	if( !measurementAt(measurementId).size().inBounds(measurementIndex) )
		return false;

	AMDataTree *axisLevel = dataTree_;
	for(int x= 0; x < scanIndex.rank()-1; x++)
		axisLevel = axisLevel->deeper(0, scanIndex[x]);

	QString measurementName = measurementAt(measurementId).name;
	if( measurementAt(measurementId).rank() == 0)
		return axisLevel->setValue(measurementName, scanIndex[scanIndex.rank()-1], newValue );
	else{
		qDebug() << "Measurement has " << measurementIndex.rank() << " dimensions";
		axisLevel = axisLevel->deeper(measurementName, scanIndex[scanIndex.rank()-1]);
		for(int x=0; x < measurementIndex.rank()-1; x++)
			axisLevel = axisLevel->deeper(0, measurementIndex[x]);
		qDebug() << "Bottom of a multidim measurement has count " << axisLevel->count() << " and columns " << axisLevel->yColumnNames();
		return axisLevel->setValue(0, measurementIndex[measurementIndex.rank()-1], newValue);
	}
}

bool AMDataTreeDataStore::setAxisValue(int axisId, int axisIndex, AMNumber newValue){
	if( axisId >= axes_.count() )
		return false;

	AMDataTree *axisLevel = dataTree_;
	for(int x= 0; x < axisId; x++)
		axisLevel = axisLevel->deeper(0, 0);
	if( axisIndex >= axisLevel->count() )
		return false;
	return axisLevel->setX(axisIndex, newValue);
}


bool AMDataTreeDataStore::setValue(const AMnDIndex &scanIndex, int measurementId, const int* inputData, const int numArrayElements){
	if( !scanSize().dimensionsMatch(scanIndex) )
		return false;
	if( !scanSize().inBounds(scanIndex) )
		return false;
	if( !measurementIndexExists(measurementId) )
		return false;
	if( numArrayElements != measurementAt(measurementId).spanSize() )
		return false;


	AMDataTree *axisLevel = dataTree_;
	for(int x= 0; x < scanIndex.rank()-1; x++)
		axisLevel = axisLevel->deeper(0, scanIndex[x]);

	AMMeasurementInfo mi = measurementAt(measurementId);
	QString measurementName = mi.name;
	if( mi.rank() == 0)
		return axisLevel->setValue(measurementName, scanIndex[scanIndex.rank()-1], inputData[0] );
	else{
		axisLevel = axisLevel->deeper(measurementName, scanIndex[scanIndex.rank()-1]);
		for(int x=0; x < numArrayElements; x+=mi.size()[mi.rank()-1])
			if(!setValueFillBottom(bottomTreeFinder(axisLevel, mi, x), inputData+x, mi.size()[mi.rank()-1]))
				return false;
	}
	return true;
}

bool AMDataTreeDataStore::setValue(const AMnDIndex &scanIndex, int measurementId, const double* inputData, const int numArrayElements){
	if( !scanSize().dimensionsMatch(scanIndex) )
		return false;
	if( !scanSize().inBounds(scanIndex) )
		return false;
	if( !measurementIndexExists(measurementId) )
		return false;
	if( numArrayElements != measurementAt(measurementId).spanSize() )
		return false;


	AMDataTree *axisLevel = dataTree_;
	for(int x= 0; x < scanIndex.rank()-1; x++)
		axisLevel = axisLevel->deeper(0, scanIndex[x]);

	AMMeasurementInfo mi = measurementAt(measurementId);
	QString measurementName = mi.name;
	if( mi.rank() == 0)
		return axisLevel->setValue(measurementName, scanIndex[scanIndex.rank()-1], inputData[0] );
	else{
		axisLevel = axisLevel->deeper(measurementName, scanIndex[scanIndex.rank()-1]);
		qDebug() << "Pretty deep, name is " << measurementName;
		for(int x=0; x < numArrayElements; x+=mi.size()[mi.rank()-1])
			if(!setValueFillBottom(bottomTreeFinder(axisLevel, mi, x), inputData+x, mi.size()[mi.rank()-1]))
				return false;
	}
	return true;
}


bool AMDataTreeDataStore::beginInsertRowsImplementation(int axisId, int numRows, int atRowIndex){
	if( (axisId < 0) || (axisId >= axes_.count()) )
		return false;

	//qDebug() << "Appending valid axis IN BEGIN INSERT ROWS IMPLEMENTATION";
	AMDataTree *baseAxisLevel = baseTree_;
	AMDataTree *axisLevel = dataTree_;

	int rowCount = scanAxisAt(axisId).size;
	if( atRowIndex != rowCount)
		return false;

	if(emptyTree_){
		emptyTree_ = false;
		for(int x=0; x < axes_.count(); x++){
			baseAxisLevel->append();
			axisLevel->append();
			axes_[x].size++;
			axisLevel = axisLevel->deeper(0,0);
			baseAxisLevel = baseAxisLevel->deeper(0,0);
		}
		return true;
	}

	QList<int> newCounts;
	for(int x=0; x <= axisId; x++)
		newCounts << scanAxisAt(x).size;
	newCounts[axisId] = rowCount+1;
	qDebug() << "Append we want " << newCounts;
	appendToDepth(baseTree_, newCounts);
	qDebug() << "base/data before data append";
	dataStoreDataPukeHelper(baseTree_, axes_.count(), 0);
	dataStoreDataPukeHelper(dataTree_, axes_.count(), 0);
	appendToDepth(dataTree_, newCounts, baseTree_);
	//axes_[axisId].size++;



#warning "Only working on append for now"
	//int rowCount = scanAxisAt(axisId).size;
//	qDebug() << "Row count on axis " << axisId << " is " << rowCount;
	//if( atRowIndex != rowCount)
	//	return false;

	/*
	if(emptyTree_){
		emptyTree_ = false;
		for(int x=0; x < axes_.count(); x++){
			axisLevel->append();
			axes_[x].size++;
			axisLevel = axisLevel->deeper(0,0);
		}
		return true;
	}
	*/

	/* Complex append */
	//QList<int> newCounts;
	//for(int x=0; x <= axisId; x++)
	//	newCounts << scanAxisAt(x).size;
	//newCounts[axisId] = rowCount+1;
	//qDebug() << "Append we want " << newCounts;

	axes_[axisId].size++;

	/* End complex append */

	/* Simple append

	for(int x=0; x < axisId; x++)
		axisLevel = axisLevel->deeper(0,0);
	axisLevel->append();

	 End simple append */

	return true;
}

void AMDataTreeDataStore::endInsertRowsImplementation(int axisId, int numRows, int atRowIndex){

}

void AMDataTreeDataStore::clearScanDataPointsImplementation(){

}

void AMDataTreeDataStore::appendToDepth(AMDataTree* dataTree, QList<int> newCounts, AMDataTree* initializerTree){
//	if(depth == 1)
	if(initializerTree && newCounts.count() == 1 && dataTree->xName() == "temperature"){
		qDebug() << "data/null pair:";
		dataStoreDataPukeHelper(dataTree, 2, 0);
		dataStoreDataPukeHelper(initializerTree, 2, 0);
	}
	if( newCounts.count() == 1){
		if(newCounts.at(0) == dataTree->count()+1){
			if(initializerTree)
				dataTree->append(AMNumber(), initializerTree, true);
			else
				dataTree->append();
		}
		else
			return;
	}
	else{
		int rowCount = newCounts.takeFirst();
		for(int x=0; x < dataTree->count(); x++){
//			if(rowCount == dataTree->count()-1) NECESSARY?
			if(initializerTree)
				appendToDepth(dataTree->deeper(0, x), newCounts, initializerTree->deeper(0, x));
			else
				appendToDepth(dataTree->deeper(0, x), newCounts);
		}
	}
}

AMDataTree* AMDataTreeDataStore::measurementInfoToTree(const AMMeasurementInfo &measurementDetails, QList<AMAxisInfo> remainingAxes){
	if(remainingAxes.count() == 1){
		AMDataTree *treeBottom = new AMDataTree(remainingAxes.at(0).size, remainingAxes.at(0).name, true, true);
		//treeBottom->createColumn(remainingAxes.at(0).name, remainingAxes.at(0).description, remainingAxes.at(0).units);
		treeBottom->createColumn(measurementDetails.name, measurementDetails.description, measurementDetails.units);
		return treeBottom;
	}
	else{
		AMDataTree *notBottom = new AMDataTree(remainingAxes.at(0).size, remainingAxes.at(0).name, true, true);
		remainingAxes.pop_front();
		notBottom->createSubtreeColumn(remainingAxes.at(0).name, measurementInfoToTree(measurementDetails, remainingAxes));
		return notBottom;
	}
}

AMDataTree* AMDataTreeDataStore::bottomTreeFinder(AMDataTree *treeTop, const AMMeasurementInfo &measurementDetails, const int offset){
	AMDataTree *retTree = treeTop;
	if( offset >= measurementDetails.spanSize() )
		return NULL;
//	qDebug() << "Info: " << retTree->xName() << measurementDetails.rank() << measurementDetails.name;
//	qDebug() << "Offset of " << offset << " navigates to ";
	int chunkSize;
	for(int x=0; x < measurementDetails.rank()-1; x++){
		chunkSize = 1;
		for(int y=x+1; y < measurementDetails.rank(); y++)
			chunkSize *= measurementDetails.size()[y];
//		qDebug() << ((int)(offset/chunkSize))%measurementDetails.size()[x] ;
		retTree = retTree->deeper(0, (((int)(offset/chunkSize))%measurementDetails.size()[x]) ) ;
	}
	return retTree;
}

bool AMDataTreeDataStore::setValueFillBottom(AMDataTree* dataTree, const int *inputData, const int bottomDimension){
	if(dataTree->numYColumns() != 1)
		return false;
	if(dataTree->ySubtreeNames().count() != 0)
		return false;
	if(dataTree->count() != bottomDimension)
		return false;
	for(int x=0; x < bottomDimension; x++)
		if(!dataTree->setValue(0, x, inputData[x]))
			return false;

	return true;
}

bool AMDataTreeDataStore::setValueFillBottom(AMDataTree* dataTree, const double *inputData, const int bottomDimension){
	if(dataTree->numYColumns() != 1)
		return false;
	if(dataTree->ySubtreeNames().count() != 0)
		return false;
	if(dataTree->count() != bottomDimension)
		return false;
	for(int x=0; x < bottomDimension; x++)
		if(!dataTree->setValue(0, x, inputData[x]))
			return false;

	return true;
}

bool AMDataTreeDataStore::measurementIndexExists(int measurementId) const{
	if(measurementId >= measurements_.count())
		return false;

	return true;
}

void AMDataTreeDataStore::dataStoreDimensionsPuke(){
	dataStoreDimensionsPukeHelper(dataTree_, scanAxesCount(), 0);
}

void AMDataTreeDataStore::dataStoreDataPuke(){
	dataStoreDataPukeHelper(dataTree_, scanAxesCount(), 0);
}

void AMDataTreeDataStore::dataStoreBasePuke(){
	dataStoreDataPukeHelper(baseTree_, scanAxesCount(), 0);
}

void AMDataTreeDataStore::dataStoreDimensionsPukeHelper(const AMDataTree *dataTree, const int depthToGo, const int depthNow){
	QString puke = "-";
	for(int x=0; x < depthNow; x++)
		puke+="-";
	qDebug() << puke+"> " << dataTree->count() << "[" << dataTree->xName() << "]";
	if(depthToGo != 1)
		for(int x=0; x < dataTree->count(); x++)
			dataStoreDimensionsPukeHelper(dataTree->deeper(0, x), depthToGo-1, depthNow+1);
}

void AMDataTreeDataStore::dataStoreDataPukeHelper(const AMDataTree *dataTree, const int depthToGo, const int depthNow){
	QString puke, tmpStr;
	AMNumber curVal;
	if(depthToGo > 2){
		for(int x=0; x < dataTree->count(); x++){
			tmpStr.setNum((int)x);
			qDebug() << dataTree->xName()+"["+tmpStr+"]";
			dataStoreDataPukeHelper(dataTree->deeper(0, x), depthToGo-1, depthNow+1);
		}
	}
	else if(depthToGo == 2){
		for(int x=0; x < dataTree->count(); x++){
			for(int y=0; y < dataTree->deeper(0,0)->count(); y++){
				curVal = dataTree->deeper(0, x)->value(0, y);
				if(curVal.state() == AMNumber::Null)
					puke += " N";
				else if(curVal.state() == AMNumber::OutOfBoundsError)
					puke += " B";
				else if(curVal.state() == AMNumber::DimensionError)
					puke += " D";
				else{
					tmpStr.setNum((double)curVal);
					puke += " "+tmpStr;
				}
			}
			qDebug() << puke;
			puke.clear();
		}
	}
	else{
		for(int x=0; x < dataTree->count(); x++){
			curVal = dataTree->value(0, x);
			if(curVal.state() == AMNumber::Null)
				puke += " N";
			else if(curVal.state() == AMNumber::OutOfBoundsError)
				puke += " B";
			else if(curVal.state() == AMNumber::DimensionError)
				puke += " D";
			else{
				tmpStr.setNum((double)curVal);
				puke += " "+tmpStr;
			}
		}
		qDebug() << puke;
	}
}

void dataTreeColumnsPuke(const AMDataTree *dataTree){
	qDebug() << "X is " << dataTree->xName() << "[" << dataTree->count() << "]";
	if(dataTree->yColumnNames().count() > 0)
	qDebug() << " & columns are: ";
	foreach( QString colName, dataTree->yColumnNames() ){
		qDebug() << "-> " << colName << "(" << dataTree->column(colName).description() << ") in " << dataTree->column(colName).units();
	}
	if(dataTree->ySubtreeNames().count() > 0)
		qDebug() << "& subtrees are: ";
	foreach( QString subtreeName, dataTree->ySubtreeNames() ){
		qDebug() << "--> " << subtreeName;
		dataTreeColumnsPuke( dataTree->prototype(subtreeName) );
	}
}
