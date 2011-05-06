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


#include "AMSamplePlate.h"
#include "util/AMErrorMonitor.h"
#include "ui/AMDateTimeUtils.h"

AMSamplePlate::AMSamplePlate(QObject *parent) : AMDbObject(parent), AMOrderedList<AMSamplePosition>() {

	setName("New Sample Plate");
	dateTime_ = QDateTime::currentDateTime();

	// Forward internal signals (itemAdded, etc.) from our signalSource() as our own
	connect(signalSource(), SIGNAL(itemAboutToBeAdded(int)), this, SIGNAL(samplePositionAboutToBeAdded(int)));
	connect(signalSource(), SIGNAL(itemAdded(int)), this, SIGNAL(samplePositionAdded(int)));
	connect(signalSource(), SIGNAL(itemAboutToBeRemoved(int)), this, SIGNAL(samplePositionAboutToBeRemoved(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SIGNAL(samplePositionRemoved(int)));
	connect(signalSource(), SIGNAL(itemChanged(int)), this, SIGNAL(samplePositionChanged(int)));
}

AMSamplePlate::AMSamplePlate(const AMSamplePlate& other) : AMDbObject(), AMOrderedList<AMSamplePosition>(other) {
	// Forward internal signals (itemAdded, etc.) from our signalSource() as our own
	connect(signalSource(), SIGNAL(itemAboutToBeAdded(int)), this, SIGNAL(samplePositionAboutToBeAdded(int)));
	connect(signalSource(), SIGNAL(itemAdded(int)), this, SIGNAL(samplePositionAdded(int)));
	connect(signalSource(), SIGNAL(itemAboutToBeRemoved(int)), this, SIGNAL(samplePositionAboutToBeRemoved(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SIGNAL(samplePositionRemoved(int)));
	connect(signalSource(), SIGNAL(itemChanged(int)), this, SIGNAL(samplePositionChanged(int)));
}

// Using auto-generated assignment operator is fine


int AMSamplePlate::sampleIdAtPosition(const AMControlInfoList &position, const QList<double> tolerances) const{
	if(tolerances.count() == 0){
		for(int x = count()-1; x >= 0; x--)
			if( at(x).position() == position )
				return at(x).sampleId();
	}
	else{
		for(int x = count()-1; x >= 0; x--)
			if( at(x).position().compareWithinTolerance(position, tolerances))
				return at(x).sampleId();
	}
	return -1;
}


// Export the current positions to the database
AMDbObjectList AMSamplePlate::dbGetPositions() {
	AMDbObjectList rv;

	for(int i=0; i<count(); i++)
		rv << &((*this)[i]);	/// \todo This uses the non-const operator[], which means that an itemChanged() signal will be emitted for all i when we're done. This signal is not necessary; does it create a loop or a performance problem? Need to watch what gets hooked up to samplePositionChanged().

	return rv;
}


// Load the positions for an existing sample plate from the database
void AMSamplePlate::dbLoadPositions(const AMDbObjectList& newPositions) {

	clear();	// get rid of our existing

	for(int i=0; i<newPositions.count(); i++) {
		AMSamplePosition* newSamplePosition = qobject_cast<AMSamplePosition*>(newPositions.at(i));
		if(newSamplePosition) {
			append(*newSamplePosition);	// note: makes a copy of object pointed to by newControlInfo, and stores in our internal list. Uses default copy constructor in AMControlInfoList
		}
		else {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -1, "Invalid position object found when trying to re-load from the database. Please report this bug to the Acquaman developers."));
		}

		if(newPositions.at(i))
			delete newPositions.at(i);	// we're copying these; don't need to keep these ones around. Our responsibility to delete.
	}
}




//AMSamplePlate::AMSamplePlate(QObject *parent) :
//		AMDbObject(parent)
//{
//	setName("Sample Plate");
//	createTime_ = QDateTime::currentDateTime();
//}



//QDateTime AMSamplePlate::createTime() const{
//	return createTime_;
//}

//int AMSamplePlate::count(){
//	return samples_->rowCount(QModelIndex());
//}

//AMSamplePosition* AMSamplePlate::samplePositionAt(int index){
//	QVariant retVal = samples_->data(samples_->index(index, 0), Qt::DisplayRole);
//	if(retVal.isValid())
//		return (AMSamplePosition*) retVal.value<void*>();
//	return NULL;
//}

//AMSamplePosition* AMSamplePlate::samplePositionByName(const QString &name){
//	if(sampleName2samplePosition_.containsF(name))
//		return sampleName2samplePosition_.valueF(name);
//	return NULL;
//}

//AMSample* AMSamplePlate::sampleAt(int index){
//	AMSamplePosition *sp = samplePositionAt(index);
//	if(sp)
//		return sp->sample();
//	return NULL;
//}

//AMSample* AMSamplePlate::sampleByName(const QString &name){
//	if(sampleName2samplePosition_.containsF(name))
//		return samplePositionByName(name)->sample();
//	return NULL;
//}

//AMControlInfoList* AMSamplePlate::positionAt(int index){
//	AMSamplePosition *sp = samplePositionAt(index);
//	if(sp)
//		return sp->position();
//	return NULL;
//}

//AMControlInfoList* AMSamplePlate::positionByName(const QString &name){
//	if(sampleName2samplePosition_.containsF(name))
//		return samplePositionByName(name)->position();
//	return NULL;
//}

//int AMSamplePlate::indexOf(const QString &name){
//	if(sampleName2samplePosition_.containsF(name)){
//		AMSamplePosition *sp = sampleName2samplePosition_.valueF(name);
//		for(int x = 0; x < count(); x++)
//			if(samplePositionAt(x) == sp)
//				return x;
//	}
//	return -1;
//}



//bool AMSamplePlate::loadFromDb(AMDatabase* db, int id){

//	valid_ = false;

//	if(!AMDbObject::loadFromDb(db, id)) {
//		emit samplePlateChanged(false);
//		return false;
//	}


//	while(count() > 0)
//		removeSamplePosition(count()-1); /// \note Where do theses AMSample and AMControlSetInfo objects get deleted? They are children of AMSamplePlate, but we might be hanging onto this sample plate for a long time (for ex: life of the program, in the case of SGMBeamline::currentSamplePlate()... If you're calling loadFromDb a lot, the memory consumption will keep on increasing.  [for now: added delete to removeSamplePosition()]

//	// AMIntList sampleIDs__ and positionIDs__ have been loaded from the DB.
//	if(sampleIDs_.count() != positionIDs_.count()) {
//		qDebug() << "Couldn't load sample plate: the number of samples " << sampleIDs_.count() << "was not equal to the number of saved positions" << positionIDs_.count();
//		emit samplePlateChanged(false);
//		return false;
//	}
//	AMSample *tmpSample;
//	AMControlInfoList *tmpPosition;
//	for( int x = 0; x < sampleIDs_.count(); x++){
//		tmpSample = new AMSample(this);
//		tmpPosition = new AMControlInfoList(this);
//		if( !tmpPosition->loadFromDb(AMDatabase::userdb(), positionIDs_.at(x)) ){
//			qDebug() << "Couldn't load sample plate positions at index " << x << ", CSI id = " << positionIDs_.at(x); /// \todo if this a production-possible error, use AMErrorMon::report()
//			qDebug() << "  positionIDs_ was" << positionIDs_ << "count was" << positionIDs_.count();
//			delete tmpSample;
//			delete tmpPosition;
//			emit samplePlateChanged(false);
//			return false;
//		}
//		if( sampleIDs_.at(x) != 0 && !tmpSample->loadFromDb(AMDatabase::userdb(), sampleIDs_.at(x)) ){
//			qDebug() << "Couldn't load sample plate sample at index " << x << ", Sample id = " << sampleIDs_.at(x);
//			qDebug() << "  sampleIDs_ was" << sampleIDs_ << "count was " << sampleIDs_.count();
//			delete tmpSample;
//			delete tmpPosition;
//			emit samplePlateChanged(false);
//			return false;
//		}
//		appendSamplePosition(tmpSample, tmpPosition);
//	}

//	valid_ = true;
//	emit samplePlateChanged(true);
//	return true;

//}

//bool AMSamplePlate::storeToDb(AMDatabase* db){
//	// get sampleIDs and positionIDs up to date. They will be saved automatically as properties.
//	for(int x = 0; x < count(); x++){
//		sampleIDs_.append(sampleAt(x)->id());
//		positionIDs_.append(positionAt(x)->id());
//	}

//	return valid_ = AMDbObject::storeToDb(db);
//}





//bool AMSamplePlate::setSamplePosition(int index, AMSamplePosition *sp){
//	AMSamplePosition *tmpSP = samplePositionAt(index);
//	bool retVal = samples_->setData(samples_->index(index, 0), qVariantFromValue((void*)sp), Qt::EditRole);
//	if(retVal){
//		sampleName2samplePosition_.removeR(tmpSP);
//		sampleName2samplePosition_.set(sp->sample()->name(), sp);
//	}
//	return retVal;
//}

//bool AMSamplePlate::addSamplePosition(int index, AMSamplePosition *sp){
//	if(!sp || !sp->sample() || !sp->position())
//		return false;
//	else if(sampleName2samplePosition_.containsR(sp))
//		return false;
//	else if(!samples_->insertRows(index, 1))
//		return false;
//	return setSamplePosition(index, sp);
//}

//bool AMSamplePlate::addSamplePosition(int index, AMSample *sample, AMControlInfoList *position){
//	if(!sample || !position)
//		return false;
//	AMSamplePosition *tmpSP = new AMSamplePosition(sample, position, this);
//	return addSamplePosition(index, tmpSP);
//}

//bool AMSamplePlate::appendSamplePosition(AMSamplePosition *sp){
//	return addSamplePosition(count(), sp);
//}

//bool AMSamplePlate::appendSamplePosition(AMSample *sample, AMControlInfoList *position){
//	return addSamplePosition(count(), sample, position);
//}

//bool AMSamplePlate::removeSamplePosition(AMSamplePosition *sp){
//	return removeSamplePosition( indexOf(sp->sample()->name()) );
//}

//bool AMSamplePlate::removeSamplePosition(int index){
//	if( (int)index >= count())
//		return false;
//	AMSamplePosition *rSP = samplePositionAt(index);
//	bool retVal = samples_->removeRows(index, 1);
//	if(retVal) {
//		sampleName2samplePosition_.removeR(rSP);
//#warning "David to double check: can I delete this?  And would it be okay for AMSamplePosition to take ownership of its sample, and position, and delete them itself?"
//		delete rSP->sample();
//		delete rSP->position();
//		delete rSP;
//	}
//	return retVal;
//}

//void AMSamplePlate::onDataChanged(QModelIndex a, QModelIndex b){
//	if(a.row() != b.row())
//		return;
//	if(insertRowLatch != -1 && insertRowLatch == a.row()){
//		insertRowLatch = -1;
//		emit samplePositionAdded(a.row());
//	}
//	else{
//		qDebug() << "I see row " << a.row() << " changed in sample plate";
//		emit samplePositionChanged(a.row());
//	}
//}

//void AMSamplePlate::onRowsInserted(QModelIndex parent, int start, int end){
//	Q_UNUSED(parent);
//	if(start != end)
//		return;
//	insertRowLatch = start;
//}

//void AMSamplePlate::onRowsRemoved(QModelIndex parent, int start, int end){
//	Q_UNUSED(parent);
//	if(start != end)
//		return;
//	emit samplePositionRemoved(start);
//}

//bool AMSamplePlate::setupModel(){
//	samples_ = new AMSamplePlateModel(this);
//	if(samples_){
//		connect(samples_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onDataChanged(QModelIndex,QModelIndex)));
//		connect(samples_, SIGNAL(rowsInserted(const QModelIndex,int, int)), this, SLOT(onRowsInserted(QModelIndex,int,int)));
//		connect(samples_, SIGNAL(rowsRemoved(const QModelIndex,int,int)), this, SLOT(onRowsRemoved(QModelIndex,int,int)));
//		return true;
//	}
//	return false;
//}

//#include "ui/AMDateTimeUtils.h"


//AMSamplePlateModel::AMSamplePlateModel(QObject *parent) :
//		QAbstractListModel(parent)
//{
//	samples_ = new QList<AMSamplePosition*>();
//}

//int AMSamplePlateModel::rowCount(const QModelIndex & /*parent*/) const{
//	return samples_->count();
//}

//QVariant AMSamplePlateModel::data(const QModelIndex &index, int role) const{
//	if(!index.isValid())
//		return QVariant();
//	if(index.row() >= samples_->count())
//		return QVariant();
//	if(role == Qt::DisplayRole)
//		return qVariantFromValue((void*)samples_->at(index.row()));
//	else
//		return QVariant();
//}

//QVariant AMSamplePlateModel::headerData(int section, Qt::Orientation orientation, int role) const{
//	if (role != Qt::DisplayRole)
//		return QVariant();

//	if (orientation == Qt::Horizontal)
//		return QString("Column %1").arg(section);
//	else
//		return QString("Row %1").arg(section);
//}

//bool AMSamplePlateModel::setData(const QModelIndex &index, const QVariant &value, int role){
//	if (index.isValid()  && index.row() < samples_->count() && role == Qt::EditRole) {
//		AMSamplePosition *oldSP = (AMSamplePosition*)data(index, role).value<void*>();
//		if(oldSP){
//			disconnect(oldSP, SIGNAL(sampleLoadedFromDb()), this, SLOT(onSampleLoadedFromDb()));
//			disconnect(oldSP, SIGNAL(positionLoadedFromDb()), this, SLOT(onPositionLoadedFromDb()));
//		}
//		AMSamplePosition *sp;
//		sp = (AMSamplePosition*) value.value<void*>();

//		samples_->replace(index.row(), sp);
//		if(sp){
//			connect(sp, SIGNAL(sampleLoadedFromDb()), this, SLOT(onSampleLoadedFromDb()));
//			connect(sp, SIGNAL(positionLoadedFromDb()), this, SLOT(onPositionLoadedFromDb()));
//		}
//		emit dataChanged(index, index);
//		return true;
//	}
//	return false;	// no value set
//}

//bool AMSamplePlateModel::insertRows(int position, int rows, const QModelIndex &index){
//	if (index.row() <= samples_->count() && position <= samples_->count()) {
//		beginInsertRows(QModelIndex(), position, position+rows-1);
//		AMSamplePosition *sp = NULL;
//		for (int row = 0; row < rows; ++row) {
//			samples_->insert(position, sp);
//		}
//		endInsertRows();
//		return true;
//	}
//	return false;
//}

//bool AMSamplePlateModel::removeRows(int position, int rows, const QModelIndex &index){
//	if (index.row() < samples_->count() && position < samples_->count()) {
//		beginRemoveRows(QModelIndex(), position, position+rows-1);
//		for (int row = 0; row < rows; ++row) {
//			samples_->removeAt(position);
//		}
//		endRemoveRows();
//		return true;
//	}
//	return false;
//}

//Qt::ItemFlags AMSamplePlateModel::flags(const QModelIndex &index) const{
//	Qt::ItemFlags flags;
//	if (index.isValid() && index.row() < samples_->count() && index.column()<4)
//		flags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
//	return flags;
//}

//void AMSamplePlateModel::onSampleLoadedFromDb(){
//	AMSamplePosition *sp = (AMSamplePosition*)QObject::sender();
//	for(int x = 0; x < samples_->count(); x++){
//		if(samples_->at(x) == sp)
//			emit dataChanged(index(x, 0), index(x, 0));
//	}
//}

//void AMSamplePlateModel::onPositionLoadedFromDb(){
//	AMSamplePosition *sp = (AMSamplePosition*)QObject::sender();
//	for(int x = 0; x < samples_->count(); x++){
//		if(samples_->at(x) == sp)
//			emit dataChanged(index(x, 0), index(x, 0));
//	}
//}

