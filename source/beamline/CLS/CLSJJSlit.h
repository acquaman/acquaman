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

class CLSJJSlit : public QObject
{
	Q_OBJECT

public:
	/// Constructor
	/*!
	  \param name A unique description of this motor
	  \param baseName The base of the PV name (if the motor status was "SMTR16114I1022:status" then the base is "SMTR16114I1022")
	  \param description A human readable description for this motor
	  \param parent QObject parent class
	  */
	virtual ~CLSJJSlit();
	CLSJJSlit(const QString &name, const QString &description, const QString &pvBaseName, double tolerance = AMCONTROL_TOLERANCE_DONT_CARE, double moveStartTimeoutSeconds = 2.0, QObject *parent = 0);

	/// check whether the JJ slit is connected or not
	bool isConnected();

	/// the current gap value
	double gap();
	/// the current center value
	double center();
	/// the current status
	double status();

signals:
	/// the signal to indicate whether the JJ slit is connected or not
	void connected(bool);

	/// the signal to indicate that the gap of the JJ slit changed
	void gapValueChanged(double);
	/// the signal to indicate that the center of the JJ slit changed
	void centerValueChanged(double);
	/// the signal to indicate that the status of the JJ slit changed
	void statusValueChanged(double);

public slots:
	/// to handle the connected signal of the PV controls (gap, center, and status)
	void onPVControlConnected(bool);

	/// to set the gap of the JJ slit
	void moveGap(double setpoint);
	/// to set the center of the JJ slit
	void moveCenter(double setpoint);

protected:
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

#endif // CLSJJSLIT_H
