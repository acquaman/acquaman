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


#include "AMSamplePlate.h"

#include <QApplication>
#include <QStyle>

#include "util/AMDateTimeUtils.h"
#include "beamline/camera/AMShapeData.h"

AMSamplePlate::AMSamplePlate(QObject *parent) :
	AMDbObject(parent)
{
	dateTime_ = QDateTime::currentDateTime();

	// Forward internal signals (itemAdded, etc.) from our signalSource() as our own
	connect(samples_.signalSource(), SIGNAL(itemAboutToBeAdded(int)), this, SIGNAL(sampleAboutToBeAdded(int)));
	connect(samples_.signalSource(), SIGNAL(itemAdded(int)), this, SIGNAL(sampleAdded(int)));
	connect(samples_.signalSource(), SIGNAL(itemAboutToBeRemoved(int)), this, SIGNAL(sampleAboutToBeRemoved(int)));
	connect(samples_.signalSource(), SIGNAL(itemRemoved(int)), this, SIGNAL(sampleRemoved(int)));
	connect(samples_.signalSource(), SIGNAL(itemChanged(int)), this, SIGNAL(sampleChanged(int)));
}

AMSamplePlate::~AMSamplePlate()
{
}

QDateTime AMSamplePlate::dateTime() const{
	return dateTime_;
}

int AMSamplePlate::sampleCount() const{
	return samples_.count();
}

const AMSample* AMSamplePlate::sampleAt(int index) const{
	if(index == -1)
		return samples_.at(samples_.count()-1);
	if(index < 0 || index > sampleCount())
		return 0; //NULL
	return samples_.at(index);
}

AMSample* AMSamplePlate::sampleAt(int index){
	if(index == -1)
		return samples_.at(samples_.count()-1);
	if(index < 0 || index > sampleCount())
		return 0; //NULL
	return samples_.at(index);
}

QList<AMSample*> AMSamplePlate::allSamples(){
	return samples_.toList();
}

QVector3D AMSamplePlate::platePosition()
{
	return platePosition_;
}

double AMSamplePlate::plateRotation()
{
	return plateRotation_;
}

bool AMSamplePlate::addSample(AMSample *sample){
	samples_.append(sample);
	sample->setSamplePlate(this);
	connect(sample, SIGNAL(sampleDetailsChanged()), this, SLOT(onSampleDetailsChanged()));
	connect(sample, SIGNAL(modifiedChanged(bool)), this, SLOT(onSampleModified(bool)));
	connect(sample, SIGNAL(requestStoreToDb()), this, SLOT(onSampleRequestStoreToDb()));
	storeToDb(database());
	return true;
}

bool AMSamplePlate::removeSample(AMSample *sample){
	int sampleIndex = indexOfSample(sample);
	if(sampleIndex != -1){
		samples_.remove(sampleIndex);
		disconnect(sample, SIGNAL(sampleDetailsChanged()), this, SLOT(onSampleDetailsChanged()));
		disconnect(sample, SIGNAL(modifiedChanged(bool)), this, SLOT(onSampleModified(bool)));
		disconnect(sample, SIGNAL(requestStoreToDb()), this, SLOT(onSampleRequestStoreToDb()));
		storeToDb(database());
		sample->removeSampleShapePositionData();
		sample->deleteLater();
		return true;
	}
	return false;
}

int AMSamplePlate::indexOfSample(const AMSample *sample){
	if(!sample)
		return -1;
	for(int x = 0; x < samples_.count(); x++)
		if(samples_.at(x) == sample)
			return x;
	return -1;
}

bool AMSamplePlate::operator ==(const AMSamplePlate &other){
	if((name() == other.name()) && (sampleCount() == other.sampleCount())){
		for(int x = 0; x < sampleCount(); x++)
			if(sampleAt(x)->name() != other.sampleAt(x)->name())
				return false;
		return true;
	}
	return false;
}

void AMSamplePlate::onSampleCameraShapesChanged(const QList<AMShapeData*> shapeList){
	AMSample *sample;
	// if there aren't enough samples on the sample plate add some.
	if(shapeList.count() > sampleCount())
	{
		foreach(AMShapeData* shape, shapeList)
		{
			if(!sampleFromShape(shape))
			{
				shape->blockSignals(true);
				AMSample* newSample = new AMSample();
				newSample->setSampleShapePositionData(shape);
				newSample->setName(shape->name());
				addSample(newSample);
				shape->blockSignals(false);
				emit sampleAddedThroughCamera(newSample);
			}
		}
	}
	// if there are too many samples on the sample plate delete some.
	else if(shapeList.count() < sampleCount())
	{
		for(int i = 0; i < samples_.count(); i ++)
		{
			sample = samples_[i];
			if(!shapeList.contains(sample->sampleShapePositionData()))
			{
				removeSample(sample);
			}
		}
	}

}

void AMSamplePlate::onShapeDataPropertyUpdated(AMShapeData *shapeData){
	AMSample *sample = sampleFromShape(shapeData);
	if(sample)
		sample->storeToDb(database());
}

void AMSamplePlate::requestSampleMoveTo(int index){
	emit sampleMoveToRequested(sampleAt(index)->sampleShapePositionData());
}

void AMSamplePlate::setPlatePosition(QVector3D position)
{
	if(position != platePosition_)
	{
		platePosition_ = position;
		setModified(true);
	}
}

void AMSamplePlate::setPlateRotation(double rotation)
{
	if(rotation != plateRotation_)
	{
		plateRotation_ = rotation;
		setModified(true);
	}
}

void AMSamplePlate::onSampleDetailsChanged(){
	AMSample *sample = qobject_cast<AMSample*>(QObject::sender());
	if(sample)
		emit sampleChanged(indexOfSample(sample));
}

void AMSamplePlate::onSampleModified(bool isModified){
	Q_UNUSED(isModified)
	bool modifiedBefore = modified();
	bool modifiedNow = false;
	for(int x = 0; x < sampleCount(); x++)
		modifiedNow |= sampleAt(x)->modified();

	if(modifiedNow != modifiedBefore)
		setModified(modifiedNow);
}

void AMSamplePlate::onSampleRequestStoreToDb(){
	storeToDb(database());
}

AMSample* AMSamplePlate::sampleFromShape(AMShapeData *shapeData){
	for(int x = 0; x < sampleCount(); x++)
		if(sampleAt(x)->sampleShapePositionData() == shapeData)
			return sampleAt(x);
	return 0; //NULL
}

void AMSamplePlate::dbLoadDateTime(const QDateTime &newDateTime){
	dateTime_ = newDateTime;
}

AMDbObjectList AMSamplePlate::dbGetSamples(){
	AMDbObjectList retVal;
	for(int x = 0; x < samples_.count(); x++)
		retVal << samples_[x];
	return retVal;
}

void AMSamplePlate::dbLoadSamples(const AMDbObjectList &newSamples){
	samples_.clear();

	for(int x = 0; x < newSamples.count(); x++){
		AMSample *newSample = qobject_cast<AMSample*>(newSamples.at(x));
		if(newSample){
			samples_.append(newSample);
			connect(newSample, SIGNAL(sampleDetailsChanged()), this, SLOT(onSampleDetailsChanged()));
			connect(newSample, SIGNAL(modifiedChanged(bool)), this, SLOT(onSampleModified(bool)));
			connect(newSample, SIGNAL(requestStoreToDb()), this, SLOT(onSampleRequestStoreToDb()));
		}
	}
}


AMSamplePlateBrowser::AMSamplePlateBrowser(AMDatabase *database, QObject *parent) :
	QAbstractListModel(parent)
{
	currentSamplePlateIndex_ = -1;
	database_ = database;
	reloadFromDatabase();
}

int AMSamplePlateBrowser::rowCount(const QModelIndex &parent) const
{
	if(parent.isValid())
		return 0;
	return allSamplePlates_.count();
}


QVariant AMSamplePlateBrowser::data(const QModelIndex &index, int role) const
{
	if(index.parent().isValid() || index.column() > 0)
		return QVariant();

	switch(role){
	case Qt::DisplayRole:
		return QString("%1 (created %2)").arg(allSamplePlates_.at(index.row())->name()).arg(AMDateTimeUtils::prettyDateTime(allSamplePlates_.at(index.row())->dateTime()));
	case Qt::DecorationRole:
		if(index.row() == currentSamplePlateIndex_)
			return QApplication::style()->standardIcon(QStyle::SP_ArrowRight);
		else
			return QVariant();
	case AM::PointerRole:
		return QVariant::fromValue((QObject*)(allSamplePlates_.at(index.row())));
	default:
		return QVariant();
	}
}

Qt::ItemFlags AMSamplePlateBrowser::flags(const QModelIndex &index) const
{
	Q_UNUSED(index)
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant AMSamplePlateBrowser::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section)
	if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return QString("Sample Plates");
	else
		return QVariant();
}

bool AMSamplePlateBrowser::hasSamplePlate(AMSamplePlate *samplePlate){
	for(int x = 0; x < allSamplePlates_.count(); x++)
		if( (allSamplePlates_.at(x) == samplePlate) || (allSamplePlates_.at(x)->operator ==(*samplePlate)) )
			return true;
	return false;
}

QList<AMSample*> AMSamplePlateBrowser::allSamples(){
	QList<AMSample*> retVal;
	for(int x = 0; x < allSamplePlates_.count(); x++)
		retVal.append(allSamplePlates_.at(x)->allSamples());
	return retVal;
}

void AMSamplePlateBrowser::reloadFromDatabase(){
	allSamplePlates_.clear();
	if(database_){
		QList<int> allSamplePlateIds = database_->objectsWhere(AMDbObjectSupport::s()->tableNameForClass<AMSamplePlate>());
		AMSamplePlate *tempPlate;
		for(int x = 0; x < allSamplePlateIds.count(); x++){
			tempPlate = new AMSamplePlate();
			tempPlate->loadFromDb(database_, allSamplePlateIds.at(x));
			allSamplePlates_.prepend(tempPlate);
		}
	}
}

void AMSamplePlateBrowser::addSamplePlate(AMSamplePlate *samplePlate){
	beginInsertRows(QModelIndex(), 0, 0);
	allSamplePlates_.prepend(samplePlate);
	endInsertRows();
}

void AMSamplePlateBrowser::setCurrentSamplePlate(AMSamplePlate *samplePlate){
	int lastSamplePlateIndex = currentSamplePlateIndex_;

	bool foundPlate = false;
	for(int x = 0; x < allSamplePlates_.count(); x++){
		if(allSamplePlates_.at(x) == samplePlate){
			currentSamplePlateIndex_ = x;
			foundPlate = true;
		}
	}

	if(!foundPlate)
		currentSamplePlateIndex_ = -1;

	if(lastSamplePlateIndex == -1 && currentSamplePlateIndex_ == -1){
		//do nothing
	}
	else{
		int topLeft = std::min(lastSamplePlateIndex, currentSamplePlateIndex_);
		int bottomRight = std::max(lastSamplePlateIndex, currentSamplePlateIndex_);
		if(topLeft == -1)
			topLeft = bottomRight;
		QModelIndex topLeftIndex = createIndex(topLeft, 0, allSamplePlates_.at(topLeft));
		QModelIndex bottomRightIndex = createIndex(bottomRight, 0, allSamplePlates_.at(bottomRight));
		emit dataChanged(topLeftIndex, bottomRightIndex);
	}
}
