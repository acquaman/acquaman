#include "AMSamplePlate.h"

AMSamplePlate::AMSamplePlate(QObject *parent) :
		AMDbObject(parent)
{
	insertRowLatch = -1;
	setName("Sample Plate");
	metaData_["createTime"] = QDateTime::currentDateTime();

	samples_ = NULL;
	AMIntList tmpList;
	metaData_["sampleIDs"].setValue(tmpList);
	metaData_["positionIDs"].setValue(tmpList);
	setupModel();
}

AMSamplePlateModel* AMSamplePlate::model(){
	return samples_;
}

QString AMSamplePlate::plateName() const{
	return name() + " created " + timeString();
}

QString AMSamplePlate::userName() const{
	return name();
}

QDateTime AMSamplePlate::createTime() const{
	return metaData("createTime").toDateTime();
}

int AMSamplePlate::count() const {
	return samples_->count();
}



AMSample* AMSamplePlate::sampleAt(int index){
	AMSamplePosition *sp = samplePositionAt(index);
	if(sp)
		return sp->sample();
	return NULL;
}

AMSample* AMSamplePlate::sampleByName(const QString &name) {
	return sampleAt(model()->indexOfSampleName(name));
}


AMControlSetInfo* AMSamplePlate::positionAt(int index){
	AMSamplePosition *sp = samplePositionAt(index);
	if(sp)
		return sp->position();
	return NULL;
}

AMControlSetInfo* AMSamplePlate::positionByName(const QString &name){
	return positionAt(model()->indexOfSampleName(name));
}




QList<AMMetaMetaData> AMSamplePlate::metaDataUniqueKeys(){
	QList<AMMetaMetaData> rv;
	rv << AMMetaMetaData(QVariant::DateTime, "createTime", true);
	rv << AMMetaMetaData(AM::IntList, "sampleIDs", true);
	rv << AMMetaMetaData(AM::IntList, "positionIDs", true);
	return rv;
}

QList<AMMetaMetaData> AMSamplePlate::metaDataKeys(){
	return AMDbObject::metaDataKeys() << metaDataUniqueKeys();
}

QList<AMMetaMetaData> AMSamplePlate::metaDataAllKeys() const{
	return this->metaDataKeys();
}

QString AMSamplePlate::databaseTableName() const{
	return AMDatabaseDefinition::samplePlateTableName();
}

bool AMSamplePlate::loadFromDb(AMDatabase* db, int id){

	valid_ = false;

	if(!AMDbObject::loadFromDb(db, id)) {
		emit samplePlateChanged(false);
		return false;
	}


	while(count() > 0)
		removeSamplePosition(count()-1); /// \note Where do theses AMSample and AMControlSetInfo objects get deleted? They are children of AMSamplePlate, but we might be hanging onto this sample plate for a long time (for ex: life of the program, in the case of SGMBeamline::currentSamplePlate()... If you're calling loadFromDb a lot, the memory consumption will keep on increasing.  [for now: added delete to removeSamplePosition()]

	AMIntList sampleIDs = metaData_.value("sampleIDs").value<AMIntList>();
	AMIntList positionIDs = metaData_.value("positionIDs").value<AMIntList>();
	if(sampleIDs.count() != positionIDs.count()) {
		qDebug() << "Couldn't load sample plate: the number of samples " << sampleIDs.count() << "was not equal to the number of saved positions" << positionIDs.count();
		emit samplePlateChanged(false);
		return false;
	}
	AMSample *tmpSample;
	AMControlSetInfo *tmpPosition;
	for( int x = 0; x < sampleIDs.count(); x++){
		tmpSample = new AMSample(this);
		tmpPosition = new AMControlSetInfo(this);
		if( !tmpPosition->loadFromDb(AMDatabase::userdb(), positionIDs.at(x)) ){
			qDebug() << "Couldn't load sample plate positions at index " << x << ", CSI id = " << positionIDs.at(x);
			qDebug() << "  positionIds was" << positionIDs << "count was" << positionIDs.count();
			delete tmpSample;
			delete tmpPosition;
			emit samplePlateChanged(false);
			return false;
		}
		if( sampleIDs.at(x) != 0 && !tmpSample->loadFromDb(AMDatabase::userdb(), sampleIDs.at(x)) ){
			qDebug() << "Couldn't load sample plate sample at index " << x << ", Sample id = " << sampleIDs.at(x);
			qDebug() << "  sampleIds was" << sampleIDs << "count was " << sampleIDs.count();
			delete tmpSample;
			delete tmpPosition;
			emit samplePlateChanged(false);
			return false;
		}
		appendSamplePosition(tmpSample, tmpPosition);
	}

	valid_ = true;
	emit samplePlateChanged(true);
	return true;

}

bool AMSamplePlate::storeToDb(AMDatabase* db){
	AMIntList sampleIDs;
	AMIntList positionIDs;
	for(int x = 0; x < count(); x++){
		sampleIDs.append(sampleAt(x)->id());
		positionIDs.append(positionAt(x)->id());
	}
	metaData_["sampleIDs"].setValue(sampleIDs);
	metaData_["positionIDs"].setValue(positionIDs);

	return valid_ = AMDbObject::storeToDb(db);
}

QString AMSamplePlate::typeDescription() const{
	return "List of Samples and their Positions on a Sample Plate";
}





bool AMSamplePlate::addSamplePosition(int index, AMSamplePosition *sp){
	/// \note This check, if a general requirement, makes a lot of other error checking code redundant.
	if(!sp || !sp->sample() || !sp->position())
		return false;

	model()->insertSamplePosition(index, sp);

}

bool AMSamplePlate::addSamplePosition(int index, AMSample *sample, AMControlSetInfo *position){
	if(!sample || !position)
		return false;
	model()->insertSamplePosition(index, new AMSamplePosition(sample,position));
	return true;
}

bool AMSamplePlate::appendSamplePosition(AMSamplePosition *sp){
	return addSamplePosition(count(), sp);
}

bool AMSamplePlate::appendSamplePosition(AMSample *sample, AMControlSetInfo *position){
	return addSamplePosition(count(), sample, position);
}

/// \note This used to remove the first sample position with the same name as sp->sample()->name().  There might be quite a few situations where this wouldn't do what you think it would (since no one is enforcing unique sample names, and names are changeable.) Now it removes the exact same object that sp is.
bool AMSamplePlate::removeSamplePosition(AMSamplePosition *sp){
	return removeSamplePosition( model()->indexOf(sp) );
}

bool AMSamplePlate::removeSamplePosition(int index){
	if( index < 0 || index >= count())
		return false;

	return model()->removeRow(index);
}

void AMSamplePlate::onDataChanged(QModelIndex a, QModelIndex b){
	if(a.row() != b.row())
		return;
	if(insertRowLatch != -1 && insertRowLatch == a.row()){
		insertRowLatch = -1;
		emit samplePositionAdded(a.row());
	}
	else{
		qDebug() << "I see row " << a.row() << " changed in sample plate";
		emit samplePositionChanged(a.row());
	}
}

void AMSamplePlate::onRowsInserted(QModelIndex parent, int start, int end){
	Q_UNUSED(parent);
	if(start != end)
		return;
	insertRowLatch = start;
}

void AMSamplePlate::onRowsRemoved(QModelIndex parent, int start, int end){
	Q_UNUSED(parent);
	if(start != end)
		return;
	emit samplePositionRemoved(start);
}

void AMSamplePlate::setupModel(){
	samples_ = new AMSamplePlateModel(this);

	connect(samples_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onDataChanged(QModelIndex,QModelIndex)));
	connect(samples_, SIGNAL(rowsInserted(const QModelIndex,int, int)), this, SLOT(onRowsInserted(QModelIndex,int,int)));
	connect(samples_, SIGNAL(rowsRemoved(const QModelIndex,int,int)), this, SLOT(onRowsRemoved(QModelIndex,int,int)));

}

#include "ui/AMDateTimeUtils.h"

QString AMSamplePlate::timeString() const{
	return AMDateTimeUtils::prettyDateTime(metaData("createTime").toDateTime());
}



// AMSamplePlateModel
//////////////////////////////////////

AMSamplePlateModel::AMSamplePlateModel(QObject *parent) :
		QStandardItemModel(parent)
{

}
QVariant AMSamplePlateModel::data(const QModelIndex &index, int role) const{

	if(role == Qt::DisplayRole && index.isValid() && index.row() < rowCount() && index.column() == 0 && !index.parent().isValid()) {
		AMSamplePosition* sp = samplePositionAt(index.row());
		if(sp && sp->sample())
			return sp->sample()->name();
	}

	return QStandardItemModel::data(index,role);
}





/// Function to insert an AMSamplePosition. This model takes ownership of the sample position, and will delete it when removed from the model.  If row == -1, the sample will be appended.
void AMSamplePlateModel::insertSamplePosition(int row, AMSamplePosition* sp) {
	if(row<0 || row>count())
		row = count();
	QStandardItem* item = new QStandardItem();
	item->setData(qVariantFromValue(sp), AM::PointerRole);
	insertRow(row, item);

	if(sp) {
		sp->setParent(this);
		connect(sp, SIGNAL(sampleLoadedFromDb()), this, SLOT(onSampleLoadedFromDb()));
		connect(sp, SIGNAL(positionLoadedFromDb()), this, SLOT(onPositionLoadedFromDb()));
		connect(sp, SIGNAL(positionValuesChanged(int)), this, SLOT(onPositionLoadedFromDb()));
	}

}

bool AMSamplePlateModel::removeRows(int row, int count, const QModelIndex &parent) {

	// must be a top-level index:
	if(!parent.isValid()) {
		for(int i=row; i<row+count; i++) {
			AMSamplePosition* sp = samplePositionAt(i);
			if(sp)
				delete sp;
		}
	}

	return QStandardItemModel::removeRows(row, count, parent);
}

int AMSamplePlateModel::indexOf(AMSamplePosition *existing) const {
	for(int x = 0; x < count(); x++){
		if(samplePositionAt(x) == existing)
			return x;
	}
	return -1;
}

int AMSamplePlateModel::indexOfSampleName(const QString &sampleName) const {
	for(int x = 0; x < count(); x++){
		AMSamplePosition* sp = samplePositionAt(x);
		if(sp && sp->sample() && sp->sample()->name() == sampleName) {
			return x;
		}
	}
	return -1;
}

void AMSamplePlateModel::onSampleLoadedFromDb(){

	int row = indexOf(qobject_cast<AMSamplePosition*>(sender()));
	if(row != -1)
		emit dataChanged(indexForRow(row), indexForRow(row));
}

void AMSamplePlateModel::onPositionLoadedFromDb(){

	int row = indexOf(qobject_cast<AMSamplePosition*>(sender()));
	if(row != -1)
		emit dataChanged(indexForRow(row), indexForRow(row));
}



//////////////////////////////////////


AMSamplePosition::AMSamplePosition(AMSample *sample, AMControlSetInfo *position, QObject *parent) :
		QObject(parent)
{
	sample_ = sample;
	position_ = position;
	if(position_){
		connect(position_, SIGNAL(valuesChanged(int)), this, SIGNAL(positionValuesChanged(int)));
		connect(position_, SIGNAL(loadedFromDb()), this, SIGNAL(positionLoadedFromDb()));
	}
	if(sample_){
		connect(sample_, SIGNAL(loadedFromDb()), this, SIGNAL(sampleLoadedFromDb()));
	}
}

AMSample* AMSamplePosition::sample(){
	return sample_;
}

AMControlSetInfo* AMSamplePosition::position(){
	return position_;
}

void AMSamplePosition::setSample(AMSample *sample){
	if(sample_)
		disconnect(sample_, SIGNAL(loadedFromDb()), this, SIGNAL(sampleLoadedFromDb()));
	sample_ = sample;
	if(sample_)
		connect(sample_, SIGNAL(loadedFromDb()), this, SIGNAL(sampleLoadedFromDb()));
}

void AMSamplePosition::setPosition(AMControlSetInfo *position){
	if(position_){
		disconnect(position_, SIGNAL(valuesChanged(int)), this, SIGNAL(positionValuesChanged(int)));
		disconnect(position_, SIGNAL(loadedFromDb()), this, SIGNAL(positionLoadedFromDb()));
	}
	position_ = position;
	if(position_){
		connect(position_, SIGNAL(valuesChanged(int)), this, SIGNAL(positionValuesChanged(int)));
		connect(position_, SIGNAL(loadedFromDb()), this, SIGNAL(positionLoadedFromDb()));
	}
}
