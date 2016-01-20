#ifndef BIOXASZEBRAINPUTCONTROL_H
#define BIOXASZEBRAINPUTCONTROL_H

#include <QObject>

#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"

class BioXASZebraInputControl : public QObject
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASZebraInputControl(const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraInputControl();

	/// Returns the name of the pulse control.
	QString name() const { return name_; }
	/// Returns the connected status.
	bool isConnected() const { return connected_; }

	/// Returns the current input value.
	int value() const;
	/// Returns the current input value as a string.
	QString valueString() const;
	/// Returns the input status value.
	bool status() const;

	/// Returns the input value control.
	AMPVControl* valueControl() const { return valueControl_; }
	/// Returns the input status control.
	AMReadOnlyPVControl* statusControl() const { return statusControl_; }

signals:
	/// Notifier that the connected state has changed.
	void connectedChanged(bool);
	/// Notifier that the input value has changed.
	void valueChanged(int);
	/// Notifier that the input value has changed, as a string.
	void valueStringChanged(const QString &);
	/// Notifier that the input status has changed.
	void statusChanged(bool);

public slots:
	/// Sets the input value.
	void setValue(int value);

protected slots:
	/// On control set bool changed.
	void onControlSetConnectedChanged(bool connected);
	/// Handles emitting the input value changed signal.
	void onValueChanged();
	/// Handles emitting the input status value changed signal.
	void onStatusChanged();

protected:
	/// The input control name.
	QString name_;
	/// The connected state.
	bool connected_;

	/// The set of subcontrols.
	AMControlSet *allControls_;
	/// The input value control.
	AMPVControl *valueControl_;
	/// The input status control.
	AMReadOnlyPVControl *statusControl_;
};

#endif // BIOXASZEBRAINPUTCONTROL_H
