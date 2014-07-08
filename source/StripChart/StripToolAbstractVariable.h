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


//#ifndef STRIPTOOLABSTRACTVARIABLE_H
//#define STRIPTOOLABSTRACTVARIABLE_H

//#include <QObject>

//#include "dataman/datasource/AMDataSource.h"
//#include "StripChart/StripToolAbstractVariableInfo.h"

//class AMnDIndex;
//class MPlotItem;

//class StripToolAbstractVariable : public QObject
//{
//    Q_OBJECT

//public:
//    explicit StripToolAbstractVariable(StripToolAbstractVariableInfo *info, AMDataSource *dataSource, QObject *parent = 0);

//signals:
//    void validStateChanged(bool isValid);

//public:
//    bool operator== (const StripToolAbstractVariable* anotherVariable) const;
//    bool isValid() const;
//    QString name() const;
//    virtual MPlotItem* plotItem() const = 0;

//protected:
//    void setInfo(StripToolAbstractVariableInfo *newInfo);
//    virtual StripToolAbstractVariableInfo* info() const;

//    void setDataSource(AMDataSource *newSource);
//    virtual AMDataSource* dataSource() const;

//protected slots:
//    void onValidStateChanged(bool isValid);

//private slots:
//    virtual void onInfoChanged() = 0;
//    virtual void onDataSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end) = 0;

//private:
//    void setName(const QString &sourceName);
//    void setDescription(const QString &newDescription);

//private:
//    StripToolAbstractVariableInfo *info_;
//    AMDataSource *dataSource_;

//    bool infoIsValid_;
//    bool dataSourceIsValid_;

//};

//#endif // STRIPTOOLABSTRACTVARIABLE_H
