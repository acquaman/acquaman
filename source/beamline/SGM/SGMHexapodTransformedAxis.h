#ifndef SGMHEXAPODTRANSFORMEDAXIS_H
#define SGMHEXAPODTRANSFORMEDAXIS_H

#include "beamline/AM3DRotatedSystemPseudoMotorControl.h"
class SGMHexapodTransformedAxis : public AM3DRotatedSystemPseudoMotorControl
{
    Q_OBJECT
public:
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
	AMAction3* createMoveAction(double setpoint);

	AMControl* trajectoryStartControl_;
	AMControl* globalXAxisFeedback_;
	AMControl* globalYAxisFeedback_;
	AMControl* globalZAxisFeedback_;
};

#endif // SGMHEXAPODTRANSFORMEDAXIS_H
