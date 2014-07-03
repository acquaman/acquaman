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


//<<<<<<< HEAD
//#ifndef STRIPTOOLDATACONTROLLER_H
//#define STRIPTOOLDATACONTROLLER_H

//#include <QObject>
//#include <QtGui>
//#include <QDir>

//#include "StripChart/ReloadPVDialog.h"
//#include "StripChart/StripToolModel.h"
//#include "StripChart/StripToolPV.h"

//class StripToolDataController : public QObject
//{
//    Q_OBJECT

//public:
//    explicit StripToolDataController(QObject *parent = 0);
//    virtual ~StripToolDataController();

//signals:
//    void addPV(const QString &pvName);
//    void setMetaData(const QString &pvName, QList<QString> metaData);

//protected:
//    StripToolModel *model_;

//    QDir appDirectory_;
//    QDir dataDirectory_;
//    QDir saveDirectory_;
//    QDir pvsDirectory_;

//public:
//    void setModel(StripToolModel *newModel);
//    void checkForPreviousPVs();

//protected:
//    void setDirectories(const QString &dir);
//    QString pvNameToFilename(const QString &pvName) const;
//    QString filenameToPvName(const QString &filename) const;
//    QList<QString> getReloadList();
//    QList<QString> getDataList();
//    void showReloadDialog(QList<QString> pvsList);
//    bool metaDataExists(const QString &pvName);
//    bool dataExists(const QString &pvName);
//    bool metaDataHeaderCheck(StripToolPV *toSave);
//    bool savePVMetaData(StripToolPV *toSave);
//    bool savePVData(StripToolPV *toSave);
//    bool deleteMetaData(const QString &pvName);
//    bool deleteData(const QString &pvName);
//    QList<QString> loadMetaData(const QString &pvName);

//public slots:
//    void toCheckForMetaData(const QString &pvName);
//    void toSavePVData(QObject *toSave);
//    void toSavePVMetaData(QObject *toSave);
//    void toDeletePVData(QObject *toDelete);

//};

//#endif // STRIPTOOLDATACONTROLLER_H
//=======
//#ifndef STRIPTOOLDATACONTROLLER_H
//#define STRIPTOOLDATACONTROLLER_H

//#include <QObject>
//#include <QtGui>
//#include <QDir>

//#include "StripChart/ReloadPVDialog.h"
//#include "StripChart/StripToolModel.h"
//#include "StripChart/StripToolPV.h"

//class StripToolDataController : public QObject
//{
//    Q_OBJECT

//public:
//    explicit StripToolDataController(QObject *parent = 0);
//    ~StripToolDataController();

//signals:
//    void addPV(const QString &pvName);
//    void setMetaData(const QString &pvName, QList<QString> metaData);

//protected:
//    StripToolModel *model_;

//    QDir appDirectory_;
//    QDir dataDirectory_;
//    QDir saveDirectory_;
//    QDir pvsDirectory_;

//public:
//    void setModel(StripToolModel *newModel);
//    void checkForPreviousPVs();

//protected:
//    void setDirectories(const QString &dir);
//    QString pvNameToFilename(const QString &pvName) const;
//    QString filenameToPvName(const QString &filename) const;
//    QList<QString> getReloadList();
//    QList<QString> getDataList();
//    void showReloadDialog(QList<QString> pvsList);
//    bool metaDataExists(const QString &pvName);
//    bool dataExists(const QString &pvName);
//    bool metaDataHeaderCheck(StripToolPV *toSave);
////    bool savePVMetaData(StripToolPV *toSave);
////    bool savePVData(StripToolPV *toSave);
//    bool deleteMetaData(const QString &pvName);
//    bool deleteData(const QString &pvName);
//    QList<QString> loadMetaData(const QString &pvName);

//public slots:
//    void toCheckForMetaData(const QString &pvName);
//    void toSavePVData(QObject *toSave);
//    void toSavePVMetaData(QObject *toSave);
//    void toDeletePVData(QObject *toDelete);

//};

//#endif // STRIPTOOLDATACONTROLLER_H
//>>>>>>> master
