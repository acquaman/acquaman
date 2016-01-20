#ifndef BIOXASZEBRAANDCONTROL_H
#define BIOXASZEBRAANDCONTROL_H

#include <QObject>

#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"

#include "beamline/BioXAS/BioXASZebraAndInputControl.h"

#define BIOXASZEBRAANDCONTROL_INPUT_CONTROL_NUM 4

class BioXASZebraAndControl : public QObject
{
    Q_OBJECT

public:
	/// Enumeration of the possible output status values.
	enum OutputStatus { Off = 0, On = 1 };

	/// Constructor.
	explicit BioXASZebraAndControl(const QString &baseName, int blockIndex, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraAndControl();

	/// Returns the name of the pulse control.
	QString name() const { return name_; }
	/// Returns the connected status of the pulse control.
	bool isConnected() const { return connected_; }

	/// Returns the output status.
	bool outputStatus() const;

	/// Returns the list of input controls.
	QList<BioXASZebraAndInputControl*> inputControls() const { return inputControls_; }
	/// Returns the output status control.
	AMReadOnlyPVControl* outputStatusControl() const { return outputStatusControl_; }

signals:
	/// Notifier that the pulse control connectivity has changed.
	void connectedChanged(bool);
	/// Notifier that the output value status has changed.
	void outputStatusChanged(bool);

protected slots:
	/// On control set bool changed.
	void onControlSetConnectedChanged(bool connected);
	/// Handles emitting the output status value changed signal.
	void onOutputStatusValueChanged();

protected:
	/// Name of the zebra pulse control.
	QString name_;
	/// Flag for previous connectivity state.
	bool connected_;

	/// The control set for this pulse control.
	AMControlSet *allControls_;
	/// The list of input controls.
	QList<BioXASZebraAndInputControl*> inputControls_;
	/// The output status control.
	AMReadOnlyPVControl *outputStatusControl_;
};

#endif // BIOXASZEBRAANDCONTROL_H
