//#include "StripToolDataController.h"

//StripToolDataController::StripToolDataController(QObject *parent) :
//    QObject(parent)
//{
//    model_ = 0;

//    appDirectory_ = 0; // the general directory for the application, usually found in the computer's home directory.
//    pvsDirectory_ = 0; // the location where the pv meta data is saved. This directory is checked when attempting to locate previously loaded pvs.
//    dataDirectory_ = 0; // the directory where the pv data is saved while the application is running, usually found in the application directory.
//    saveDirectory_ = 0; // the suggested directory where the user can choose to save pv data when the application terminates, so it won't be deleted!

//    setDirectories("app");
//}



//StripToolDataController::~StripToolDataController()
//{
//}



//void StripToolDataController::setModel(StripToolModel *newModel)
//{
//    model_ = newModel;
//    connect( this, SIGNAL(addPV(const QString &)), model_, SLOT(toAddPV(const QString &)) );
//    connect( this, SIGNAL(setMetaData(QString, QList<QString>)), model_, SLOT(toSetMetaData(QString, QList<QString>)) );

//    connect( model_, SIGNAL(metaDataCheck(QString)), this, SLOT(toCheckForMetaData(QString)) );
//    connect( model_, SIGNAL(savePVData(QObject*)), this, SLOT(toSavePVData(QObject*)) );
//    connect( model_, SIGNAL(savePVMetadata(QObject*)), this, SLOT(toSavePVMetaData(QObject*)) );
//    connect( model_, SIGNAL(deletePVData(QObject*)), this, SLOT(toDeletePVData(QObject*)) );
//}


//void StripToolDataController::setDirectories(const QString &dir)
//{
//    QDir home = QDir(QDir::homePath());
//    appDirectory_ = QDir( home.filePath("StripTool") );

//    if (!appDirectory_.exists())
//    {
//        home.mkdir("StripTool");
//    }

//    dataDirectory_ = QDir( appDirectory_.absoluteFilePath("data") );

//    if (!dataDirectory_.exists())
//    {
//        appDirectory_.mkdir("data");
//    }

//    pvsDirectory_ = QDir( appDirectory_.absoluteFilePath("pvs") );

//    if (!pvsDirectory_.exists())
//    {
//        appDirectory_.mkdir("pvs");
//    }

//    if (dir == "")
//    {
//        // do nothing

//    } else if (dir == "pvs") {

//        appDirectory_.setCurrent(appDirectory_.absoluteFilePath("pvs"));

//    } else if (dir == "data") {

//        appDirectory_.setCurrent(appDirectory_.absoluteFilePath("data"));

//    } else if (dir == "app") {

//        home.setCurrent(home.absoluteFilePath("StripTool"));

//    } else {

//        qDebug() << "Unable to set current directory to " << dir;

//    }
//}



//QString StripToolDataController::pvNameToFilename(const QString &pvName) const
//{
//    QString name = pvName;
//    return name.replace(":", "_").append(".txt");
//}



//QString StripToolDataController::filenameToPvName(const QString &filename) const
//{
//    QString name = filename;
//    return name.replace("_", ":").remove(".txt");
//}



//QList<QString> StripToolDataController::getReloadList()
//{
//    setDirectories("pvs");

//    QList<QString> pvsContents = pvsDirectory_.entryList();
//    QList<QString> reloadList;

//    foreach(QString entry, pvsContents)
//    {
//        if (entry != "." && entry != "..")
//            reloadList.append( filenameToPvName(entry) );
//    }

//    return reloadList;
//}



//QList<QString> StripToolDataController::getDataList()
//{
//    setDirectories("data");

//    QList<QString> dataContents = pvsDirectory_.entryList();
//    QList<QString> dataList;

//    foreach(QString entry, dataContents)
//    {
//        if (entry != "." && entry != "..")
//            dataList.append( filenameToPvName(entry) );
//    }

//    return dataList;
//}



//void StripToolDataController::checkForPreviousPVs()
//{
//    setDirectories("pvs");
//    QList<QString> pvsList = getReloadList();

//    if (pvsList.size() > 0)
//        showReloadDialog(pvsList);
//}



//void StripToolDataController::showReloadDialog(QList<QString> pvsList)
//{
//    ReloadPVDialog reloadDialog(pvsList);
//    QList<QString> reloadList;

//    if (reloadDialog.exec())
//    {
//        reloadList = reloadDialog.pvNamesList();

//        foreach(QString name, reloadList)
//        {
//            qDebug() << "Name from file : " << name;
//            emit addPV(name);
//        }
//    }
//}



//bool StripToolDataController::metaDataExists(const QString &pvName)
//{
//    QList<QString> savedPVs = getReloadList();

//    if (savedPVs.contains(pvName))
//        return true;
//    else
//        return false;
//}



//bool StripToolDataController::dataExists(const QString &pvName)
//{
//    QList<QString> savedPVs = getDataList();

//    if (savedPVs.contains(pvName))
//        return true;
//    else
//        return false;
//}



//void StripToolDataController::toSavePVData(QObject *toSave)
//{
////    StripToolPV *pv = (StripToolPV *) toSave;

////    // check that metadata exists for this pv.
////    if (!metaDataExists(pv->name()))
////        savePVMetaData(pv);

//    // write the new data to the pv's data file.
////    qDebug() << "Saving collected data for pv" << pv->name() << ":" << savePVData(pv);
//}



//void StripToolDataController::toSavePVMetaData(QObject *toSave)
//{
////    StripToolPV *pv = (StripToolPV*) toSave;

////    // check that collected data exists for this pv.
////    if (!dataExists(pv->name()))
////        savePVData(pv);

//    // write the new meta data to the pv's reload file.
////    qDebug() << "Saving meta data for pv" << pv->name() << ":" << savePVMetaData(pv);
//}



//bool StripToolDataController::metaDataHeaderCheck(StripToolPV *toSave)
//{
////    int headerCount = toSave->metaDataHeaders().size();
////    int dataCount = toSave->metaData().size();

////    if (headerCount != dataCount)
////    {
////        qDebug() << "ERROR : number of metadata entries do not match the number of headers for this pv : " << toSave->name();
////        return false;
////    }

//    return true;
//}



////bool StripToolDataController::savePVMetaData(StripToolPV *toSave)
////{
////    setDirectories("pvs");

////    QString filename = pvNameToFilename(toSave->name());

////    if (filename.isEmpty())
////        return false;

////    QFile file(filename);

////    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
////    {
////        qDebug() << file.errorString();
////        return false;
////    }

////    if (!metaDataHeaderCheck(toSave))
////        return false;

////    // create list of data to replace the (possibly already stored) meta data.

////    QList<QString> headers = toSave->metaDataHeaders();
////    QList<QString> metaData = toSave->metaData();
////    QList<QString> fileContents;

////    for (int i = 0; i < metaData.count(); i++)
////    {
////        QString entry = headers.at(i) + "\t" + metaData.at(i);
////        fileContents.append(entry);
////    }

////    // write data to file.

////    QTextStream out(&file);

////    foreach(QString fileEntry, fileContents)
////        out << fileEntry << "\n";

////    file.close();

////    if (getReloadList().contains(toSave->name()))
////    {
////        return true;
////    }

////    return false;
////}



////bool StripToolDataController::savePVData(StripToolPV *toSave)
////{
////    setDirectories("data");
////    QString filename = pvNameToFilename(toSave->name());

////    if (filename.isEmpty())
////        return false;

////    QFile file(filename);

////    if (!file.open(QIODevice::Append | QIODevice::Text))
////    {
////        qDebug() << file.errorString();
////        return false;
////    }

////    // identify lists of data to be added to data file.

////    QList<QString> timesToSave = toSave->saveMasterTimes().toList();

////    QList<double> dataToSave = toSave->saveMasterValues().toList();

////    // write data to file.

////    QTextStream out(&file);

////    for (int i = 0; i < dataToSave.size(); i++)
////    {
////        out << timesToSave.at(i) << "\t" << dataToSave.at(i) << "\n";
////    }

////    file.close();

////    if (getDataList().contains(toSave->name()))
////        return true;
////    else
////        return false;
////}



//void StripToolDataController::toDeletePVData(QObject *toDelete)
//{
//    StripToolPV *pv = (StripToolPV*) toDelete;

//    if (metaDataExists(pv->name()))
//        qDebug() << "Deleting metadata for pv" << pv->name() << ":" << deleteMetaData(pv->name());

//    if (dataExists(pv->name()))
//        qDebug() << "Deleting collected data for pv" << pv->name() << ":" << deleteData(pv->name());
//}



//bool StripToolDataController::deleteMetaData(const QString &pvName)
//{
//    setDirectories("pvs");
//    QFile toDelete(pvNameToFilename(pvName));
//    return toDelete.remove();
//}



//bool StripToolDataController::deleteData(const QString &pvName)
//{
//    setDirectories("data");
//    QFile toDelete(pvNameToFilename(pvName));
//    return toDelete.remove();
//}



//QList<QString> StripToolDataController::loadMetaData(const QString &pvName)
//{
//    setDirectories("pvs");

//    QList<QString> metaData;
//    QList<QString> emptyList;

//    QFile file(pvNameToFilename(pvName));

//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//    {
//        qDebug() << file.errorString();
//        return emptyList;
//    }

//    QTextStream in(&file);
//    QList<QString> fileContents;

//    while(!in.atEnd())
//    {
//        QString line = in.readLine();
//        fileContents.append(line);
//    }

//    int entryNum = fileContents.size();

//    if (entryNum == 0)
//    {
//        qDebug() << "An apparently empty meta data file has been found for pv" << pvName;
//        return emptyList;
//    }

//    foreach (QString entry, fileContents)
//    {
//        int length = entry.size();
//        for (int i = 0; i < length; i++)
//        {
//            if (entry.at(i).isSpace())
//            {
//                QString data = entry.mid(i).trimmed();
//                metaData.append(data);
//                break;
//            }
//        }
//    }

//    if (metaData.size() != entryNum)
//    {
//        qDebug() << "The number of meta data items is different than the number of file entries!! Cannot load pv" << pvName;
//        return emptyList;
//    }

//    return metaData;
//}



//void StripToolDataController::toCheckForMetaData(const QString &pvName)
//{
//    qDebug() << "Checking for pre-existing meta data...";

//    if (metaDataExists(pvName))
//    {
//        QList<QString> metaData = loadMetaData(pvName);
//        qDebug() << "Found : " << metaData;
//        emit setMetaData(pvName, metaData);

//    } else {

//        qDebug() << "No metadata found for pv" << pvName;
//    }
//}




