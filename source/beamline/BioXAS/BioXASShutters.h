#ifndef BIOXASSHUTTERS_H
#define BIOXASSHUTTERS_H

#include "beamline/BioXAS/BioXASBiStateGroup.h"

class BioXASShutters : public BioXASBiStateGroup
{
	Q_OBJECT

public:
	/// Enumeration of the possible value states.
	enum Value { Open = 0, Closed = 1 };

	/// Constructor.
	BioXASShutters(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASShutters();

	/// Returns the list of shutters.
	QList<AMControl*> shuttersList() const { return children_; }

signals:
	/// Notifier that the shutters have changed.
	void shuttersChanged();

public slots:
	/// Adds a shutter control.
	void addShutter(AMControl *newShutter, double openValue, double closedValue);
	/// Removes a shutter control.
	void removeShutter(AMControl *newValve);
	/// Clears all shutter controls.
	void clearShutters();
};

#endif // BIOXASSHUTTERS_H
