#ifndef CLSJJSLITS_H
#define CLSJJSLITS_H

#include <QObject>

#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSJJSlitGapControl.h"
#include "beamline/CLS/CLSJJSlitCenterControl.h"

/// The class definition of CLSJJSlit, which contains a vertical CLSJJSlitsBladesControl and a horizontal CLSJJSlitsBladesControl
class CLSJJSlits : public QObject
{
	Q_OBJECT

public:
	/// Constructor
	explicit CLSJJSlits(const QString &name, const QString &upperBladePVName, const QString &lowerBladePVName, const QString &inboardBladePVName, const QString &outboardBladePVName, QObject*parent = 0);
	/// Destructor
	virtual ~CLSJJSlits();
	/// Returns the name.
	const QString name() const { return name_; }
	/// Returns the current connected state.
	bool isConnected() const { return connected_; }

	/// Returns the upper blade motor control.
	AMControl* upperBladeControl() const { return upperBlade_; }
	/// Returns the lower blade motor control.
	AMControl* lowerBladeControl() const { return lowerBlade_; }
	/// Returns the inboard blade motor control.
	AMControl* inboardBladeControl() const { return inboardBlade_; }
	/// Returns the outboard blade motor control.
	AMControl* outboardBladeControl() const { return outboardBlade_; }

	/// Returns the vertical blades gap control.
	AMControl* verticalGapControl() const { return verticalGap_; }
	/// Returns the vertical blades center control.
	AMControl* verticalCenterControl() const { return verticalCenter_; }
	/// Returns the horizontal blades gap control.
	AMControl* horizontalGapControl() const { return horizontalGap_; }
	/// Returns the horizontal blades center control.
	AMControl* horizontalCenterControl() const { return horizontalCenter_; }

signals:
	/// Notifier that the connected state has changed.
	void connectedChanged(bool isConnected);

protected slots:
	/// Sets the current connected state.
	void setConnected(bool isConnected);

	/// Handles updating the current connected state.
	void updateConnected();

protected:
	/// The name.
	QString name_;
	/// Current connected state.
	bool connected_;

	/// Upper blade motor control.
	AMPVwStatusControl *upperBlade_;
	/// Lower blade motor control.
	AMPVwStatusControl *lowerBlade_;
	/// Inboard blade motor control.
	AMPVwStatusControl *inboardBlade_;
	/// Outboard blade motor control.
	AMPVwStatusControl *outboardBlade_;

	/// Vertical blades gap control.
	CLSJJSlitGapControl *verticalGap_;
	/// Vertical blades center control.
	CLSJJSlitCenterControl *verticalCenter_;
	/// Horizontal blades gap control.
	CLSJJSlitGapControl *horizontalGap_;
	/// Horizontal blades center control.
	CLSJJSlitCenterControl *horizontalCenter_;

};

#endif // CLSJJSLITS_H
