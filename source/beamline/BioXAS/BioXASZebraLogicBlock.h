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
	/// Enumeration of the possible output state values.
	enum OutputState { Low = 0, High = 1 };

	/// Constructor.
	explicit BioXASZebraLogicBlock(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraLogicBlock();

	/// Returns the connected state.
	bool isConnected() const { return connected_; }

	/// Returns the output state value.
	double outputStateValue() const;
	/// Returns true if the output state is high, false otherwise.
	bool isOutputStateHigh() const;

	/// Returns the input control at the given index.
	BioXASZebraLogicBlockInput* inputControlAt(int index) const;
	/// Returns the list of input controls.
	QList<BioXASZebraLogicBlockInput*> inputControls() const { return inputControls_; }
	/// Returns the output state control.
	AMReadOnlyPVControl* outputStateControl() const { return outputStateControl_; }

signals:
	/// Notifier that the output state value has changed.
	void outputStateChanged(double);
	/// Notifier that the output state value has changed, true if the state is now high.
	void outputStateHighChanged(bool);

public slots:
	/// Sets the value preference for the input control at the given index.
	void setInputValuePreference(int index, int newPreference);

protected slots:
	/// Sets the connected state.
	void setConnected(bool isConnected);
	/// Updates the connected state.
	void updateConnected();

	/// Handles emitting the output state value changed signal.
	void onOutputStateValueChanged();

protected:
	/// The connected state.
	bool connected_;
	/// The set of all subcontrols.
	AMControlSet *allControls_;
	/// The list of input controls.
	QList<BioXASZebraLogicBlockInput*> inputControls_;
	/// The output status control.
	AMReadOnlyPVControl *outputStateControl_;
};

#endif // BIOXASZEBRALOGICBLOCK_H
