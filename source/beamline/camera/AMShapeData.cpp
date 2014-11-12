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


#include "AMShapeData.h"

#include <QString>
#include <QVector3D>
#include <QPolygonF>

#include "util/AMErrorMonitor.h"

/// constructor
AMShapeData::AMShapeData(QObject* parent)
	:QObject(parent)
{
	shape_ = new QPolygonF();
	setOtherDataFieldOne(" ");
	setOtherDataFieldTwo(" ");
	coordinateCount_ = -1;
	visible_ = true;
	zAxisRotation_ = 0;
	xAxisRotation_ = 0;
	yAxisRotation_ = 0;
}

/// constructor
AMShapeData::AMShapeData(QPolygonF shape, QString name, QString otherDataFieldOne, QString otherDataFieldTwo, double idNumber, QObject *parent)
	:QObject(parent)
{
	shape_ = new QPolygonF();
	*shape_ = shape;
	setName(name);
	setOtherDataFieldOne(otherDataFieldOne);
	setOtherDataFieldTwo(otherDataFieldTwo);
	setIdNumber(idNumber);
	coordinateCount_ = -1;
	visible_ = true;
	zAxisRotation_ = 0;
	xAxisRotation_ = 0;
	yAxisRotation_ = 0;
}


AMShapeData::~AMShapeData()
{
	coordinate_.clear();

	delete shape_;
}

QPolygonF* AMShapeData::shape() const
{
	if(!(shape_))
		AMErrorMon::alert(this, AMSHAPEDATA_SHAPE_IS_NULL, QString("AMShapeData::shape() was called on an object with null shape.") );
	return shape_;
}

QString AMShapeData::name() const
{
	return name_;
}

QString AMShapeData::otherDataFieldOne() const
{
	return otherDataFieldOne_;
}

QString AMShapeData::otherDataFieldTwo() const
{
	return otherDataFieldTwo_;
}

double AMShapeData::idNumber() const
{
	return idNumber_;
}

QVector3D AMShapeData::coordinate(int index) const
{
	if(validIndex(index))
		return coordinate_[index];
	else
		return QVector3D(0,0,0);
}

QVector<QVector3D> AMShapeData::coordinates() const{
	return coordinate_;
}

double AMShapeData::rotation() const
{
	return zAxisRotation_;
}

double AMShapeData::tilt() const
{
	return xAxisRotation_;
}

double AMShapeData::yAxisRotation() const
{
	return yAxisRotation_;
}

bool AMShapeData::visible() const
{
	return visible_;
}


void AMShapeData::setShape(const QPolygonF shape)
{
	if(*shape_ != shape)
	{
		*shape_ = shape;
		emit shapeDataChanged(this);
	}
}

void AMShapeData::setName(QString name)
{

	if(name_ != name)
	{
		name_ = name;
		emit nameChanged(name_);
		emit shapeDataChanged(this);
	}
}

void AMShapeData::setOtherDataFieldOne(const QString &otherData)
{
	if(otherDataFieldOne_ != otherData)
	{
		otherDataFieldOne_ = otherData;
		emit otherDataFieldOneChanged(otherData);
		emit shapeDataChanged(this);
	}
}

void AMShapeData::setOtherDataFieldTwo(QString otherDataFieldTwo)
{
	if(otherDataFieldTwo_ != otherDataFieldTwo)
	{
		otherDataFieldTwo_ = otherDataFieldTwo;
		emit otherDataFieldTwoChanged(otherDataFieldTwo_);
		emit shapeDataChanged(this);
	}
}

void AMShapeData::setIdNumber(double idNumber)
{
	if(idNumber_ != idNumber)
	{
		idNumber_ = idNumber;
		emit shapeDataChanged(this);
	}
}

void AMShapeData::setCoordinate(QVector3D coordinate, int index)
{
	if(validIndex(index) && coordinate_[index] != coordinate)
	{
		coordinate_[index] = coordinate;
		emit shapeDataChanged(this);
	}
}

void AMShapeData::setCoordinateShape(QVector<QVector3D> coordinates)
{
	int count = coordinates.count();
	if(coordinates.isEmpty()) return;
	if(coordinates == coordinate_) return;
	coordinate_.clear();
	for(int i = 0; i < count; i++)
	{
		if(coordinateCount_ < i)
		{
			coordinateCount_ = i;
		}
		coordinate_<<coordinates[i];
		if(coordinateCount_ < coordinate_.count())
			coordinateCount_ = coordinate_.count();
	}
	emit shapeDataChanged(this);
}


void AMShapeData::setRotation(double zAxisRotation)
{
	if(zAxisRotation_ != zAxisRotation)
	{
		zAxisRotation_ = zAxisRotation;
		emit shapeDataChanged(this);
	}
}

void AMShapeData::setTilt(double xAxisRotation)
{
	if(xAxisRotation_ != xAxisRotation)
	{
		xAxisRotation_ = xAxisRotation;
		emit shapeDataChanged(this);
	}
}

void AMShapeData::setYAxisRotation(double yAxisRotation)
{
	if(yAxisRotation_ != yAxisRotation)
	{
		yAxisRotation_ = yAxisRotation;
		emit shapeDataChanged(this);
	}
}

void AMShapeData::setVisible(bool visible)
{
	if(visible_ != visible)
	{
		visible_ = visible;
		emit shapeDataChanged(this);
	}
}

#include <QDebug>
void AMShapeData::copy(const AMShapeData *other)
{
	if(!(other))
		AMErrorMon::alert(this, AMSHAPEDATA_COPY_NULL_COPY_INPUT, QString("AMShapeData attempted to copy a null AMShapeData as input.") );
	blockSignals(true);
	setName(other->name());
	setOtherDataFieldOne(other->otherDataFieldOne());
	setIdNumber(other->idNumber());
	setRotation(other->rotation());
	setTilt(other->tilt());
	setYAxisRotation(other->yAxisRotation());
	setVisible(other->visible());
	setShape(*other->shape());

	QVector<QVector3D> nullShape;
	if(other->count() > 0)
		nullShape = QVector<QVector3D>(other->count(), QVector3D(0,0,0));

	setCoordinateShape(nullShape);
	for(int i = 0; i < other->count(); i++)
	{
		setCoordinate(other->coordinate(i),i);

	}
	blockSignals(false);

	emit shapeDataChanged(this);
}

/// finds the center of the shape
/// Shape should have a repeated start/end point
QVector3D AMShapeData::centerCoordinate() const
{
	QVector3D center = QVector3D(0,0,0);
	for(int i = 0; i < (coordinateCount_ - 1); i++)// dont want the last point
	{
		center += coordinate(i);
	}
	return center/(double)(coordinateCount_ - 1);
}

/// shifts the shape by the given amount
void AMShapeData::shift(const QVector3D &shift)
{
	if(shift != QVector3D(0,0,0))
	{
		for(int i = 0; i < count(); i++)
		{
			setCoordinate(coordinate_[i] +  shift,i);
		}
		emit shapeDataChanged(this);
	}
}

/// shifts the shape to the given location
void AMShapeData::shiftTo(const QVector3D &shiftTo)
{
	QVector3D shiftPoint = shiftTo - centerCoordinate();
	shift(shiftPoint);
}

/// returns a count of the number of coordinates
/// Should be one more than the number or vertices (start/end is repeated)
int AMShapeData::count() const
{
	return coordinateCount_;
}

/// checks to see if the shape is backwards
bool AMShapeData::backwards() const
{
	if(count() < 3) return false;

	QVector3D points [3];
	for(int i = 0; i < 3 ; i++)
	{
		points[i] = QVector3D(shape_->at(i));
	}
	QVector3D rayOne = points[1] - points[0];
	QVector3D rayTwo = points[2] - points[1];
	QVector3D normal = QVector3D::normal(rayOne,rayTwo);

	// normal should be either positive or negative 1
	return(normal.z() < 0);
}

bool AMShapeData::isEqual(const AMShapeData &other) const
{
	if(other.name_ != name_)
		return false;
	if(other.idNumber_ != idNumber_)
		return false;
	if(other.otherDataFieldOne_ != otherDataFieldOne_)
		return false;
	if(other.zAxisRotation_ != zAxisRotation_)
		return false;
	if(other.xAxisRotation_ != xAxisRotation_)
		return false;
	if(other.yAxisRotation_ != yAxisRotation_)
		return false;
	if(other.coordinateCount_ != coordinateCount_)
		return false;
	if(other.shape_ != shape_)
		return false;
	for(int i = 0; i < coordinateCount_; i++)
	{
		if(other.coordinate_.at(i) != coordinate_.at(i))
			return false;
	}
	return true;
}

void AMShapeData::removeShape()
{
	emit shapeDataRemoved(this);
}

/// checks for a valid coordinate index
bool AMShapeData::validIndex(int index) const
{
	return (index >= 0 && index <= coordinateCount_);
}



