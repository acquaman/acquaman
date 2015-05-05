#ifndef CLSJJSLIT_H
#define CLSJJSLIT_H

#include "beamline/AMPVControl.h"

class CLSMAXvMotor;
class CLSJJSlitGapControl;
class CLSJJSlitCenterControl;

/// The class definition of CLSJJSlit, which contains a vertical CLSJJSlitBladesControl and a horizontal CLSJJSlitBladesControl
class CLSJJSlits : public QObject
{
	Q_OBJECT

public:
	/// Constructor
	/*!
	  \param name A unique description of this motor
	  \param verticalBladesPVBaseName The base of the PV name of the vertical blades (if the motor status was "SMTR16114I1022:status" then the base is "SMTR16114I1022")
	  \param horizontalBladesPVBaseName The base of the PV name o the horizontal blades (if the motor status was "SMTR16114I1022:status" then the base is "SMTR16114I1022")
	  \param description A human readable description for this JJ slit
	  \param parent QObject parent class
	  */
	explicit CLSJJSlits(const QString &name, const QString &upperBladePVName, const QString &lowerBladePVName, const QString &inboardBladePVName, const QString &outboardBladePVName, QObject*parent = 0);
	/// Destructor
	virtual ~CLSJJSlits();
	/// Returns the name.
	const QString name() const { return name_; }
	/// Returns the current connected state.
	bool isConnected() const { return connected_; }

	/// Returns the upper blade motor control.
	CLSMAXvMotor* upperBladeControl() const { return upperBlade_; }
	/// Returns the lower blade motor control.
	CLSMAXvMotor* lowerBladeControl() const { return lowerBlade_; }
	/// Returns the inboard blade motor control.
	CLSMAXvMotor* inboardBladeControl() const { return inboardBlade_; }
	/// Returns the outboard blade motor control.
	CLSMAXvMotor* outboardBladeControl() const { return outboardBlade_; }

	/// Returns the vertical blades gap control.
	CLSJJSlitGapControl* verticalGapControl() const { return verticalGap_; }
	/// Returns the vertical blades center control.
	CLSJJSlitCenterControl* verticalCenterControl() const { return verticalCenter_; }
	/// Returns the horizontal blades gap control.
	CLSJJSlitGapControl* horizontalGapControl() const { return horizontalGap_; }
	/// Returns the horizontal blades center control.
	CLSJJSlitCenterControl* horizontalCenterControl() const { return horizontalCenter_; }

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
	CLSMAXvMotor *upperBlade_;
	/// Lower blade motor control.
	CLSMAXvMotor *lowerBlade_;
	/// Inboard blade motor control.
	CLSMAXvMotor *inboardBlade_;
	/// Outboard blade motor control.
	CLSMAXvMotor *outboardBlade_;

	/// Vertical blades gap control.
	CLSJJSlitGapControl *verticalGap_;
	/// Vertical blades center control.
	CLSJJSlitCenterControl *verticalCenter_;
	/// Horizontal blades gap control.
	CLSJJSlitGapControl *horizontalGap_;
	/// Horizontal blades center control.
	CLSJJSlitCenterControl *horizontalCenter_;

};

#endif // CLSJJSLIT_H
