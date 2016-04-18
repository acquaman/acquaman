#ifndef BIOXASZEBRALOGICBLOCKINPUT_H
#define BIOXASZEBRALOGICBLOCKINPUT_H

#include "beamline/BioXAS/BioXASZebraInput.h"

class BioXASZebraLogicBlockInput : public BioXASZebraInput
{
    Q_OBJECT

public:
	/// Enumeration of the possible enabled states.
	enum EnabledState { NotEnabled = 0, Enabled = 1 };
	/// Enumeration of the possible inverted states.
	enum InvertedState { NotInverted = 0, Inverted = 1 };

	/// Constructor.
	explicit BioXASZebraLogicBlockInput(const QString &name, const QString &baseName, int inputIndex, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraLogicBlockInput();

	/// Returns the current enabled state value.
	double enabledState() const;
	/// Returns the current inverted state value.
	double invertedState() const;

	/// Returns true if the input is enabled.
	bool isEnabled() const;
	/// Returns true if the input is inverted.
	bool isInverted() const;

	/// Returns the enabled state control.
	AMPVControl* enabledStateControl() const { return enabledStateControl_; }
	/// Returns the invert state control.
	AMPVControl* invertedStateControl() const { return invertedStateControl_; }

signals:
	/// Notifier that the enabled state control value changed.
	void enabledStateChanged(double);
	/// Notifier that the invert state control value changed.
	void invertedStateChanged(double);

public slots:
	/// Sets the enabled state value.
	void setEnabled(bool isEnabled);
	/// Sets the invert state value.
	void setInverted(bool isInverted);

protected slots:
	/// Handles emitting the enabled state value changed signal.
	void onEnabledStateValueChanged();
	/// Handles emitting the inverted state value changed signal.
	void onInvertedStateValueChanged();

protected:
	/// The enabled state control.
	AMPVControl *enabledStateControl_;
	/// The inverted state control.
	AMPVControl *invertedStateControl_;
};

#endif // BIOXASZEBRALOGICBLOCKINPUT_H
