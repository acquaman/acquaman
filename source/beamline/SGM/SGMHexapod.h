#ifndef SGMHEXAPOD_H
#define SGMHEXAPOD_H

#include <QObject>
#include "beamline/AMControlSet.h"
/*!
  * A class which represents the AMPIC887 Controlled hexapod within the SGM
  * endstation.
  */
class AMControl;
class SGMHexapod : public QObject
{
    Q_OBJECT
public:
	/*!
	  * Creates an instance of an SGMHexapod
	  */
    explicit SGMHexapod(QObject *parent = 0);

	/*!
	  * Virtual destructor for an SGMHexapod
	  */
	~SGMHexapod() {}	

	/*!
	  * The connected state of all the hexapod controls.
	  * \returns True if all hexapod controls are connected, false otherwise.
	  */
	bool isConnected();

	AMControl* xPrime() const;

	/*!
	  * The main x Axis Control.
	  */
	AMControl* xAxis() const;

	/*!
	  * The main y Axis Control.
	  */
	AMControl* yAxis() const;

	/*!
	  * The main z Axis Control.
	  */
	AMControl* zAxis() const;

	/*!
	  * The main u Axis Control.
	  */
	AMControl* uAxis() const;

	/*!
	  * The main v Axis Control.
	  */
	AMControl* vAxis() const;

	/*!
	  * The main w Axis Control.
	  */
	AMControl* wAxis() const;

	/*!
	  * The system velocity Control.
	  */
	AMControl* systemVelocity() const;

	/*!
	  * The stop all Control.
	  */
	AMControl* stopAll() const;

	/*!
	  * The x Axis trajectory setpoint control
	  */
	AMControl* xAxisTrajectorySetpoint() const;

	/*!
	  * The y Axis trajectory setpoint control
	  */
	AMControl* yAxisTrajectorySetpoint() const;

	/*!
	  * The z Axis trajectory setpoint control
	  */
	AMControl* zAxisTrajectorySetpoint() const;

	/*!
	  * The u Axis trajectory setpoint control
	  */
	AMControl* uAxisTrajectorySetpoint() const;

	/*!
	  * The v Axis trajectory setpoint control
	  */
	AMControl* vAxisTrajectorySetpoint() const;

	/*!
	  * The w Axis trajectory setpoint control
	  */
	AMControl* wAxisTrajectorySetpoint() const;

	/*!
	  * The trajectory move start control.
	  */
	AMControl* trajectoryStart() const;

	/*!
	  * The trajectory reset control.
	  */
	AMControl* trajectoryReset() const;
signals:

	/*!
	  * Signal which indicates that the connected state of the controls in the
	  * hexapod have been altered.
	  * \param connected ~ The connected state of the hexapod controls. True if
	  * all hexapod controls are connected, false otherwise.
	  */
	void connected(bool connected);
public slots:

protected:
	AMControl* xPrime_;

	AMControl* xAxis_;
	AMControl* yAxis_;
	AMControl* zAxis_;
	AMControl* uAxis_;
	AMControl* vAxis_;
	AMControl* wAxis_;

	AMControl* systemVelocity_;
	AMControl* stopAll_;

	AMControl* xAxisTrajectorySetpoint_;
	AMControl* yAxisTrajectorySetpoint_;
	AMControl* zAxisTrajectorySetpoint_;
	AMControl* uAxisTrajectorySetpoint_;
	AMControl* vAxisTrajectorySetpoint_;
	AMControl* wAxisTrajectorySetpoint_;
	AMControl* trajectoryStart_;
	AMControl* trajectoryReset_;

	AMControlSet* allControls_;

};

#endif // SGMHEXAPOD_H
