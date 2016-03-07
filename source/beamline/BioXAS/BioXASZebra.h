#ifndef BIOXASZEBRA_H
#define BIOXASZEBRA_H

#include <QObject>

#include "beamline/AMTriggerManager.h"

#include "beamline/BioXAS/BioXASZebraPulseControl.h"
#include "beamline/BioXAS/BioXASZebraSoftInputControl.h"
#include "beamline/BioXAS/BioXASZebraLogicBlock.h"

#include "beamline/AMPVControl.h"

#include "beamline/AMDetectorTriggerSource.h"

#include <QList>
#include <QSignalMapper>

/// This is a class for controlling the zebra triggering box.
class BioXASZebra : public AMTriggerManager
{
	Q_OBJECT
public:
	/// Constructor.
	explicit BioXASZebra(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASZebra();

	/// Returns the list of zebra pulse controls.
	AMControlSet* pulseControls() const { return pulseControls_; }
	/// Returns the pulse control for the given index.
	BioXASZebraPulseControl *pulseControlAt(int index) const;

	/// Returns the list of soft input controls.
	AMControlSet* softInputControls() const { return softInputControls_; }
	/// Returns the soft input control at the given index.
	BioXASZebraSoftInputControl *softInputControlAt(int index) const;

	/// Returns the list of AND blocks.
	AMControlSet* andBlocks() const { return andBlocks_; }
	/// Returns the AND block at the given index.
	BioXASZebraLogicBlock* andBlockAt(int index) const;

	/// Returns the list of OR blocks.
	AMControlSet* orBlocks() const { return orBlocks_; }
	/// Returns the OR block at the given index.
	BioXASZebraLogicBlock* orBlockAt(int index) const;

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
	/// Returns true if the manager is connected. Reimplemented to include the states of the pulse controls, soft input controls, and blocks, and or blocks.
	virtual bool managerConnected() const;

protected:
	/// Holds a list of pulse controls.
	AMControlSet *pulseControls_;
	/// List of soft input controls.
	AMControlSet *softInputControls_;
	/// List of AND blocks.
	AMControlSet *andBlocks_;
	/// List of OR blocks.
	AMControlSet *orBlocks_;

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
