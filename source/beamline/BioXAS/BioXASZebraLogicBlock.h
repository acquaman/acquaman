#ifndef BIOXASZEBRALOGICBLOCK_H
#define BIOXASZEBRALOGICBLOCK_H

#include "beamline/AMConnectedControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"

#include "beamline/BioXAS/BioXASZebraLogicBlockInput.h"

#define BIOXASZEBRALOGICBLOCK_INPUT_NUM 4

class BioXASZebraLogicBlock : public AMConnectedControl
{
    Q_OBJECT

public:
	/// Enumeration of the possible output state values.
	enum OutputState { Low = 0, High = 1 };

	/// Constructor.
	explicit BioXASZebraLogicBlock(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraLogicBlock();

	/// Returns the output state value.
	double outputStateValue() const;
	/// Returns true if the output state is high, false otherwise.
	bool isOutputStateHigh() const;

	/// Returns the list of input controls.
	QList<BioXASZebraLogicBlockInput*> inputControls() const { return inputControls_; }
	/// Returns the output state control.
	AMReadOnlyPVControl* outputStateControl() const { return outputStateControl_; }

signals:
	/// Notifier that the output state value has changed.
	void outputStateChanged(double);
	/// Notifier that the output state value has changed, true if the state is now high.
	void outputStateHighChanged(bool);

protected slots:
	/// Handles emitting the output state value changed signal.
	void onOutputStateValueChanged();

protected:
	/// The list of input controls.
	QList<BioXASZebraLogicBlockInput*> inputControls_;
	/// The output status control.
	AMReadOnlyPVControl *outputStateControl_;
};

#endif // BIOXASZEBRALOGICBLOCK_H
