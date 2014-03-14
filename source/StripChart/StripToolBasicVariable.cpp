//#include "StripToolBasicVariable.h"
//#include <QDebug>

//StripToolBasicVariable::StripToolBasicVariable(StripToolBasicVariableInfo *info, AMDataSource *dataSource, QObject *parent) :
//    StripToolAbstractVariable(info, dataSource, parent)
//{
//    QString defaultCreationDateTime = QDateTime::currentDateTime().toString();
//    Qt::CheckState defaultCheckState = Qt::Checked;
//    bool defaultSelectionState = false;

//    info_ = info;

//    setCreationDateTime(defaultCreationDateTime);
//    setCheckState(defaultCheckState);
//    setSelectionState(defaultSelectionState);

//    qDebug() << "StripToolBasicVariable object created for" << name();
//}



//StripToolBasicVariableInfo* StripToolBasicVariable::info() const {
//    return info_;
//}



//void StripToolBasicVariable::setCreationDateTime(const QString &newCreationInfo) {
//    info()->setCreationDateTime(newCreationInfo);
//}



//QString StripToolBasicVariable::creationDateTime() const {
//    if (isValid() && info()->hasCreationDateTime())
//        return info()->creationDateTime();
//    else
//        return "";
//}



//void StripToolBasicVariable::setDescription(const QString &newDescription) {
//    if (isValid()) {
//        info()->setDescription(newDescription);
//        emit descriptionChanged(description());
//        emit infoChanged();
//    }
//}



//QString StripToolBasicVariable::description() const {
//    if (isValid() && info()->hasDescription())
//        return info()->description();
//    else
//        return "";
//}



//void StripToolBasicVariable::setUnits(const QString &newUnits) {
//    if (isValid()) {
//        info()->setUnits(newUnits);
//        emit unitsChanged(units());
//        emit infoChanged();
//    }
//}



//QString StripToolBasicVariable::units() const {
//    if (isValid() && info()->hasUnits())
//        return info()->units();
//    else
//        return "";
//}



//void StripToolBasicVariable::setCheckState(Qt::CheckState &newState) {
//    if (isValid()) {
//        info()->setCheckState(newState);
//    }

//    if (info()->hasCheckState())
//        emit checkStateChanged(info()->checkState());
//}



//Qt::CheckState StripToolBasicVariable::checkState() const {
//    if (isValid() && info()->hasCheckState()) {
//        return info()->checkState();

//    } else {
//        qDebug() << "StripToolBasicVariable :: attempting to retrieve checkstate information for" << name() << "that is not valid!! Big problem!!";
//        return Qt::Unchecked;
//    }
//}



//void StripToolBasicVariable::setSelectionState(bool isSelected)
//{
//    if (isValid())
//        info()->setSelectionState(isSelected);

//    if (info()->hasSelectionState())
//        emit selectionStateChanged(info()->selectionState());
//}



//bool StripToolBasicVariable::selectionState() const {
//    if (isValid() && info()->hasSelectionState()) {
//        return info()->selectionState();

//    } else {
//        qDebug() << "StripToolBasicVariable :: attempting to retrieve selection state information for" << name() << "that is not valid!! Big problem!!";
//        return false;
//    }
//}



//void StripToolBasicVariable::onInfoChanged() {
//    // TODO code for updating the pv meta data saved to file goes here.
//}




