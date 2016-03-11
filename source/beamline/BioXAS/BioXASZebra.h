#ifndef BIOXASZEBRA_H
#define BIOXASZEBRA_H

#include <QObject>

#include "actions3/AMAction3.h"

#include "beamline/AMDetectorTriggerSource.h"
#include "beamline/BioXAS/BioXASZebraPulseControl.h"
#include "beamline/BioXAS/BioXASZebraSoftInputControl.h"
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

	/// Returns the trigger source.
	AMZebraDetectorTriggerSource* triggerSource() const { return triggerSource_; }

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

	/// Creates and returns a new action that adds a detector.
	AMAction3* createAddDetectorAction(AMDetector *newDetector);
	/// Creates and returns a new action that removes a detector.
	AMAction3* createRemoveDetectorAction(AMDetector *detector);
	/// Creates and returns a new action that clears the detectors.
	AMAction3* createClearDetectorsAction();

	/// Creates and returns a new action that adds a detector manager.
	AMAction3* createAddDetectorManagerAction(QObject *newManager);
	/// Creates and returns a new action that removes a detector manager.
	AMAction3* createRemoveDetectorManagerAction(QObject *manager);
	/// Creates and returns a new action the clear the detector managers.
	AMAction3* createClearDetectorManagersAction();

signals:
	/// Notifier that the connectivity has changed.
	void connectedChanged(bool);

public slots:
	/// Adds a detector. Returns true if successful, false otherwise.
	bool addDetector(AMDetector *newDetector);
	/// Removes a detector. Returns true if successful, false otherwise.
	bool removeDetector(AMDetector *detector);
	/// Clears all detectors. Returns true if successful, false otherwise.
	bool clearDetectors();

	/// Adds a detector manager. Returns true if successful, false otherwise.
	bool addDetectorManager(QObject *newManager);
	/// Removes a detector manager. Returns true if successful, false otherwise.
	bool removeDetectorManager(QObject *detector);
	/// Clears all detector managers. Returns true if successful, false otherise.
	bool clearDetectorManagers();

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

	/// Trigger source for all Zebra accessory controls.
	AMZebraDetectorTriggerSource *triggerSource_;

	/// Holds a list of pulse controls.
	QList<BioXASZebraPulseControl *> pulseControls_;
	/// List of soft input controls.
	QList<BioXASZebraSoftInputControl *> softInputControls_;
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
