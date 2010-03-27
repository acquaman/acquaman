#ifndef ACQMAN_SCANCONTROLLER_H
#define ACQMAN_SCANCONTROLLER_H

#include <QObject>
#include <QMap>
#include <QDateTime>
#include "ScanConfiguration.h"

class AMScanController : public QObject
{
Q_OBJECT
Q_PROPERTY(bool running READ isRunning)
Q_PROPERTY(bool paused READ isPaused)
/*
Q_PROPERTY(QString id READ id)
Q_PROPERTY(QString name READ name WRITE setName)
Q_PROPERTY(int number READ number WRITE setNumber)
Q_PROPERTY(QString sampleName READ sampleName WRITE setSampleName)
Q_PROPERTY(QString comments READ comments WRITE setComments NOTIFY commentsChanged)
Q_PROPERTY(QDateTime startTime READ startTime WRITE setStartTime)
*/

public:
    explicit AMScanController(QObject *parent = 0);

    /// Returns the scan configuration for this controller
    AMScanConfiguration* configuration(){return cfg_;}
    /// Returns true if the scan is running but not paused
    bool isRunning() const {return running_ && !paused_;}
    /// Convenience call, returns true if the scan is not running
    bool isStopped() const {return !isRunning();}
    /// Returns true if the scan is running and paused
    bool isPaused() const {return paused_;}

signals:
    /// New scan configuration is set
    void newConfigurationLoaded();
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


public slots:
    /// Sets a new scan configuration
    virtual void newConfigurationLoad(AMScanConfiguration &cfg) = 0;
    /// Start scan running if not currently running or paused
    virtual void start() = 0;
    /// Cancel scan if currently running or paused
    virtual void cancel() = 0;
    /// Pause scan if currently running
    virtual void pause() = 0;
    /// Resume scan if currently paused
    virtual void resume() = 0;


protected:
    /// Configuration for this scan
    AMScanConfiguration *cfg_;
    /// Holds whether this scan is currently running
    bool running_;
    /// Holds whether this scan is currently paused
    bool paused_;
};

#endif // SCANCONTROLLER_H
