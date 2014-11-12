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


#include "STVariable.h"

STVariable::STVariable(const QString &name, QObject *parent) :
    QObject(parent)
{
    index_ = -1;
    pointsSaved_ = 50;

    description_ = "";

    units_ = "";

    created_ = QDateTime::currentDateTime();

    pv_ = new AMProcessVariable(name, true, this);

    data_ = new AM0DAccumulatorAB("PVData", this);
    data_->setDataStoredCountMax(pointsSaved_);

    times_ = new ST0DTimestampAB("Timestamps", this);
    times_->setDataStoredCountMax(pointsSaved_);

    timedData_ = new AM1DTimedDataAB("TimedData", this);

    data_->setInputDataSources(QList<AMDataSource*>() << new AM0DProcessVariableDataSource(pv_, pv_->pvName(), this));
    times_->setInputDataSources(QList<AMDataSource*>() << new AM0DProcessVariableDataSource(pv_, pv_->pvName(), this));
    timedData_->setInputDataSources(QList<AMDataSource*>() << data_ << times_);

    seriesData_ = new AMDataSourceSeriesData(timedData_);
    series_ = new MPlotSeriesBasic();
    series_->setModel(seriesData_, true);
    series_->setDescription(" ");

    series_->setMarker(MPlotMarkerShape::None);

    connect( pv_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)) );
    connect( pv_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)) );
    if (pv_->isConnected())
        emit connected(true);

}

STVariable::~STVariable()
{

}

int STVariable::index() const
{
    return index_;
}

int STVariable::pointsSaved() const
{
    return pointsSaved_;
}

QString STVariable::name() const
{
    return pv_->pvName();
}

QString STVariable::description() const
{
    return description_;
}

bool STVariable::hasDescription() const
{
    return (description_ != "");
}

bool STVariable::showDescription() const
{
    return (series_->description() != " ");
}

QString STVariable::units() const
{
    return units_;
}

bool STVariable::hasUnits() const
{
    return (units_ != "");
}

QColor STVariable::color() const
{
    return series_->linePen().color();
}

int STVariable::lineThickness() const
{
    return series_->linePen().width();
}

QDateTime STVariable::created() const
{
    return created_;
}

bool STVariable::isConnected() const
{
    return pv_->isConnected();
}

AMProcessVariable* STVariable::pv() const
{
    return pv_;
}

double STVariable::value() const
{
    return pv_->getDouble();
}

MPlotSeriesBasic* STVariable::series() const
{
    return series_;
}

bool STVariable::markersEnabled() const
{
    return (series_->marker() != 0);
}

bool STVariable::operator==(const STVariable &other) const
{
    bool namesMatch = false;
    bool creationDateTimesMatch = false;

    if (name() == other.name())
        namesMatch = true;

    if (created() == other.created())
        creationDateTimesMatch = true;

    return (namesMatch && creationDateTimesMatch);
}

void STVariable::setIndex(int newIndex)
{
    if (index_ != newIndex && newIndex >= 0) {
        index_ = newIndex;
        emit indexChanged(index_);
    }
}

void STVariable::setPointsSaved(int newPoints)
{
    if (newPoints > 0) {
        pointsSaved_ = newPoints;

        data_->setDataStoredCountMax(pointsSaved_);
        times_->setDataStoredCountMax(pointsSaved_);

        emit pointsSavedChanged(pointsSaved_);
    }
}

void STVariable::setDescription(const QString &newDescription)
{
    if (description_ != newDescription) {
        description_ = newDescription;

        emit descriptionChanged(description_);
    }
}

void STVariable::setShowDescription(bool showDescription)
{
    bool showState = (series_->description() != " ");

    if (showState != showDescription) {
        if (showDescription)
            series_->setDescription(description_);

        else
            series_->setDescription(" ");

        emit showDescriptionChanged(showDescription);
    }
}

void STVariable::setUnits(const QString &units)
{
    if (units_ != units) {
        units_ = units;

        emit unitsChanged(units_);
    }
}

void STVariable::setColor(const QColor &newColor)
{
    QColor currentColor = series_->linePen().color();

    if (currentColor != newColor) {
        series_->setLinePen(QPen(newColor));
        emit colorChanged(newColor);
    }
}

void STVariable::setLineThickness(int newThickness)
{
    if (newThickness < 0)
        return;

    int currentThickness = series_->linePen().width();

    if (currentThickness != newThickness) {
        series_->setLinePen(QPen(QBrush(color()), newThickness));
        emit lineThicknessChanged(newThickness);
    }
}

void STVariable::setDataBufferSize(int bufferSize)
{
    data_->setDataStoredCountMax(bufferSize);
    times_->setDataStoredCountMax(bufferSize);
}

void STVariable::setTimeValue(int newValue)
{
    times_->setTimeValue(newValue);
}

void STVariable::setTimeUnits(STTime::Units newUnits)
{
    times_->setTimeUnits(newUnits);
}

void STVariable::setTimeFilteringEnabled(bool isEnabled)
{
    times_->enableTimeFiltering(isEnabled);
}

void STVariable::enableMarkers(bool newState)
{
    bool enabledState = (series_->marker() != 0);

    if (enabledState != newState) {
        QColor lineColor = series_->linePen().color();

        if (newState)
            series_->setMarker(MPlotMarkerShape::Circle, 6, QPen(lineColor));
        else
            series_->setMarker(MPlotMarkerShape::None);

        emit markersEnabled(true);
    }
}

