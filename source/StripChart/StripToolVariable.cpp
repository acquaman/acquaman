#include "StripToolVariable.h"
#include <QDebug>

StripToolVariable::StripToolVariable(AMDataSource *dataSource, QObject *parent) :
    QObject(parent)
{
    name_ = "";
    description_ = "";
    units_ = "";
    creationDateTime_ = QDateTime::currentDateTime().toString();
    isValid_ = false;

    setName(dataSource->name());
    setDataSource(dataSource);

    qDebug() << "Instance of StripToolVariable created.";
}



bool StripToolVariable::operator== (const StripToolVariable &anotherVariable) {
    return (this->name() == anotherVariable.name());
}



QString StripToolVariable::name() const {
    return name_;
}



void StripToolVariable::setName(const QString &newName) {
    name_ = newName;
    emit nameChanged(name_);
    emit infoChanged();

    if (description() == "")
        setDescription(name());
}



QString StripToolVariable::description() const {
    return description_;
}



void StripToolVariable::setDescription(const QString &newDescription) {
    if (newDescription == "")
        description_ = name();
    else
        description_ = newDescription;

    emit descriptionChanged(description_);
    emit infoChanged();
}



QString StripToolVariable::units() const {
    return units_;
}



void StripToolVariable::setUnits(const QString &newUnits) {
    units_ = newUnits;
    emit unitsChanged(units_);
    emit infoChanged();
}



bool StripToolVariable::isValid() const {
    return isValid_;
}



QList<QString> StripToolVariable::getInfo() {
    QList<QString> properties;

    properties << name();
    properties << description();
    properties << units();

    return properties;
}



void StripToolVariable::setInfo(QList<QString> newProperties) {
    int propNum = newProperties.size();

    qDebug() << "StripToolVariable :: Attempting to set properties for variable named" << name();

    if (propNum <= 0) {
        qDebug() << "StripToolVariable :: Cannot set properties for" << name() << "because properties list provided was empty.";
        return;

    } else if (newProperties.at(0) != name()) {
        qDebug() << "StripToolVariable :: Cannot set properties for" << name() << "because properties list provided is for variable named" << newProperties.at(0);
        return;
    }

    if (propNum > 0)
        setDescription(newProperties.at(1));

    if (propNum > 1)
        setUnits(newProperties.at(2));
}



QString StripToolVariable::creationDateTime() const {
    return creationDateTime_;
}



AMDataSource* StripToolVariable::dataSource() const {
    return dataSource_;
}



void StripToolVariable::setDataSource(AMDataSource *newDataSource) {
    if (dataSource_ == newDataSource)
        return;

    if (dataSource_ != 0)
        dataSource_->deregisterObserver(this);

    dataSource_ = newDataSource;
    dataSource_->registerObserver(this);

    if (dataSource_->isValid())
        setAsValid(true);

    setName(dataSource_->name());

    connect( dataSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onDataSourceStateChanged(int)) );
    connect( dataSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onDataSourceChanged(AMnDIndex, AMnDIndex)) );
    connect( dataSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onDataSourceValueChanged(AMnDIndex, AMnDIndex)) );

    emit dataSourceChanged();
}



void StripToolVariable::onDataSourceStateChanged(int newState) {
    if (newState == 0) {
        setAsValid(true);

    } else {

        setAsValid(false);
    }
}



void StripToolVariable::onDataSourceValueChanged(AMnDIndex start, AMnDIndex end) {
    Q_UNUSED(start)
    Q_UNUSED(end)

    qDebug() << "StripToolVariable :: detecting a data source value update.";
}



void StripToolVariable::setAsValid(bool isValid) {
    isValid_ = isValid;
    qDebug() << "StripToolVariable :: variable" << name() << "is now valid : " << isValid_;
}





