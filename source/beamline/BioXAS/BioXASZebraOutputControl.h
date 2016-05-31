#ifndef BIOXASZEBRAOUTPUTCONTROL_H
#define BIOXASZEBRAOUTPUTCONTROL_H

#include <QObject>

#include "beamline/AMControl.h"

class AMPVControl;
class AMReadOnlyPVControl;
class AMControlSet;

class BioXASZebraOutputControl : public AMControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASZebraOutputControl(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraOutputControl();

	/// Returns the name of the output control.
	QString name() const { return name_; }
	/// Returns the connected state.
	virtual bool isConnected() const;

	/// Returns the output value.
	int outputValue() const;
	/// Returns the output value string.
	QString outputValueString() const;
	/// Returns the output status value.
	double outputStatusValue() const;
	/// Returns true if the output status value is high, false otherwise.
	bool isOutputStateHigh() const;

	/// Returns the output value control.
	AMPVControl* outputValueControl() const { return outputValueControl_; }
	/// Returns the output status control.
	AMReadOnlyPVControl* outputStatusControl() const { return outputStatusControl_; }

signals:
	/// Notifier that the connected state has changed.
	void connectedChanged(bool);
	/// Notifier that the output value has changed.
	void outputValueChanged(int);
	/// Notifier that the output value string has changed.
	void outputValueStringChanged(const QString &);
	/// Notifier that the output status has changed.
	void outputStatusChanged(double);

	/// Notifier that the output value preference has changed.
	void outputValuePreferenceChanged(int);

public slots:
	/// Sets the output value.
	void setOutputValue(int newValue);
	/// Sets the output value preference.
	void setOutputValuePreference(int newValue);

protected slots:
	/// Handles emitting the appropriate signals when the connected state has changed.
	void onControlSetConnectedChanged();
	/// Handles emitting the appropriate signals when the output value has changed.
	void onOutputValueChanged();
	/// Handles emitting the appropriate signals when the output status has changed.
	void onOutputStatusChanged();

	/// Updates the output value control with the output value preference.
	void updateOutputValueControl();

protected:
	/// The name of the output control.
	QString name_;

	/// The set of all sub controls.
	AMControlSet *allControls_;
	/// The output value control.
	AMPVControl *outputValueControl_;
	/// The output status control.
	AMReadOnlyPVControl* outputStatusControl_;

	/// The flag indicating whether an output value preference has been set.
	bool outputValuePreferenceSet_;
	/// The output value preference.
	int outputValuePreference_;
};

#endif // BIOXASZEBRAOUTPUTCONTROL_H
