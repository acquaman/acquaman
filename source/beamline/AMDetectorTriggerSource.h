#ifndef AMDETECTORTRIGGERSOURCE_H
#define AMDETECTORTRIGGERSOURCE_H

#include <QObject>
#include "dataman/info/AMDetectorInfo.h"

class AMDetectorTriggerSource : public QObject
{
Q_OBJECT
public:
	/// Constructor takes the programmer unique name for identification
	AMDetectorTriggerSource(const QString &name, QObject *parent = 0);

	/// Returns the programmer name
	QString name() const { return name_; }

public slots:
	/// Call this slot to trigger the source (cause detectors connected to it to acquire)
	void trigger(AMDetectorDefinitions::ReadMode readMode);

signals:
	/// This signal is emitted once trigger is called. Detectors and other classes that need to instantiate an AMDetectorTriggerSource should listen for this signal and cause triggering/acquisition once it is received.
	void triggered(AMDetectorDefinitions::ReadMode readMode);

protected:
	/// Holds the programmer name
	QString name_;
};

#endif // AMDETECTORTRIGGERSOURCE_H
