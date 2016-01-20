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
	explicit BioXASZebraLogicBlockInput(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraLogicBlockInput();

	/// Returns the current enabled status value.
	bool enabledStatus() const;
	/// Returns the current inverted status value.
	bool invertedStatus() const;

	/// Returns the enabled status control.
	AMPVControl* enabledStatusControl() const { return enabledStatusControl_; }
	/// Returns the invert status control.
	AMPVControl* invertedStatusControl() const { return invertedStatusControl_; }

signals:
	/// Notifier that the enabled status control value changed.
	void enabledStatusChanged(bool);
	/// Notifier that the invert status control value changed.
	void invertedStatusChanged(bool);

public slots:
	/// Sets the enabled status value.
	void setEnabled(bool isEnabled);
	/// Sets the invert status value.
	void setInverted(bool isInverted);

protected slots:
	/// Handles emitting the enabled status value changed signal.
	void onEnabledStatusValueChanged();
	/// Handles emitting the inverted status value changed signal.
	void onInvertedStatusValueChanged();

protected:
	/// The enabled status control.
	AMPVControl *enabledStatusControl_;
	/// The inverted status control.
	AMPVControl *invertedStatusControl_;
};

#endif // BIOXASZEBRALOGICBLOCKINPUT_H
