#ifndef BIOXASCARBONFILTERFARM_H
#define BIOXASCARBONFILTERFARM_H

#include <QObject>

#include "beamline/AMControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmControl.h"

class BioXASCarbonFilterFarm : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarm(const QString &name, const QString &pvBase, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarm();

	/// Returns the name.
	QString name() const { return name_; }
	/// Returns true if the filter farm is connected, false otherwise.
	bool isConnected const { return connected_; }

	/// Returns the total filter control.
	AMControl* filterControl() const { return filter_; }

signals:
	/// Notifier that the connected state has changed.
	void connectedChanged(bool isConnected);

protected slots:
	/// Sets the connected state.
	void setConnected(bool isConnected);

protected:
	/// The name.
	QString name_;
	/// The current connected state.
	bool connected_;
	/// The total filter control.
	BioXASCarbonFilterFarmControl *filter_;

	/// The upstream position motor control.
	CLSMAXvMotor *upstreamPosition_;
	/// The upstream status control.
	AMReadOnlyPVControl *upstreamStatus_;
	/// The downstream position motor control.
	CLSMAXvMotor *downstreamPo
};

#endif // BIOXASCARBONFILTERFARM_H
