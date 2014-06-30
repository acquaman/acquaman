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


#include "dataman/AMSample.h"

#include "util/AMPeriodicTable.h"
#include "util/AMErrorMonitor.h"

#include "beamline/camera/AMShapeData.h"
#include "dataman/database/AMConstDbObject.h"
#include "dataman/AMSamplePlate.h"

AMSample::AMSample(QObject* parent)
	: AMDbObject(parent)
{

	init("New Sample");
}

AMSample::AMSample(const QString &sampleName, QObject *parent)
	: AMDbObject(parent)
{

	init(sampleName);
}

AMSample::AMSample(int databaseId, AMDatabase *database, QObject *parent)
	: AMDbObject(parent)
{
	loadFromDb(database, databaseId);
}

AMSample::~AMSample(){
	if(sampleShapePositionData_){
		sampleShapePositionData_->deleteLater();
		sampleShapePositionData_ = 0;
	}
}

QDateTime AMSample::dateTime() const
{
	return dateTime_;
}

QString AMSample::dateTimeString() const
{
	/// returns the dateTime formatted with dateTimeFormat
	return dateTime().toString(dateTimeFormat());
}

QString AMSample::notes() const
{
	return notes_;
}

QByteArray AMSample::image() const
{
	return image_;
}

QList<const AMElement *> AMSample::elements() const
{
	return elements_;
}

QList<int> AMSample::elementList() const
{
	QList<int> elementList;
	foreach(const AMElement* element, elements_)
	{
		if(element)
		{
			elementList<<element->atomicNumber();
		}
	}
	return elementList;
}

QString AMSample::elementString() const
{
	/// output a string listing of the elements
	QStringList elementStringList;
	foreach(const AMElement* element, elements_)
	{
		elementStringList.append(element->symbol());
	}

	return elementStringList.join(", ");
}

AMConstDbObjectList AMSample::scanList() const
{
	return scanList_;
}

const AMScan* AMSample::scanAt(int index) const{
	if(index < 0 || index >= scanList_.count())
		return 0;

	const AMScan *retVal = qobject_cast<const AMScan*>(scanList_.at(index)->object());
	return retVal;
}

bool AMSample::hasScan(AMScan *scan) const
{
	AMConstDbObject *constScan;
	for(int x = 0, size = scanList_.count(); x < size; x++){
		constScan = scanList_.at(x);
		if(constScan->database() == scan->database() && constScan->tableName() == scan->dbTableName() && constScan->id() == scan->id())
			return true;
	}
	return false;
}

const QStringList AMSample::tags() const
{
	return tags_;
}

bool AMSample::hasTag(QString tag) const
{
	return tags_.contains(tag,Qt::CaseSensitive);
}

AMShapeData *AMSample::sampleShapePositionData() const
{
	return sampleShapePositionData_;
}

int AMSample::thumbnailCount() const
{
	if(image_.isEmpty()) return 0;
	return 1;
}

AMDbThumbnail AMSample::thumbnail(int index) const
{
	bool success = true;
	if(index != 0 || !thumbnailCount())
		success = false;

	QImage image;
	if(success && image.loadFromData(image_, "PNG"))
		return AMDbThumbnail(name(),dateTimeString(),image);
	else
		return AMDbThumbnail(name(), dateTimeString(), AMDbThumbnail::InvalidType, QByteArray());
}

AMConstDbObject* AMSample::constDbSamplePlate() const{
	return samplePlate_;
}

const AMSamplePlate* AMSample::samplePlate() const{
	const AMSamplePlate *samplePlate = qobject_cast<const AMSamplePlate*>(samplePlate_->object());
	return samplePlate;
}

void AMSample::setName(const QString &name)
{
	AMDbObject::setName(name);
	AMShapeData* samplePosition = sampleShapePositionData();
	if(samplePosition)
	{
		samplePosition->setName(name);
	}
	emit sampleDetailsChanged();
}

void AMSample::setDateTime(const QDateTime dateTime)
{

	dateTime_ = dateTime;
	setModified(true);
}

void AMSample::setCurrentDateTime()
{
	setDateTime(QDateTime::currentDateTime());
	setModified(true);
	emit dateTimeChanged(dateTime_);
	emit sampleDetailsChanged();
}

void AMSample::setNotes(const QString notes)
{
	if(notes_ != notes){
		notes_ = notes;
		setModified(true);
		emit notesChanged(notes_);
		emit sampleDetailsChanged();
	}
}

/// sets the image from a QImage
void AMSample::setImage(const QImage& sampleImage)
{
	QBuffer imageBuffer;
	imageBuffer.open(QIODevice::WriteOnly);
	sampleImage.save(&imageBuffer, "PNG");
	imageBuffer.close();
	image_ = imageBuffer.buffer();
	setModified(true);
	emit sampleDetailsChanged();
}

void AMSample::setRawImage(const QByteArray &rawPngImage)
{
	image_ = rawPngImage;
	setModified(true);
	emit sampleDetailsChanged();
}

void AMSample::setElements(const QList<const AMElement *> elements)
{
	elements_ = elements;
	setModified(true);
	emit sampleDetailsChanged();
}

void AMSample::setScanList(AMConstDbObjectList scanList)
{
	scanList_ = scanList;
	setModified(true);
	emit sampleDetailsChanged();
}

void AMSample::addScan(AMScan *scan)
{
	/// add scan to the list
	if(!hasScan(scan))
	{
		//		scanList_.append(scan);
		scanList_.append(new AMConstDbObject(scan));
		setModified(true);
	}
}

void AMSample::removeScan(AMScan *scan)
{
	//	/// remove all instances of scan
	//	while(hasScan(scan))
	//	{
	//		scanList_.removeAt(scanList_.indexOf(scan));
	//		setModified(true);
	//	}
	if(!hasScan(scan))
		return;

	AMConstDbObject *constScan;
	for(int x = 0, size = scanList_.count(); x < size; x++){
		constScan = scanList_.at(x);
		if(constScan->database() == scan->database() && constScan->tableName() == scan->dbTableName() && constScan->id() == scan->id()){
			scanList_.removeAt(x);
			setModified(true);
			return;
		}
	}
}

void AMSample::setTags(const QStringList tags)
{
	if(tags_ != tags){
		tags_ = tags;
		setModified(true);
		emit tagsChanged(tags_);
		emit sampleDetailsChanged();
	}
}

void AMSample::addTag(const QString tag)
{
	/// add a tag, if not already in the list
	bool modified = false;
	if(!hasTag(tag))
	{
		tags_.append(tag);
		modified = true;
	}

	/// remove all empty tags
	int count = tags_.removeAll("");
	if(count > 0) modified = true;
	setModified(modified);
	if(modified)
	{
		emit tagsChanged(tags_);
		emit sampleDetailsChanged();
	}
}

void AMSample::removeTag(const QString tag)
{
	/// remove all instances of tag
	int count = tags_.removeAll(tag);
	if(count > 0)
	{
		setModified(true);
		emit tagsChanged(tags_);
		emit sampleDetailsChanged();
	}

}

void AMSample::setCurrentTag(QString tag)
{

	currentTag_ = tag;
	emit currentTagChanged(currentTag_);
}

void AMSample::editCurrentTag(QString tag)
{
	QString oldTag = currentTag_;
	int index = tags_.indexOf(oldTag);
	if(index == -1)
		AMErrorMon::alert(this, AMSAMPLE_EDITCURRENTTAG_TAG_NOT_FOUND, QString("AMSample cannot find a tag named %1 to edit.").arg(tag) );
	else if(index >= 0 && index < tags_.size())
	{
		tags_.removeAt(index);
		tags_.insert(index, tag);
		emit tagsChanged(tags_);
		emit sampleDetailsChanged();
	}
	else
		AMErrorMon::alert(this, AMSAMPLE_EDITCURRENTTAG_TAG_INDEX_OUT_OF_RANGE, QString("AMSample found a tag named %1 but the index (%2) was somehow out of range.").arg(tag).arg(index) );
}

void AMSample::setElementList(const AMIntList& elements)
{
	elements_.clear();
	foreach(int element, elements)
	{
		addElement(AMPeriodicTable::table()->elementByAtomicNumber(element));
	}
	emit sampleDetailsChanged();
	emit elementsChanged(elementString());
}

void AMSample::addElement(const AMElement *element)
{
	/// add an element into the list.
	/// place it in order according to atomic number
	if(!elements_.contains(element))
	{
		int position = 0;
		while(elements_.count() > position && elements_.at(position)->atomicNumber() < element->atomicNumber())
			position++;
		elements_.insert(position,element);
		setModified(true);
	}
	emit sampleDetailsChanged();
	emit elementsChanged(elementString());

}

void AMSample::removeElement(const AMElement *element)
{
	/// removes all instances of element from the list
	int count = elements_.removeAll(element);
	if(count > 0) setModified(true);
	emit sampleDetailsChanged();
	emit elementsChanged(elementString());
}

void AMSample::toggleElement(const AMElement *element)
{
	/// toggle element into or out of the list
	if(elements_.contains(element))
		removeElement(element);
	else
		addElement(element);
}

void AMSample::setSampleShapePositionData(AMShapeData *sampleShapePositionData)
{
//	qdebug()<<"AMSample:setSampleShapePositionData";
	if(sampleShapePositionData_ != sampleShapePositionData)
	{
		if(sampleShapePositionData_)
		{
			disconnect(sampleShapePositionData_, SIGNAL(nameChanged(QString)), this, SLOT(setName(QString)));
			disconnect(this, SIGNAL(currentTagChanged(QString)), sampleShapePositionData_, SLOT(setOtherDataFieldOne(QString)));
			disconnect(this, SIGNAL(elementsChanged(QString)), sampleShapePositionData_, SLOT(setOtherDataFieldTwo(QString)));
			disconnect(sampleShapePositionData_, SIGNAL(shapeDataChanged(AMShapeData*)), this, SLOT(onShapeDataChanged()));
		}
		sampleShapePositionData_ = sampleShapePositionData;
		if(sampleShapePositionData_)
		{
			sampleShapePositionData_->setName(name());
			if(!tags_.isEmpty())
				setCurrentTag(tags_.first());
			connect(sampleShapePositionData_, SIGNAL(nameChanged(QString)), this, SLOT(setName(QString)));
			// set other Data field one to tags
			connect(this, SIGNAL(currentTagChanged(QString)), sampleShapePositionData_, SLOT(setOtherDataFieldOne(QString)));
			connect(this, SIGNAL(elementsChanged(QString)), sampleShapePositionData_, SLOT(setOtherDataFieldTwo(QString)));
			connect(sampleShapePositionData_, SIGNAL(shapeDataChanged(AMShapeData*)), this, SLOT(onShapeDataChanged()));
		}

		emit sampleShapeDataChanged();
		emit requestCurrentTag();
		emit currentTagChanged(currentTag_);
		emit elementsChanged(elementString());
	}
//	qdebug()<<"AMSample:setSampleShapePositionData - done";
}

/// deletes the sample shape position data.
void AMSample::removeSampleShapePositionData()
{
//	qdebug()<<"AMSample::removeSampleShapePositionData";
	if(sampleShapePositionData_)
	{

		sampleShapePositionData()->removeShape();
		setSampleShapePositionData(0);
	}
	else
		AMErrorMon::alert(this, AMSAMPLE_REQUEST_TO_REMOVE_NULL_POSITIION_DATA, QString("AMSample was asked to remove the sample shape position data, but it is currently null.") );
}

void AMSample::removeSample()
{
	removeSampleShapePositionData();
	emit sampleAboutToBeRemoved();
}

void AMSample::setSamplePlate(const AMSamplePlate *samplePlate){
	if(!samplePlate_)
		samplePlate_ = new AMConstDbObject(samplePlate);
	else
		samplePlate_->setObject(samplePlate);
	setModified(true);
}

void AMSample::onShapeDataChanged()
{
	setModified(true);
}

void AMSample::forceStoreToDb()
{
//	storeToDb(database());
	emit requestStoreToDb();
}


/// the format used to present dateTime as a string
QString AMSample::dateTimeFormat() const
{
	return QString("MMM d (yyyy)");
}

void AMSample::init(QString name)
{
	makeConnections();
	sampleShapePositionData_ = 0;
	currentTag_ = "";
	samplePlate_ = 0; //NULL
	setName(name);
	setCurrentDateTime();
}

void AMSample::makeConnections()
{
	//connect(this, SIGNAL(tagsChanged(QStringList)), this, SLOT(getCurrentTag()));
	connect(this, SIGNAL(tagsChanged(QStringList)), this, SIGNAL(requestCurrentTag()));
	connect(this, SIGNAL(nameChanged(QString)), this, SIGNAL(sampleDetailsChanged()));
}

QByteArray AMSample::rawImage() const
{
	return image_;
}

AMConstDbObjectList AMSample::dbReadScanList() const
{
	return scanList_;
}

void AMSample::dbLoadScanList(const AMConstDbObjectList &newScanList)
{
	scanList_.clear();
	foreach(AMConstDbObject *constScan, newScanList)
		scanList_.append(constScan);
}

AMQVector3DVector AMSample::dbReadShapeData() const{
	if(sampleShapePositionData_)
		return sampleShapePositionData_->coordinates();
	return QVector<QVector3D>();
}

void AMSample::dbLoadShapeData(AMQVector3DVector newShapeData){
	AMShapeData* shapeData = new AMShapeData(this);
	setSampleShapePositionData(shapeData);
	sampleShapePositionData_->setCoordinateShape(newShapeData);
	emit sampleShapeDataChanged();
}

AMConstDbObject* AMSample::dbReadSamplePlate() const{
	return samplePlate_;
}

void AMSample::dbLoadSamplePlate(AMConstDbObject *newSamplePlate){
	samplePlate_ = newSamplePlate;
}
