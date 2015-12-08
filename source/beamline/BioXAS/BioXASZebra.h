#ifndef BIOXASZEBRA_H
#define BIOXASZEBRA_H

#include <QObject>

#include "beamline/BioXAS/BioXASZebraPulseControl.h"
#include "beamline/AMPVControl.h"

#include <QList>

/// This is a class for controlling the zebra triggering box.
class BioXASZebra : public QObject
{
	Q_OBJECT
public:
	/// Constructor.
	explicit BioXASZebra(const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASZebra();

	/// Returns the connected status of the zebra.
	bool isConnected() const;

	/// Returns the list of zebra pulse controls.
	QList<BioXASZebraPulseControl *> pulseControls() const;

signals:
	/// Notifier that the connectivity has changed.
	void connectedChanged(bool);

public slots:

protected slots:
	/// Handles changes of the connectivity of the sub controls.
	void onConnectedChanged();

protected:
	/// Flag for holding the connected status.
	bool connected_;

	/// Holds a list of pulse controls.
	QList<BioXASZebraPulseControl *> pulseControls_;
};

#endif // BIOXASZEBRA_H
