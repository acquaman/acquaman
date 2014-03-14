#include "StripToolVariableInfoImporter.h"
#include <QDebug>

StripToolVariableInfoImporter::StripToolVariableInfoImporter(QObject *parent) :
    QObject(parent)
{
    importDirectory_ = 0;

    defaultSettings();

    qDebug() << "StripToolVariableInfoImporter object created.";
}



void StripToolVariableInfoImporter::setImportDirectory(QDir newImportDirectory)
{
    if (newImportDirectory.exists())
        importDirectory_ = newImportDirectory;
    else
        qDebug() << "StripToolVariableInfoImporter::setImportDirectory : cannot import from a nonexistant directory! No changes made to import directory.";
}



void StripToolVariableInfoImporter::importInfos() {
    QList<QString> filesToAdd = getImportListFromDirectory(importDirectory());

    qDebug() << "StripToolVariableInfoImporter::importInfos() : Files to import : " << filesToAdd;

    foreach (QString file, filesToAdd) {
        StripToolVariableInfo* variableInfo = loadInfoFromFile(file);
        emit importedInfo(variableInfo);
    }
}



QDir StripToolVariableInfoImporter::importDirectory() const {
    return importDirectory_;
}



bool StripToolVariableInfoImporter::infoExists(const QString &pvName) const
{
    QList<QString> previousNames = getImportListFromDirectory(importDirectory_);

    if (previousNames.contains(pvName)) {
        return true;

    } else {
        return false;
    }
}



QString StripToolVariableInfoImporter::filenameToVariableName(const QString &filename) const {
    QString name = filename;
    return name.replace("_", ":").remove(".txt");
}



QList<QString> StripToolVariableInfoImporter::getImportListFromDirectory(QDir importDirectory) const {

    QList<QString> dirContents = importDirectory.entryList();
    QList<QString> reloadList;

    foreach(QString entry, dirContents)
    {
        if (entry != "." && entry != "..")
            reloadList.append( entry );
    }

    qDebug() << "StripToolVariableInfoImporter::getImportListFromDirectory(...) : " << reloadList.length() << " files to import.";

    return reloadList;
}



StripToolVariableInfo* StripToolVariableInfoImporter::loadInfoFromFile(const QString &filename) const
{
    StripToolVariableInfo *newInfo = new StripToolVariableInfo();

    QDir::setCurrent(importDirectory().path());
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << file.errorString();
        return 0;
    }

    QTextStream in(&file);
    QList<QString> fileContents;

    while (!in.atEnd()) {
        QString line = in.readLine();
        fileContents.append(line);
    }

    file.close();

    foreach (QString entry, fileContents) {
        int length = entry.size();

        for (int i = 0; i < length; i++) {

            if (entry.at(i) == ':') {
                QString header = entry.mid(0, i - 1).trimmed();
                QString data = entry.mid(i + 1).trimmed();

                if (header == "Name") {
                    qDebug() << "StripToolVariableInfoImporter : setting info name from file.";
                    newInfo->setName(data);

                } else if (header == "Description") {
                    qDebug() << "StripToolVariableInfoImporter : setting description from file.";
                    newInfo->setDescription(data);

                } else if (header == "Units") {
                    qDebug() << "StripToolVariableInfoImporter : setting units from file.";
                    newInfo->setUnits(data);

                } else if (header == "Color") {
                    qDebug() << "StripToolVariableInfoImporter : setting color from file.";
                    newInfo->setColor(QColor(data));

                } else if (header == "Granularity") {
                    qDebug() << "StripToolVariableInfoImporter : setting granularity from file.";
                    newInfo->setGranularity(data.toInt());
                }
            }
        }
    }

    return newInfo;
}



void StripToolVariableInfoImporter::defaultSettings()
{

}

