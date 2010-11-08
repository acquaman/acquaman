#include "AMSamplePlate.h"

AMSamplePlate::AMSamplePlate(QObject *parent) :
		AMDbObject(parent)
{
	insertRowLatch = -1;
	setName("Sample Plate");
	createTime_ = QDateTime::currentDateTime();

	samples_ = NULL;
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
	return createTime_;
}

int AMSamplePlate::count(){
	return samples_->rowCount(QModelIndex());
}

AMSamplePosition* AMSamplePlate::samplePositionAt(size_t index){
	QVariant retVal = samples_->data(samples_->index(index, 0), Qt::DisplayRole);
	if(retVal.isValid())
		return (AMSamplePosition*) retVal.value<void*>();
	return NULL;
}

AMSamplePosition* AMSamplePlate::samplePositionByName(const QString &name){
	if(sampleName2samplePosition_.containsF(name))
		return sampleName2samplePosition_.valueF(name);
	return NULL;
}

AMSample* AMSamplePlate::sampleAt(size_t index){
	AMSamplePosition *sp = samplePositionAt(index);
	if(sp)
		return sp->sample();
	return NULL;
}

AMSample* AMSamplePlate::sampleByName(const QString &name){
	if(sampleName2samplePosition_.containsF(name))
		return samplePositionByName(name)->sample();
	return NULL;
}

AMControlSetInfo* AMSamplePlate::positionAt(size_t index){
	AMSamplePosition *sp = samplePositionAt(index);
	if(sp)
		return sp->position();
	return NULL;
}

AMControlSetInfo* AMSamplePlate::positionByName(const QString &name){
	if(sampleName2samplePosition_.containsF(name))
		return samplePositionByName(name)->position();
	return NULL;
}

int AMSamplePlate::indexOf(const QString &name){
	if(sampleName2samplePosition_.containsF(name)){
		AMSamplePosition *sp = sampleName2samplePosition_.valueF(name);
		for(int x = 0; x < count(); x++)
			if(samplePositionAt(x) == sp)
				return x;
	}
	return -1;
}



bool AMSamplePlate::loadFromDb(AMDatabase* db, int id){

	valid_ = false;

	if(!AMDbObject::loadFromDb(db, id)) {
		emit samplePlateChanged(false);
		return false;
	}


	while(count() > 0)
		removeSamplePosition(count()-1); /// \note Where do theses AMSample and AMControlSetInfo objects get deleted? They are children of AMSamplePlate, but we might be hanging onto this sample plate for a long time (for ex: life of the program, in the case of SGMBeamline::currentSamplePlate()... If you're calling loadFromDb a lot, the memory consumption will keep on increasing.  [for now: added delete to removeSamplePosition()]

	// AMIntList sampleIDs__ and positionIDs__ have been loaded from the DB.
	if(sampleIDs_.count() != positionIDs_.count()) {
		qDebug() << "Couldn't load sample plate: the number of samples " << sampleIDs_.count() << "was not equal to the number of saved positions" << positionIDs_.count();
		emit samplePlateChanged(false);
		return false;
	}
	AMSample *tmpSample;
	AMControlSetInfo *tmpPosition;
	for( int x = 0; x < sampleIDs_.count(); x++){
		tmpSample = new AMSample(this);
		tmpPosition = new AMControlSetInfo(this);
		if( !tmpPosition->loadFromDb(AMDatabase::userdb(), positionIDs_.at(x)) ){
			qDebug() << "Couldn't load sample plate positions at index " << x << ", CSI id = " << positionIDs_.at(x); /// \todo if this a production-possible error, use AMErrorMon::report()
			qDebug() << "  positionIDs_ was" << positionIDs_ << "count was" << positionIDs_.count();
			delete tmpSample;
			delete tmpPosition;
			emit samplePlateChanged(false);
			return false;
		}
		if( sampleIDs_.at(x) != 0 && !tmpSample->loadFromDb(AMDatabase::userdb(), sampleIDs_.at(x)) ){
			qDebug() << "Couldn't load sample plate sample at index " << x << ", Sample id = " << sampleIDs_.at(x);
			qDebug() << "  sampleIDs_ was" << sampleIDs_ << "count was " << sampleIDs_.count();
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
	// get sampleIDs and positionIDs up to date. They will be saved automatically as properties.
	for(int x = 0; x < count(); x++){
		sampleIDs_.append(sampleAt(x)->id());
		positionIDs_.append(positionAt(x)->id());
	}

	return valid_ = AMDbObject::storeToDb(db);
}





bool AMSamplePlate::setSamplePosition(size_t index, AMSamplePosition *sp){
	AMSamplePosition *tmpSP = samplePositionAt(index);
	bool retVal = samples_->setData(samples_->index(index, 0), qVariantFromValue((void*)sp), Qt::EditRole);
	if(retVal){
		sampleName2samplePosition_.removeR(tmpSP);
		sampleName2samplePosition_.set(sp->sample()->name(), sp);
	}
	return retVal;
}

bool AMSamplePlate::addSamplePosition(size_t index, AMSamplePosition *sp){
	if(!sp || !sp->sample() || !sp->position())
		return false;
	else if(sampleName2samplePosition_.containsR(sp))
		return false;
	else if(!samples_->insertRows(index, 1))
		return false;
	return setSamplePosition(index, sp);
}

bool AMSamplePlate::addSamplePosition(size_t index, AMSample *sample, AMControlSetInfo *position){
	if(!sample || !position)
		return false;
	AMSamplePosition *tmpSP = new AMSamplePosition(sample, position, this);
	return addSamplePosition(index, tmpSP);
}

bool AMSamplePlate::appendSamplePosition(AMSamplePosition *sp){
	return addSamplePosition(count(), sp);
}

bool AMSamplePlate::appendSamplePosition(AMSample *sample, AMControlSetInfo *position){
	return addSamplePosition(count(), sample, position);
}

bool AMSamplePlate::removeSamplePosition(AMSamplePosition *sp){
	return removeSamplePosition( indexOf(sp->sample()->name()) );
}

bool AMSamplePlate::removeSamplePosition(size_t index){
	if( (int)index >= count())
		return false;
	AMSamplePosition *rSP = samplePositionAt(index);
	bool retVal = samples_->removeRows(index, 1);
	if(retVal) {
		sampleName2samplePosition_.removeR(rSP);
#warning "David to double check: can I delete this?  And would it be okay for AMSamplePosition to take ownership of its sample, and position, and delete them itself?"
		delete rSP->sample();
		delete rSP->position();
		delete rSP;
	}
	return retVal;
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

bool AMSamplePlate::setupModel(){
	samples_ = new AMSamplePlateModel(this);
	if(samples_){
		connect(samples_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onDataChanged(QModelIndex,QModelIndex)));
		connect(samples_, SIGNAL(rowsInserted(const QModelIndex,int, int)), this, SLOT(onRowsInserted(QModelIndex,int,int)));
		connect(samples_, SIGNAL(rowsRemoved(const QModelIndex,int,int)), this, SLOT(onRowsRemoved(QModelIndex,int,int)));
		return true;
	}
	return false;
}

#include "ui/AMDateTimeUtils.h"

QString AMSamplePlate::timeString() const{
	return AMDateTimeUtils::prettyDateTime(createTime_);
}

AMSamplePlateModel::AMSamplePlateModel(QObject *parent) :
		QAbstractListModel(parent)
{
	samples_ = new QList<AMSamplePosition*>();
}

int AMSamplePlateModel::rowCount(const QModelIndex & /*parent*/) const{
	return samples_->count();
}

QVariant AMSamplePlateModel::data(const QModelIndex &index, int role) const{
	if(!index.isValid())
		return QVariant();
	if(index.row() >= samples_->count())
		return QVariant();
	if(role == Qt::DisplayRole)
		return qVariantFromValue((void*)samples_->at(index.row()));
	else
		return QVariant();
}

QVariant AMSamplePlateModel::headerData(int section, Qt::Orientation orientation, int role) const{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString("Column %1").arg(section);
	else
		return QString("Row %1").arg(section);
}

bool AMSamplePlateModel::setData(const QModelIndex &index, const QVariant &value, int role){
	if (index.isValid()  && index.row() < samples_->count() && role == Qt::EditRole) {
		AMSamplePosition *oldSP = (AMSamplePosition*)data(index, role).value<void*>();
		if(oldSP){
			disconnect(oldSP, SIGNAL(sampleLoadedFromDb()), this, SLOT(onSampleLoadedFromDb()));
			disconnect(oldSP, SIGNAL(positionLoadedFromDb()), this, SLOT(onPositionLoadedFromDb()));
		}
		AMSamplePosition *sp;
		sp = (AMSamplePosition*) value.value<void*>();

		samples_->replace(index.row(), sp);
		if(sp){
			connect(sp, SIGNAL(sampleLoadedFromDb()), this, SLOT(onSampleLoadedFromDb()));
			connect(sp, SIGNAL(positionLoadedFromDb()), this, SLOT(onPositionLoadedFromDb()));
		}
		emit dataChanged(index, index);
		return true;
	}
	return false;	// no value set
}

bool AMSamplePlateModel::insertRows(int position, int rows, const QModelIndex &index){
	if (index.row() <= samples_->count() && position <= samples_->count()) {
		beginInsertRows(QModelIndex(), position, position+rows-1);
		AMSamplePosition *sp = NULL;
		for (int row = 0; row < rows; ++row) {
			samples_->insert(position, sp);
		}
		endInsertRows();
		return true;
	}
	return false;
}

bool AMSamplePlateModel::removeRows(int position, int rows, const QModelIndex &index){
	if (index.row() < samples_->count() && position < samples_->count()) {
		beginRemoveRows(QModelIndex(), position, position+rows-1);
		for (int row = 0; row < rows; ++row) {
			samples_->removeAt(position);
		}
		endRemoveRows();
		return true;
	}
	return false;
}

Qt::ItemFlags AMSamplePlateModel::flags(const QModelIndex &index) const{
	Qt::ItemFlags flags;
	if (index.isValid() && index.row() < samples_->count() && index.column()<4)
		flags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	return flags;
}

void AMSamplePlateModel::onSampleLoadedFromDb(){
	AMSamplePosition *sp = (AMSamplePosition*)QObject::sender();
	for(int x = 0; x < samples_->count(); x++){
		if(samples_->at(x) == sp)
			emit dataChanged(index(x, 0), index(x, 0));
	}
}

void AMSamplePlateModel::onPositionLoadedFromDb(){
	AMSamplePosition *sp = (AMSamplePosition*)QObject::sender();
	for(int x = 0; x < samples_->count(); x++){
		if(samples_->at(x) == sp)
			emit dataChanged(index(x, 0), index(x, 0));
	}
}

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
