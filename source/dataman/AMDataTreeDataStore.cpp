#include "AMDataTreeDataStore.h"

AMDataTreeDataStore::AMDataTreeDataStore(const AMAxisInfo &initialAxis, QObject *parent) :
	AMDbObject(parent)
{
	baseTree_ = new AMDataTree(0, initialAxis.name, true);
	dataTree_ = baseTree_;
}

AMDataTreeDataStore::~AMDataTreeDataStore(){
	qDebug() << "In destructor";
}

#warning "Handling naming, but not yet handling insertions when data already exists"
bool AMDataTreeDataStore::addMeasurement(const AMMeasurementInfo &measurementDetails){
	if(baseTree_->yColumnNames().contains(measurementDetails.name) || baseTree_->ySubtreeNames().contains(measurementDetails.name))
		return false;
	measurements_ << measurementDetails;
	if(measurementDetails.rank() == 0)
		baseTree_->createColumn(measurementDetails.name, measurementDetails.description, measurementDetails.units);
	else{
		QList<AMAxisInfo> axes;
		foreach(AMAxisInfo axis, measurementDetails.axes)
			axes << AMAxisInfo(axis.name, axis.size, axis.description, axis.units);
		baseTree_->createSubtreeColumn(measurementDetails.name, measurementInfoToTree(measurementDetails, axes) );
	}
	return true;
}

int AMDataTreeDataStore::idOfMeasurement(const QString &measurementName) const{
	for(int x = 0; x < measurements_.count(); x++)
		if(measurementName == measurements_.at(x).name)
			return x;
	return -1;
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

}

int AMDataTreeDataStore::idOfScanAxis(const QString &axisName) const{

}

AMAxisInfo AMDataTreeDataStore::scanAxisAt(int id) const{

}

int AMDataTreeDataStore::scanAxesCount() const{

}


AMNumber AMDataTreeDataStore::value(const AMnDIndex &scanIndex, int measurementId, const AMnDIndex &measurementIndex) const {

}

AMNumber AMDataTreeDataStore::axisValue(int axisId, int axisIndex) const {

}

bool AMDataTreeDataStore::setValue(const AMnDIndex &scanIndex, int measurementId, const AMnDIndex &measurementIndex, const AMNumber &newValue){

}

bool AMDataTreeDataStore::setAxisValue(int axisId, int axisIndex, AMNumber newValue){

}


bool AMDataTreeDataStore::setValue(const AMnDIndex &scanIndex, int measurementId, const int* inputData){

}

bool AMDataTreeDataStore::setValue(const AMnDIndex &scanIndex, int measurementId, const double* inputData){

}


bool AMDataTreeDataStore::beginInsertRowsImplementation(int axisId, int numRows, int atRowIndex){

}

void AMDataTreeDataStore::endInsertRowsImplementation(int axisId, int numRows, int atRowIndex){

}

void AMDataTreeDataStore::clearScanDataPointsImplementation(){

}


AMDataTree* AMDataTreeDataStore::measurementInfoToTree(const AMMeasurementInfo &measurementDetails, QList<AMAxisInfo> remainingAxes){
	if(remainingAxes.count() == 1){
		AMDataTree *treeBottom = new AMDataTree(remainingAxes.at(0).size, remainingAxes.at(0).name, true);
		//treeBottom->createColumn(remainingAxes.at(0).name, remainingAxes.at(0).description, remainingAxes.at(0).units);
		treeBottom->createColumn(measurementDetails.name, measurementDetails.description, measurementDetails.units);
		return treeBottom;
	}
	else{
		AMDataTree *notBottom = new AMDataTree(remainingAxes.at(0).size, remainingAxes.at(0).name, true);
		remainingAxes.pop_front();
		notBottom->createSubtreeColumn(remainingAxes.at(0).name, measurementInfoToTree(measurementDetails, remainingAxes));
		return notBottom;
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
