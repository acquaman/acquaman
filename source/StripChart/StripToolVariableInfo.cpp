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


#include "StripToolVariableInfo.h"
#include <QDebug>

StripToolVariableInfo::StripToolVariableInfo(QObject *parent) :
    QObject(parent)
{
    dataSource_ = 0;
    defaultSettings();

    qDebug() << "StripToolVariableInfo object created.";
}



bool StripToolVariableInfo::isValid() const
{
    if (name() != "" && dataSource_ != 0 && dataSource_->isValid()) {
        qDebug() << "StripToolVariableInfo is valid.";
        return true;

    } else {
        qDebug() << "StripToolVariableInfo is invalid.";
        return false;
    }
}



bool StripToolVariableInfo::hasName() const
{
    return hasName_;
}



bool StripToolVariableInfo::hasDataSource() const
{
    return hasDataSource_;
}



bool StripToolVariableInfo::hasCreationDateTime() const
{
    return hasCreationDateTime_;
}



bool StripToolVariableInfo::hasDescription() const
{
    return hasDescription_;
}



bool StripToolVariableInfo::hasUnits() const
{
    return hasUnits_;
}



bool StripToolVariableInfo::hasGranularity() const
{
    return hasGranularity_;
}



bool StripToolVariableInfo::hasAverageOverPoints() const
{
    return hasAverageOverPoints_;
}



bool StripToolVariableInfo::hasColor() const
{
    return hasColor_;
}



bool StripToolVariableInfo::hasSelectionState() const
{
    return hasSelectionState_;
}



bool StripToolVariableInfo::hasCheckState() const
{
    return hasCheckState_;
}



bool StripToolVariableInfo::hasCustomAxisMax() const
{
    return hasCustomAxisMax_;
}



bool StripToolVariableInfo::hasCustomAxisMin() const
{
    return hasCustomAxisMin_;
}



QString StripToolVariableInfo::name() const
{
    if (hasName())
        return name_;
    else
        return "";
}



AMDataSource* StripToolVariableInfo::dataSource() const
{
    if (hasDataSource())
        return dataSource_;
    else
        return 0;
}



QString StripToolVariableInfo::description() const
{
    if (hasDescription())
        return description_;
    else
        return "";
}



QString StripToolVariableInfo::creationDateTime() const
{
    if (hasCreationDateTime())
        return creationDateTime_;

    else
        return "";
}



QString StripToolVariableInfo::units() const
{
    if (hasUnits())
        return units_;
    else
        return "";
}



int StripToolVariableInfo::granularity() const
{
    if (hasGranularity())
        return granularity_;
    else
        return -1;
}



int StripToolVariableInfo::averageOverPoints() const
{
    if (hasAverageOverPoints())
        return averageOverPoints_;
    else
        return -1;
}



QColor StripToolVariableInfo::color() const
{
    if (hasColor())
        return color_;
    else
        return QColor();
}



QString StripToolVariableInfo::colorName() const
{
    if (hasColor())
        return color_.name();
    else
        return "";
}



bool StripToolVariableInfo::selectionState() const
{
    if (hasSelectionState())
        return selectionState_;
    else
        return false;
}



Qt::CheckState StripToolVariableInfo::checkState() const
{
    if (hasCheckState())
        return checkState_;
    else
        return Qt::Unchecked;
}



double StripToolVariableInfo::customAxisMax() const
{
    if (hasCustomAxisMax())
        return customAxisMax_;
    else
        return -1;
}



double StripToolVariableInfo::customAxisMin() const
{
    if (hasCustomAxisMin())
        return customAxisMin_;
    else
        return -1;
}



int StripToolVariableInfo::timeAmount() const
{
    return timeAmount_;
}



TimeEntryWidget::TimeUnits StripToolVariableInfo::timeUnits() const
{
    return timeUnits_;
}



int StripToolVariableInfo::displayedTimeMS() const
{
    // want to return the number of milliseconds of data to display at a given time.
    // this is a function of the current time amount and units.

    int displayedTime;

    switch (timeUnits_) {
    case (TimeEntryWidget::Seconds) :
        displayedTime = timeAmount_ * 1000.0;
        break;

    case (TimeEntryWidget::Minutes) :
        displayedTime = timeAmount_ * 1000.0 * 60 * 60;
        break;

    case (TimeEntryWidget::Hours) :
        displayedTime = timeAmount_ * 1000.0 * 60 * 60;
        break;

    default:
        displayedTime = 0;
        qDebug() << "StripToolVariableInfo::displayedTimeMS() : unknown units encountered!! Setting displayed time to zero.";
        break;
    }

    return displayedTime;
}



double StripToolVariableInfo::timeMSToTimeUnits(int measurementTimeMS) const
{
    // want to take a value in milliseconds and return a 'proper' time--original value converted into timeUnits_.
    double properTime;

    switch (timeUnits_) {
    case (TimeEntryWidget::Seconds) :
        properTime = measurementTimeMS / 1000.0;
        break;

    case (TimeEntryWidget::Minutes) :
        properTime = measurementTimeMS / 1000.0 / 60;
        break;

    case (TimeEntryWidget::Hours) :
        properTime = measurementTimeMS / 1000.0 / 60 / 60;
        break;

    default:
        properTime = 0;
        qDebug() << "StripToolVariableInfo::timeMSToTimeUnits(...) : unknown units encountered!! Setting proper time to zero.";
        break;
    }

    return properTime;
}



bool StripToolVariableInfo::importAutomatically() const
{
    return importAutomatically_;
}



void StripToolVariableInfo::setName(const QString &sourceName)
{
    if (sourceName == "") {
        hasName_ = false;
        name_ = sourceName;

        setDataSource(0);
        setDescription("");

    } else {
        hasName_ = true;
        name_ = sourceName;
        setDescription(name_);

        if (dataSource_ == 0) {
            setDataSource( new AM0DProcessVariableDataSource(new AMProcessVariable(name_, true, this), name_, this) );
        }

    }

    emit nameChanged(name_);
    emit infoChanged();

    qDebug() << "StripToolVariableInfo :: set name for variable " << name();
}



void StripToolVariableInfo::setDataSource(AMDataSource *newSource)
{
    if (newSource == 0) {
        hasDataSource_ = false;
        qDebug() << "StripToolVariableInfo :: removed data source for variable " << name();

    } else {
        hasDataSource_ = true;

        if (name() == "")
            setName(newSource->name());
    }

    if (dataSource_ != 0) {
        disconnect( dataSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onDataSourceValueUpdate(AMnDIndex,AMnDIndex)) );

//        dataSource_->deleteLater();
        dataSource_ = 0;
    }

    dataSource_ = newSource;

    if (dataSource_ != 0) {
        connect( dataSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onDataSourceValueUpdate(AMnDIndex,AMnDIndex)) );
    }

    emit dataSourceChanged();
    qDebug() << "StripToolVariableInfo :: set new data source for variable " << name();
}



void StripToolVariableInfo::setDescription(const QString &newDescription)
{
    if (newDescription == "" && !isValid()) {
        hasDescription_ = false;
        description_ = newDescription;

    } else if (newDescription == "" && isValid()) {
        hasDescription_ = true;
        description_ = name();

    } else if (newDescription != "") {
        hasDescription_ = true;
        description_ = newDescription;
    }

    emit descriptionChanged(description_);
    emit infoChanged();
}



void StripToolVariableInfo::setCreationDateTime(const QString &creationTime)
{
    hasCreationDateTime_ = true;
    creationDateTime_ = creationTime;

    emit infoChanged();
}



void StripToolVariableInfo::setUnits(const QString &newUnits)
{
    hasUnits_ = true;
    units_ = newUnits;

    emit unitsChanged(units_);
    emit infoChanged();
}



void StripToolVariableInfo::setGranularity(const int newGranularity)
{
    if (newGranularity > 0) {
        hasGranularity_ = true;
        granularity_ = newGranularity;

        emit granularityChanged(granularity_);
        emit infoChanged();
    }
}



void StripToolVariableInfo::setAverageOverPoints(const int avgPoints)
{
    if (avgPoints > 0) {
        hasAverageOverPoints_ = true;

        if (avgPoints != averageOverPoints()) {
            averageOverPoints_ = avgPoints;
            emit infoChanged();
        }
    }
}



void StripToolVariableInfo::setColor(const QColor &newColor)
{
    hasColor_ = true;

    if (!newColor.isValid()) {
        if (color_.isValid()) {
            qDebug() << "StripToolVariableInfo::setColor(...) : cannot change color to an invalid QColor. No changes made.";
            return;

        } else {
            qDebug() << "StripToolVariableInfo::setColor(...) : cannot change color to an invalid QColor. Restoring default color.";
            color_ = QColor(Qt::red);
        }

    } else if (color_ == newColor) {
        return;

    } else {
        qDebug() << "StripToolVariableInfo :: changing color from " << color_.name() << " to " << newColor.name();
        color_ = newColor;
    }

    emit colorChanged(color_.name());
    emit infoChanged();
}



void StripToolVariableInfo::setSelectionState(const bool isSelected)
{
    hasSelectionState_ = true;
    selectionState_ = isSelected;

    emit selectionStateChanged(selectionState_);
    emit infoChanged();
}



void StripToolVariableInfo::setCheckState(Qt::CheckState checked)
{
    hasCheckState_ = true;

    if (checkState_ != checked) {
        checkState_ = checked;

        if (checked == Qt::Checked)
            qDebug() << "StripToolVariableInfo::setCheckState(...) : variable " << name() << "checked.";
        else
            qDebug() << "StripToolVariableInfo::setCheckState(...) : variable " << name() << "unchecked.";

        emit checkStateChanged(checkState_);
        emit infoChanged();
    }
}



void StripToolVariableInfo::setCustomAxisMax(const double newMax)
{
    qDebug() << "StripToolVariableInfo :: setting custom axis max : " << newMax;
    hasCustomAxisMax_ = true;
    customAxisMax_ = newMax;
    emit axisMaxChanged(newMax);
    emit infoChanged();
}



void StripToolVariableInfo::setCustomAxisMin(const double newMin)
{
    qDebug() << "StripToolVariableInfo :: setting custom axis min : " << newMin;
    hasCustomAxisMin_ = true;
    customAxisMin_ = newMin;
    emit axisMinChanged(newMin);
    emit infoChanged();
}



void StripToolVariableInfo::eraseCustomAxisMax()
{
    qDebug() << "StripToolVariableInfo :: erasing custom axis max limits.";
    hasCustomAxisMax_ = false;
    customAxisMax_ = 0;
    emit axisMaxErased();
    emit infoChanged();
}



void StripToolVariableInfo::eraseCustomAxisMin()
{
    qDebug() << "StripToolVariableInfo :: erasing custom axis min limits.";
    hasCustomAxisMin_ = false;
    customAxisMin_ = 0;
    emit axisMinErased();
    emit infoChanged();
}



void StripToolVariableInfo::setTimeAmount(int newTime)
{
    timeAmount_ = newTime;
}



void StripToolVariableInfo::setTimeUnits(TimeEntryWidget::TimeUnits units)
{
    timeUnits_ = units;
}



void StripToolVariableInfo::setToImportAutomatically(const bool import)
{
    importAutomatically_ = import;
    emit infoChanged();
}



void StripToolVariableInfo::onDataSourceValueUpdate(const AMnDIndex &start, const AMnDIndex &end)
{
    Q_UNUSED(start)
    Q_UNUSED(end)

    double newValue = (double) dataSource()->value(AMnDIndex());
    QTime newTime = QTime::currentTime();

    emit dataSourceValueUpdate(newTime, newValue);
}



void StripToolVariableInfo::defaultSettings()
{
    hasName_ = false;
    hasDataSource_ = false;
    hasCreationDateTime_ = false;
    hasDescription_ = false;
    hasUnits_ = false;
    hasGranularity_ = false;
    hasAverageOverPoints_ = false;
    hasColor_ = false;
    hasSelectionState_ = false;
    hasCheckState_ = false;
    hasCustomAxisMax_ = false;
    hasCustomAxisMin_ = false;

    customAxisMax_ = 0;
    customAxisMin_ = 0;

    setUnits("");
    setGranularity(1);
    setAverageOverPoints(1);
    setColor(QColor(Qt::red));
    setSelectionState(false);
    setCheckState(Qt::Unchecked);

    setTimeAmount(10);
    setTimeAmount(TimeEntryWidget::Seconds);

    setToImportAutomatically(true);
}
