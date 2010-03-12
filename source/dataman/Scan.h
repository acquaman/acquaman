#ifndef SCAN_H
#define SCAN_H

#include <QObject>

class Scan : public QObject
{
Q_OBJECT
Q_PROPERTY(QString id READ id)
Q_PROPERTY(QString name READ name WRITE setName)
Q_PROPTERY(int number READ number WRITE setNumber)
Q_PROPERTY(QString sampleName READ sampleName WRITE setSampleName)
Q_PROPERTY(QString comments READ comments WRITE setComments NOTIFY commentsChanged)
Q_PROPERTY(QDateTime startTime READ startTime WRITE setStartTime)
public:
    explicit Scan(QObject *parent = 0);

    /// Returns scan's unique id
    QString id() const;
    /// Returns scan's user given name
    QString name() const;
    /// Returns scan's appended number
    int number() const;
    /// Returns name of sample
    QString sampleName() const;
    /// Returns comments for scan
    QString comments() const;
    /// Returns original start time
    QDateTime startTime() const;
    /// Returns the full scan name: number appended to name
    QString fullName() const;

signals:
    /// Emitted when comments string changed
    void commentsChanged(const QString &);

public slots:
    /// Sets user given name
    void setName(const QString &name);
    /// Sets appended number
    void setNumber(int number);
    /// Sets name of sample
    void setSampleName(const QString &sampleName);
    /// Sets comments for scan
    void setComments(const QString &comments);
    /// Sets original start time
    void setStartTime(const QDateTime &startTime);

protected:
    /// Unique scan name
    QString id_;

    /// List of channels
    QList<Channel> ch_;

    /// User defined scan name
    QString name_;
    /// Number to be appended to scan name
    int number_;
    /// Sample name
    QString sampleName_;
    /// Commments for scan
    QString comments_;
    /// Start time of original scan
    QDateTime startTime_;
};

#endif // SCAN_H
