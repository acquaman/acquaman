#ifndef ACQMAN_SCANCONFIGURATION_H
#define ACQMAN_SCANCONFIGURATION_H

#include <QObject>
#include "AMRegion.h"

class AMScanConfiguration : public QObject
{
Q_OBJECT

Q_PROPERTY(QString fileName READ fileName WRITE setFileName)
Q_PROPERTY(QString filePath READ filePath WRITE setFilePath)
/*
Q_PROPERTY(int number READ number WRITE setNumber)
Q_PROPERTY(QString sampleName READ sampleName WRITE setSampleName)
Q_PROPERTY(QString comments READ comments WRITE setComments NOTIFY commentsChanged)
Q_PROPERTY(QDateTime startTime READ startTime WRITE setStartTime)
*/

public:
    explicit AMScanConfiguration(QObject *parent = 0);

    /// Returns the name of the file to save raw data in
    QString fileName() const { return fileName_;}
    /// Returns the path to save the raw data file to
    QString filePath() const { return filePath_; }

    /*
signals:
    /// Emitted when comments string changed
    void commentsChanged(const QString &);
*/

public slots:
    /// Sets the file name
    void setFileName(const QString &fileName) { fileName_ = fileName; }
    /// Sets the file path
    void setFilePath(const QString &filePath) { filePath_ = filePath; }

protected:
    /// File name
    QString fileName_;
    /// File path
    QString filePath_;
};

#endif // ACQMAN_SCANCONFIGURATION_H
