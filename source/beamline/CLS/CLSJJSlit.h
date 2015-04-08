#ifndef CLSJJSLIT_H
#define CLSJJSLIT_H

#include "beamline/AMPVControl.h"

/// This function object provides the moving check for the CLSMAXvMotors
class CLSJJSlitControlStatusChecker : public AMAbstractControlStatusChecker {
public:
	/// Status values will be compare to \c isStoppedValue, and return true if the status value is not equal to isStoppedValue (something that isn't stopped is moving)
	virtual ~CLSJJSlitControlStatusChecker() {}
	CLSJJSlitControlStatusChecker() {}

	/// Return true (moving) if the \c statusValue is not 0 (STOPPED) and is not 2 (AT LIMIT) and is not 3 (FORCED STOP) and is not 4 (ERROR)
	virtual bool operator()(quint32 statusValue) { return (statusValue != 0) && (statusValue != 2) && (statusValue != 3) && (statusValue != 4); }
};


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
	CLSJJSlit(const QString &name, const QString &description, const QString &verticalBladesPVBaseName, const QString &horizontalBladesPVBaseName, double tolerance = AMCONTROL_TOLERANCE_DONT_CARE, double moveStartTimeoutSeconds = 2.0, QObject *parent = 0);

	/// check whether the JJ slit is connected or not
	bool isConnected() const;

	/// return the vertical blade control
	CLSJJSlitBladesControl * verticalBladesControl() const { return verticalBladesControl_; }
	/// return the horizontal blade control
	CLSJJSlitBladesControl * horizontalBladesControl() const { return horizontalBladesControl_; }

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

	/// to set the Vertical gap of the JJ slit
	void moveVerticalGap(double setpoint);
	/// to set the Vertical center of the JJ slit
	void moveVerticalCenter(double setpoint);

	/// to set the Horizontal gap of the JJ slit
	void moveHorizontalGap(double setpoint);
	/// to set the Horizontal center of the JJ slit
	void moveHorizontalCenter(double setpoint);

protected:
	/// the name of the JJ slit
	QString name_;
	/// a brief description of the JJ slit
	QString description_;

	/// flag of connection
	bool wasConnected_;

	/// vertical blades
	CLSJJSlitBladesControl * verticalBladesControl_;
	/// horizotnal blades
	CLSJJSlitBladesControl * horizontalBladesControl_;

};

#endif // CLSJJSLIT_H
