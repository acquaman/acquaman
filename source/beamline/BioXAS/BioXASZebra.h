#ifndef BIOXASZEBRA_H
#define BIOXASZEBRA_H

#include <QObject>

#include "beamline/BioXAS/BioXASZebraPulseControl.h"
#include "beamline/BioXAS/BioXASZebraLogicBlock.h"

#include "beamline/AMPVControl.h"

#include <QList>
#include <QSignalMapper>

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
	/// Returns the pulse control for the given index.
	BioXASZebraPulseControl *pulseControlAt(int index) const;

	/// Returns the list of soft input controls.
	QList<AMPVControl *> softInputControls() const;
	/// Returns the soft input control at the given index.
	AMPVControl *softInputControlAt(int index) const;

	/// Returns the list of AND blocks.
	QList<BioXASZebraLogicBlock*> andBlocks() const;
	/// Returns the AND block at the given index.
	BioXASZebraLogicBlock* andBlockAt(int index) const;

	/// Returns the list of OR blocks.
	QList<BioXASZebraLogicBlock*> orBlocks() const;
	/// Returns the OR block at the given index.
	BioXASZebraLogicBlock* orBlockAt(int index) const;

signals:
	/// Notifier that the connectivity has changed.
	void connectedChanged(bool);

public slots:

protected slots:
	/// Handles changes of the connectivity of the sub controls.
	void onConnectedChanged();

	/// Adds a pulse control to the list of synchronized pulse controls. Returns true if the control was successfully added, false otherwise.
	bool addSynchronizedPulseControl(BioXASZebraPulseControl *newControl);
	/// Removes a pulse control from the list of synchronized pulse controls. Returns true if the control was successfully removed, false otherwise.
	bool removeSynchronizedPulseControl(BioXASZebraPulseControl *newControl);

	/// Updates the list of synchronized pulse controls with the given control object's 'delay before' value.
	void onSynchronizedDelayBeforeValueChanged(QObject *controlObject);
	/// Updates the list of synchronized pulse controls with the given control object's 'pulse width' value.
	void onSynchronizedPulseWidthValueChanged(QObject *controlObject);
	/// Updates the list of synchronized pulse control with the given control object's 'time units' value.
	void onSynchronizedTimeUnitsValueChanged(QObject *controlObject);

protected:
	/// Flag for holding the connected status.
	bool connected_;

	/// Holds a list of pulse controls.
	QList<BioXASZebraPulseControl *> pulseControls_;
	/// List of soft input controls.
	QList<AMPVControl *> softInputControls_;
	/// List of AND blocks.
	QList<BioXASZebraLogicBlock*> andBlocks_;
	/// List of OR blocks.
	QList<BioXASZebraLogicBlock*> orBlocks_;

	/// List of synchronized pulse controls.
	QList<BioXASZebraPulseControl*> synchronizedPulseControls_;
	/// The 'delay before' value signal mapper for synchronized pulse controls.
	QSignalMapper *synchronizedDelayBeforeMapper_;
	/// The 'pulse width' value signal mapper for synchronized pulse controls.
	QSignalMapper *synchronizedPulseWidthMapper_;
	/// The 'time units' value signal mapper for synchronized pulse controls.
	QSignalMapper *synchronizedTimeUnitsMapper_;
};

#endif // BIOXASZEBRA_H
