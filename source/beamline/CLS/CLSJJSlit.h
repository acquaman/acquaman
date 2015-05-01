#ifndef CLSJJSLIT_H
#define CLSJJSLIT_H

#include "beamline/AMPVControl.h"


/// The definitions of CLSJJSlitBladesControl, which contains a gapPVControl, a centerPVControl and a statusPVControl
class CLSJJSlitBladesControl : public QObject
{
	Q_OBJECT

public:
	enum Direction {
		Vertical = 0,
		Horizontal = 1
	};

	/// Destructor
	virtual ~CLSJJSlitBladesControl ();
	/// Constructor
	/*!
	  \param name A unique description of this motor
	  \param baseName The base of the PV name (if the motor status was "SMTR16114I1022:status" then the base is "SMTR16114I1022")
	  \param description A human readable description for this motor
	  \param parent QObject parent class
	  */
	CLSJJSlitBladesControl(const QString &name, const QString &description, const Direction bladeDirection, const QString &pvBaseName, double tolerance = AMCONTROL_TOLERANCE_DONT_CARE, double moveStartTimeoutSeconds = 2.0, QObject *parent = 0);

	/// check whether the JJ slit is connected or not
	bool isConnected() const;

	/// return the gap pv control
	AMControl *gapPVControl() const { return gapPVControl_; }
	/// return the center PV control
	AMControl *centerPVControl() const { return centerPVControl_; }
	/// return the status PV control
	AMControl *statusPVControl() const { return statusPVControl_; }

	/// return the current gap value
	double gap() const { return gapPVControl_->value(); }
	/// return the current center value
	double center() const { return centerPVControl_->value(); }
	/// return the current status
	double status()  const { return statusPVControl_->value(); }
	/// return the current status string
	QString statusString()  const { return statusPVControl_->enumNameAt(statusPVControl_->value()); }

signals:
	/// the signal to indicate whether the JJ slit is connected or not
	void connected(bool);

	/// the signal to indicate that the gap of the JJ slit changed
	void gapValueChanged(double);
	/// the signal to indicate that the center of the JJ slit changed
	void centerValueChanged(double);
	/// the signal to indicate that the status of the JJ slit changed
	void statusStringChanged(QString);

public slots:
	/// to set the gap of the JJ slit
	void moveGap(double setpoint);
	/// to set the center of the JJ slit
	void moveCenter(double setpoint);

protected slots:
	/// to handle the connected signal of the PV controls (gap, center, and status)
	void onPVControlConnected();
	/// to handle the signale of the status PV value changed
	void onStatusValueChanged();

protected:
	/// the direction of the blade
	Direction direction_;

	/// the name of the JJ slit
	QString name_;
	/// a brief description of the JJ slit
	QString description_;

	/// flag of connection
	bool wasConnected_;

	/// PV Controll for gap PV
	AMControl *gapPVControl_;
	/// PV Controll for center PV
	AMControl *centerPVControl_;
	/// PV Control for status PV
	AMControl *statusPVControl_;
};

class CLSMAXvMotor;
class CLSJJSlitGapControl;
class CLSJJSlitCenterControl;

/// The class definition of CLSJJSlit, which contains a vertical CLSJJSlitBladesControl and a horizontal CLSJJSlitBladesControl
class CLSJJSlit : public QObject
{
	Q_OBJECT

public:
	/// Destructor
	virtual ~CLSJJSlit();
	/// Constructor
	/*!
	  \param name A unique description of this motor
	  \param verticalBladesPVBaseName The base of the PV name of the vertical blades (if the motor status was "SMTR16114I1022:status" then the base is "SMTR16114I1022")
	  \param horizontalBladesPVBaseName The base of the PV name o the horizontal blades (if the motor status was "SMTR16114I1022:status" then the base is "SMTR16114I1022")
	  \param description A human readable description for this JJ slit
	  \param parent QObject parent class
	  */
	explicit CLSJJSlit(const QString &name, const QString &description, const QString &upperBladePVName, const QString &lowerBladePVName, const QString &inboardBladePVName, const QString &outboardBladePVName, double tolerance = AMCONTROL_TOLERANCE_DONT_CARE, double moveStartTimeoutSeconds = 2.0, double limit = 11.0, QObject *parent = 0);

	/// check whether the JJ slit is connected or not
	bool isConnected() const;
	/// return the limit of the slit
	double limit() const { return limit_; }

	/// Returns the upper blade motor control.
	CLSMAXvMotor* upperBladeControl() const { return upperBlade_; }
	/// Returns the lower blade motor control.
	CLSMAXvMotor* lowerBladeControl() const { return lowerBlade_; }
	/// Returns the inboard blade motor control.
	CLSMAXvMotor* inboardBladeControl() const { return inboardBlade_; }
	/// Returns the outboard blade motor control.
	CLSMAXvMotor* outboardBladeControl() const { return outboardBlade_; }

//	/// return the vertical blade control
//	CLSJJSlitBladesControl * verticalBladesControl() const { return verticalBladesControl_; }
//	/// return the horizontal blade control
//	CLSJJSlitBladesControl * horizontalBladesControl() const { return horizontalBladesControl_; }

	/// Returns the vertical blades gap control.
	CLSJJSlitGapControl* verticalGapControl() const { return verticalGap_; }
	/// Returns the vertical blades center control.
	CLSJJSlitCenterControl* verticalCenterControl() const { return verticalCenter_; }
	/// Returns the horizontal blades gap control.
	CLSJJSlitGapControl* horizontalGapControl() const { return horizontalGap_; }
	/// Returns the horizontal blades center control.
	CLSJJSlitCenterControl* horizontalCenterControl() const { return horizontalCenter_; }

signals:
	/// the signal to indicate whether the JJ slit is connected or not
	void connected(bool);

	/// the signal to indicate that the vertical gap of the JJ slit changed
	void verticalGapValueChanged(double);
	/// the signal to indicate that the vertical center of the JJ slit changed
	void verticalCenterValueChanged(double);
	/// the signal to indicate that the status of the vetical motors of JJ slit changed
	void verticalStatusStringChanged(QString);

	/// the signal to indicate that the horizontal gap of the JJ slit changed
	void horizontalGapValueChanged(double);
	/// the signal to indicate that the horizontal center of the JJ slit changed
	void horizontalCenterValueChanged(double);
	/// the signal to indicate that the status of the vetical motors of JJ slit changed
	void horizontalStatusStringChanged(QString);

public slots:
	/// to handle the connected signal of the CLSJJSlitBladeControl
	void onBladesControlConnected(bool);

	/// Handles updating controls when the vertical gap changes.
	void onVerticalGapChanged();
	/// Handles updating controls when the vertical center changes.
	void onVerticalCenterChanged();

//	/// to set the Vertical gap of the JJ slit
//	void moveVerticalGap(double setpoint);
//	/// to set the Vertical center of the JJ slit
//	void moveVerticalCenter(double setpoint);

//	/// to set the Horizontal gap of the JJ slit
//	void moveHorizontalGap(double setpoint);
//	/// to set the Horizontal center of the JJ slit
//	void moveHorizontalCenter(double setpoint);

protected:
	/// the name of the JJ slit
	QString name_;
	/// a brief description of the JJ slit
	QString description_;
	/// the limit of the slit
	double limit_;

	/// flag of connection
	bool wasConnected_;

//	/// vertical blades
//	CLSJJSlitBladesControl * verticalBladesControl_;
//	/// horizotnal blades
//	CLSJJSlitBladesControl * horizontalBladesControl_;

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
