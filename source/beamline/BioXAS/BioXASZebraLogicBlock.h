#ifndef BIOXASZEBRALOGICBLOCK_H
#define BIOXASZEBRALOGICBLOCK_H

#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"

#include "beamline/BioXAS/BioXASZebraLogicBlockInput.h"

#define BIOXASZEBRALOGICBLOCK_INPUT_NUM 4

class BioXASZebraLogicBlock : public AMControl
{
    Q_OBJECT

public:
	/// Enumeration of the possible output status values.
	enum OutputStatus { Off = 0, On = 1 };

	/// Constructor.
	explicit BioXASZebraLogicBlock(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraLogicBlock();

	/// Returns the connected status of the pulse control.
	bool isConnected() const { return connected_; }

	/// Returns the output status.
	bool outputStatus() const;

	/// Returns the list of input controls.
	QList<BioXASZebraLogicBlockInput*> inputControls() const { return inputControls_; }
	/// Returns the output status control.
	AMReadOnlyPVControl* outputStatusControl() const { return outputStatusControl_; }

signals:
	/// Notifier that the output value status has changed.
	void outputStatusChanged(bool);

protected slots:
	/// Sets the connected state.
	void setConnected(bool isConnected);
	/// Updates the connected state.
	void updateConnected();

	/// Handles emitting the output status value changed signal.
	void onOutputStatusValueChanged();

protected:
	/// The connected state.
	bool connected_;
	/// The set of all subcontrols.
	AMControlSet *allControls_;
	/// The list of input controls.
	QList<BioXASZebraLogicBlockInput*> inputControls_;
	/// The output status control.
	AMReadOnlyPVControl *outputStatusControl_;
};

#endif // BIOXASZEBRALOGICBLOCK_H
