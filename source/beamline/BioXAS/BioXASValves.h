#ifndef BIOXASVALVES_H
#define BIOXASVALVES_H

#include "beamline/BioXAS/BioXASBiStateGroup.h"

class BioXASValves : public BioXASBiStateGroup
{
    Q_OBJECT

public:
	/// Enumeration of the possible value states.
	enum Value { Open = 0, Closed = 1 };

	/// Constructor.
	explicit BioXASValves(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASValves();

	QList<AMControl*> valvesList() const { return children_; }

signals:
	/// Notifier that the valves have changed.
	void valvesChanged();

public slots:
	/// Adds a valve control.
	void addValve(AMControl *newShutter, double openValue, double closedValue);
	/// Removes a valve control.
	void removeValve(AMControl *newValve);
	/// Clears all valve controls.
	void clearValves();
};

#endif // BIOXASVALVES_H
