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

        setDataSource( new AM0DProcessVariableDataSource(new AMProcessVariable(name_, true, this), name_, this) );
        setDescription(name_);
    }

    emit nameChanged(name_);
    emit infoChanged();

    qDebug() << "StripToolVariableInfo :: set name for variable " << name();
}



void StripToolVariableInfo::setDataSource(AM0DProcessVariableDataSource *newSource)
{
    if (newSource == 0) {
        hasDataSource_ = false;

    } else {
        hasDataSource_ = true;
    }

    if (dataSource_ != 0) {
        disconnect( dataSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onDataSourceValueUpdate(AMnDIndex,AMnDIndex)) );

        dataSource_->deleteLater();
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



void StripToolVariableInfo::setColor(const QColor &newColor)
{
    hasColor_ = true;
    color_ = newColor;

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
    hasColor_ = false;
    hasSelectionState_ = false;
    hasCheckState_ = false;

    setUnits("");
    setGranularity(1);
    setColor(QColor(Qt::red));
    setSelectionState(false);
    setCheckState(Qt::Unchecked);
    setToImportAutomatically(true);
}
