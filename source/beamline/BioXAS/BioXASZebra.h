#ifndef BIOXASZEBRA_H
#define BIOXASZEBRA_H

#include <QObject>

#include "beamline/BioXAS/BioXASZebraPulseControl.h"
#include "beamline/BioXAS/BioXASZebraSoftInputControl.h"
#include "beamline/BioXAS/BioXASZebraLogicBlock.h"

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
	/// Returns the pulse control for the given index.
	BioXASZebraPulseControl *pulseControlAt(int index) const;

	/// Returns the list of soft input controls.
	QList<BioXASZebraSoftInputControl *> softInputControls() const;
	/// Returns the soft input control at the given index.
	BioXASZebraSoftInputControl *softInputControlAt(int index) const;

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

protected slots:
	/// Handles changes of the connectivity of the sub controls.
	void onConnectedChanged();

protected:
	/// Flag for holding the connected status.
	bool connected_;

	/// Holds a list of pulse controls.
	QList<BioXASZebraPulseControl *> pulseControls_;
	/// List of soft input controls.
	QList<BioXASZebraSoftInputControl *> softInputControls_;
	/// List of AND blocks.
	QList<BioXASZebraLogicBlock*> andBlocks_;
	/// List of OR blocks.
	QList<BioXASZebraLogicBlock*> orBlocks_;
};

#endif // BIOXASZEBRA_H
