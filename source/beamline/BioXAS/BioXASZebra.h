#ifndef BIOXASZEBRA_H
#define BIOXASZEBRA_H

#include "beamline/AMConnectedControl.h"

#include "beamline/BioXAS/BioXASZebraPulseControl.h"
#include "beamline/BioXAS/BioXASZebraSoftInputControl.h"
#include "beamline/BioXAS/BioXASZebraLogicBlock.h"

#include "beamline/AMPVControl.h"

#include <QList>

/// This is a class for controlling the zebra triggering box.
class BioXASZebra : public AMConnectedControl
{
	Q_OBJECT
public:
	/// Constructor.
	explicit BioXASZebra(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASZebra();

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

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();

protected:
	/// Returns true if the controls are connected, false otherwise.
	bool controlsConnected() const;

protected:
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
