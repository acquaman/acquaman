#ifndef BIOXASZEBRAINPUT_H
#define BIOXASZEBRAINPUT_H

#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"

class BioXASZebraInput : public AMControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASZebraInput(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraInput();

	/// Returns the connected status.
	bool isConnected() const { return connected_; }

	/// Returns the current input value.
	int inputValue() const;
	/// Returns the current input value as a string.
	QString inputValueString() const;
	/// Returns the input status value.
	bool inputStatus() const;

	/// Returns the input value control.
	AMPVControl* inputValueControl() const { return valueControl_; }
	/// Returns the input status control.
	AMReadOnlyPVControl* inputStatusControl() const { return statusControl_; }

signals:
	/// Notifier that the connected state has changed.
	void connectedChanged(bool);
	/// Notifier that the input value has changed.
	void inputValueChanged(int);
	/// Notifier that the input value has changed, as a string.
	void inputValueStringChanged(const QString &);
	/// Notifier that the input status has changed.
	void inputStatusChanged(bool);

public slots:
	/// Sets the input value.
	void setInputValue(int value);

protected slots:
	/// On control set bool changed.
	void onControlSetConnectedChanged(bool connected);
	/// Handles emitting the input value changed signal.
	void onInputValueChanged();
	/// Handles emitting the input status value changed signal.
	void onInputStatusChanged();

protected:
	/// The connected state.
	bool connected_;

	/// The set of subcontrols.
	AMControlSet *allControls_;
	/// The input value control.
	AMPVControl *valueControl_;
	/// The input status control.
	AMReadOnlyPVControl *statusControl_;
};

#endif // BIOXASZEBRAINPUT_H
