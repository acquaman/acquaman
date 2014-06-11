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
