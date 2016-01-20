#ifndef BIOXASZEBRALOGICBLOCKCONTROL_H
#define BIOXASZEBRALOGICBLOCKCONTROL_H

#include "beamline/AMControl.h"
#include "beamline/AMControlSet.h"

#include "beamline/BioXAS/BioXASZebraLogicBlockInputControl.h"

class BioXASZebraLogicBlockControl : public AMControl
{
    Q_OBJECT

public:
	/// Enumeration of the possible output status values.
	enum OutputStatus { Off = 0, On = 1 };

	/// Constructor.
	explicit BioXASZebraLogicBlockControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraLogicBlockControl();

	/// Returns the connected status of the pulse control.
	bool isConnected() const { return connected_; }

	/// Returns the output status.
	bool outputStatus() const;

	/// Returns the list of input controls.
	AMControlSet* inputControlsSet() const { return inputControls_; }
	/// Returns the output status control.
	AMReadOnlyPVControl* outputStatusControl() const { return outputStatusControl_; }

signals:
	/// Notifier that the input controls have changed.
	void inputControlsSetChanged(AMControlSet*);
	/// Notifier that the output value control has changed.
	void outputStatusControlChanged(AMReadOnlyPVControl*);
	/// Notifier that the output value status has changed.
	void outputStatusChanged(bool);

public slots:
	/// Sets the input controls.
	void setInputControlsSet(AMControlSet *newControls);
	/// Sets the output status control.
	void setOutputStatusControl(AMReadOnlyPVControl *newControl);

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
	/// The set of input controls.
	AMControlSet *inputControls_;
	/// The output status control.
	AMReadOnlyPVControl *outputStatusControl_;
};

#endif // BIOXASZEBRALOGICBLOCKCONTROL_H
