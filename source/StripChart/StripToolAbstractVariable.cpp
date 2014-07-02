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


//#include "StripToolAbstractVariable.h"
//#include <QDebug>

//StripToolAbstractVariable::StripToolAbstractVariable(StripToolAbstractVariableInfo* info, AMDataSource *dataSource, QObject *parent) :
//    QObject(parent)
//{
//    info_ = 0;
//    dataSource_ = 0;

//    infoIsValid_ = false;
//    dataSourceIsValid_ = false;

//    connect( this, SIGNAL(validStateChanged(bool)), this, SLOT(onValidStateChanged(bool)) );

//    setInfo(info);
//    setDataSource(dataSource);

//    qDebug() << "StripToolAbstractVariable object created for" << name();
//}



//bool StripToolAbstractVariable::operator== (const StripToolAbstractVariable* anotherVariable) const {
//    if (this->isValid() && anotherVariable->isValid())
//        return (this->name() == anotherVariable->name());
//    else
//        return false;
//}



//bool StripToolAbstractVariable::isValid() const {
//    return infoIsValid_ && dataSourceIsValid_;
//}



//QString StripToolAbstractVariable::name() const {
//    return info()->name();
//}



//void StripToolAbstractVariable::setInfo(StripToolAbstractVariableInfo *newInfo) {
//    if (info_ != 0)
//        disconnect( info_, SIGNAL(infoChanged()), this, SLOT(onInfoChanged()) );

//    info_ = newInfo;

//    if (info_ != 0) {
//        infoIsValid_ = true;
//        info_->setParent(this);
//        connect( info_, SIGNAL(infoChanged()), this, SLOT(onInfoChanged()) );

//        if (isValid())
//            emit validStateChanged(true);
//        else
//            emit validStateChanged(false);

//    } else {
//        infoIsValid_ = false;
//    }
//}



//StripToolAbstractVariableInfo* StripToolAbstractVariable::info() const {
//    return info_;
//}



//void StripToolAbstractVariable::setDataSource(AMDataSource *newSource) {
//    if (dataSource_ != 0)
//        disconnect( dataSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onDataSourceValuesChanged(AMnDIndex,AMnDIndex)) );

//    dataSource_ = newSource;

//    if (dataSource_ != 0) {
//        dataSourceIsValid_ = true;
//        connect( dataSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex, AMnDIndex)), this, SLOT(onDataSourceValuesChanged(AMnDIndex, AMnDIndex)) );

//        if (isValid())
//            emit validStateChanged(true);

//        else
//            emit validStateChanged(false);

//    } else {
//        dataSourceIsValid_ = false;
//    }
//}



//AMDataSource* StripToolAbstractVariable::dataSource() const {
//    return dataSource_;
//}



//void StripToolAbstractVariable::onValidStateChanged(bool isValid) {
//    if (isValid) {
//        qDebug() << "Variable is now valid: it has both a data source and an info.";
//        setName(dataSource()->name());
//        setDescription("");

//    } else {
//        if (!infoIsValid_)
//            qDebug() << "Variable is not valid : the info has not been set properly.";

//        if (!dataSourceIsValid_)
//            qDebug() << "Variable is not valid : the data source has not been set properly.";
//    }
//}



//void StripToolAbstractVariable::setName(const QString &sourceName) {
//    info()->setName(sourceName);
//}



//void StripToolAbstractVariable::setDescription(const QString &newDescription) {
//    info()->setDescription(newDescription);
//}
