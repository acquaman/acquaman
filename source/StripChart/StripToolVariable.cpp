#include "StripToolVariable.h"

StripToolVariable::StripToolVariable(QObject *parent) :
    QObject(parent)
{
    name_ = "";
    description_ = "";
    units_ = "";
    checkState_ = Qt::Checked;
    dataSource_ = 0;
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
}



QString StripToolVariable::units() const {
    return units_;
}



void StripToolVariable::setUnits(const QString &newUnits) {
    units_ = newUnits;
    emit unitsChanged(units_);
}



Qt::CheckState StripToolVariable::checkState() const {
    return checkState_;
}



void StripToolVariable::setCheckState(Qt::CheckState newState) {
    checkState_ = newState;
    emit checkStateChanged(checkState_);
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

    if (dataSource_ != 0)
        dataSource_->registerObserver(this);

//    connect( dataSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onDataSourceValueChanged(AMnDIndex, AMnDIndex)) );

    emit dataSourceChanged();
}





