#ifndef SGMHEXAPODTRANSFORMEDAXIS_H
#define SGMHEXAPODTRANSFORMEDAXIS_H

#include "beamline/AM3DRotatedSystemControl.h"
/*!
  * A class which represents an interface with the Hexapod used to perform 'regular'
  * single axis motions across an arbitrarily rotated system. Owing to the trajectory
  * based nature of the hexapod PVs, this requires access to the trajectory setpoint
  * PVs, the actual feedback PVs, and the trajectory start PV.
  */
class SGMHexapodTransformedAxis : public AM3DRotatedSystemControl
{
    Q_OBJECT
public:
	/*!
	  * Creates an instance of a hexapod tranformed axis.
	  * \param axis ~ The designation of the axis this motor represents.
	  * \param globalXAxisSetpoint ~ The trajectory setpoint PV for the X Axis.
	  * \param globalXAxisFeedback ~ The actual feedback PV for the X Axis.
	  * \param globalYAxisSetpoint ~ The trajectory setpoint PV for the Y Axis.
	  * \param globalYAxisFeedback ~ The actual feedback PV for the Y Axis.
	  * \param globalZAxisSetpoint ~ The trajectory setpoint PV for the Z Axis.
	  * \param globalZAxisFeedback ~ The actual feedback PV for the Z Axis.
	  * \param trajectoryStartControl ~ The PV control which starts a trajectory
	  * motion of the hexapod.
	  * \param name ~ The name of this axis motor.
	  * \param units ~ The units of this axis motor.
	  * \param parent ~ The QObject parent.
	  * \param description ~ A human readable description of this motor.
	  */
	explicit SGMHexapodTransformedAxis(AxisDesignation axis,
									   AMControl* globalXAxisSetpoint,
									   AMControl* globalXAxisFeedback,
									   AMControl* globalYAxisSetpoint,
									   AMControl* globalYAxisFeedback,
									   AMControl* globalZAxisSetpoint,
									   AMControl* globalZAxisFeedback,
									   AMControl* trajectoryStartControl,
									   const QString &name,
									   const QString &units,
									   QObject *parent = 0,
									   const QString &description = "");

signals:

public slots:
protected slots:
	/*!
	  * Updates the connected state of this pseudo motor based on the connected
	  * states of the global axis child controls.
	  */
	void updateConnected();

	/*!
	  * Updates the feedback value of this pseudo motor by transforming the vector
	  * representing the feedback values of the global child controls into our
	  * transformed coordinate system.
	  */
	void updateValue();

	/*!
	  * Updates the moving state of this pseudo motor by checking the moving state
	  * of the global axis child controls and updating if their movements affected
	  * those of this pseudo motor.
	  */
	void updateMoving();

protected:
	/*!
	  * Creates a move action which will perform the trajectory move on the global
	  * system which corresponds with a motion of this motor in the rotated system
	  * to the provided setpoint.
	  */
	AMAction3* createMoveAction(double setpoint);

	AMControl* trajectoryStartControl_;
	AMControl* globalXAxisFeedback_;
	AMControl* globalYAxisFeedback_;
	AMControl* globalZAxisFeedback_;
};

#endif // SGMHEXAPODTRANSFORMEDAXIS_H
