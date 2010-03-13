#ifndef SCAN_H
#define SCAN_H

#include <QObject>
#include <QMap>
#include <QDateTime>
#include "Channel.h"

class Scan : public QObject
{
Q_OBJECT
Q_PROPERTY(QString id READ id)
Q_PROPERTY(QString name READ name WRITE setName)
Q_PROPERTY(int number READ number WRITE setNumber)
Q_PROPERTY(QString sampleName READ sampleName WRITE setSampleName)
Q_PROPERTY(QString comments READ comments WRITE setComments NOTIFY commentsChanged)
Q_PROPERTY(QDateTime startTime READ startTime WRITE setStartTime)

friend void Channel::addToScan(Scan& destination);

public:
    explicit Scan(QObject *parent = 0);

    /// Returns scan's unique id
    QString id() const { return id_;}
    /// Returns scan's user given name
    QString name() const { return name_;}
    /// Returns scan's appended number
    int number() const { return number_;}
    /// Returns name of sample
    QString sampleName() const { return sampleName_;}
    /// Returns comments for scan
    QString comments() const { return comments_;}
    /// Returns original start time
    QDateTime startTime() const {return startTime_;}
    /// Returns the full scan name: number appended to name
    QString fullName() const {return QString("%1%2").arg(name_).arg(number_); }

    /// Return number of available channels
    int numChannels() const { return ch_.count();}
    /// Returns specified channel by name: (returns 0 if not found)
    Channel* channel(QString name);
    /// Return specified channel by index: (returns 0 if not found)
    Channel* channel(size_t index) { if(index < (size_t)ch_.count() ) return ch_.at(index); else return 0; }


signals:
    /// Emitted when comments string changed
    void commentsChanged(const QString &);
/*
  Belongs in scan controller
    /// Scan has started
    void started();
    /// Scan completed
    void finished();
    /// Scan canceled by user
    void cancelled();
    /// Scan paused
    void paused();
    /// Scan resumed
    void resumed();
    /// Time left in scan
    void timeRemaining(double seconds);
*/

public slots:
    /// Sets user given name
    void setName(const QString &name) { name_ = name;}
    /// Sets appended number
    void setNumber(int number) { number_ = number;}
    /// Sets name of sample
    void setSampleName(const QString &sampleName) { sampleName_ = sampleName;}
    /// Sets comments for scan
    void setComments(const QString &comments) { comments_ = comments; /* TODO: necessary? emit commentsChanged(comments_);*/ }
    /// Sets original start time
    void setStartTime(const QDateTime &startTime) { startTime_ = startTime;}

    /// Delete a channel from scan:
    bool deleteChannel(Channel* channel);
    bool deleteChannel(const QString& channelName);
    bool deleteChannel(size_t index);

protected:
    /// Unique scan name
    QString id_;

    /// List of channels
    QList<Channel*> ch_;

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
