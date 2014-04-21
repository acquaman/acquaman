#ifndef AMDETECTORTRIGGERSOURCE_H
#define AMDETECTORTRIGGERSOURCE_H

#include <QObject>
#include "dataman/info/AMDetectorInfo.h"

class AMDetectorTriggerSource : public QObject
{
Q_OBJECT
public:
	/// Constructor takes the programmer unique name for identification
 	virtual ~AMDetectorTriggerSource();
	AMDetectorTriggerSource(const QString &name, QObject *parent = 0);

	/// Returns the programmer name
	QString name() const { return name_; }

public slots:
	/// Call this slot to trigger the source (cause detectors connected to it to acquire)
	void trigger(AMDetectorDefinitions::ReadMode readMode);

	void setSucceeded();
	void setFailed();

signals:
	/// This signal is emitted once trigger is called. Detectors and other classes that need to instantiate an AMDetectorTriggerSource should listen for this signal and cause triggering/acquisition once it is received.
	void triggered(AMDetectorDefinitions::ReadMode readMode);

	void succeeded();
	void failed();

protected:
	/// Holds the programmer name
	QString name_;
};

class AMDetectorDwellTimeSource : public QObject
{
Q_OBJECT
public:
 	virtual ~AMDetectorDwellTimeSource();
	AMDetectorDwellTimeSource(const QString &name, QObject *parent = 0);

	QString name() const { return name_; }

public slots:
	void requestSetDwellTime(double dwellSeconds);
    void requestSetDarkCurrentCorrectionTime(double timeSeconds);

	void setSucceeded();
	void setFailed();

signals:
    void setDwellTime(double dwellSeconds);
    void setDarkCurrentCorrectionTime(double timeSeconds);
    void darkCurrentTimeChanged(double timeSeconds);

	void succeeded();
	void failed();

protected:
	QString name_;
};

#endif // AMDETECTORTRIGGERSOURCE_H
