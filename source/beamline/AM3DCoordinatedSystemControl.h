#ifndef AM3DCOORDINATEDSYSTEMCONTROL_H
#define AM3DCOORDINATEDSYSTEMCONTROL_H

#include "beamline/AMPseudoMotorControl.h"
#include <QVector3D>

/*!
  * \brief A motor which represents a single transformed axis relative to some global
  * coordinate system defined by controls for globalX, globalY and globalZ.
  * Movements across the axis which this pseudo motor represents will be translated
  * into a coordinated movement across one or more of the global axes. Conversely
  * movements across the global axes will be translated back in order to accurately
  * reflect the feedback and status of the arbitrary axis.
  *
  * These pseudo motors will often be used in sets of three, in order that 3
  * motors controlling some global system can be translated across some new transformed
  * system.
  * NOTE: In order for this to accurately work it is required that the three provided
  * motors work in the same units of measurement, and in largely the same fashion.
  * As such it is strongly recommended that the three motors be of the same type,
  * controlled by the same driver version (eg. three MaxVMotors).
  */
class AM3DCoordinatedSystemControl : public AMPseudoMotorControl
{
    Q_OBJECT
public:
	/*!
	  * Enumerates the different axes within the system that a motor can represent.
	  */
	enum AxisDesignation {
		XAxis,
		YAxis,
		ZAxis
	};

	/*!
	  * Creates a new 3D motion pseudo motor control which represents the provided
	  * transformed 'prime' axis, and will coordinate movement across the three
	  * provided motor controls.
	  * NOTE: All three controls are required. Refactoring (or a distinct class)
	  * will be required for transforms across 2D systems).
	  * \param axis ~ The axis of the transformed system that this motor represents.
	  * \param globalXAxis ~ A reference to the control which performs motions on
	  * the global X axis.
	  * \param globalYAxis ~ A reference to the control which performs motions on
	  * the global Y axis.
	  * \param globalZAxis ~ A reference to the control which performs motions on
	  * the global Z axis.
	  * \param name ~ The name by which this pseudo motor will be known.
	  * \param units ~ The units of measurement which will be displayed to users
	  * for this control.
	  * \param parent ~ The parent QObject in the QObject ownership hierarchy.
	  * \param description ~ A brief description of the control.
	  */
	AM3DCoordinatedSystemControl(AxisDesignation axis,
								 AMControl* globalXAxis,
								 AMControl* globalYAxis,
								 AMControl* globalZAxis,								 
								 const QString &name,
								 const QString &units,
								 QObject *parent = 0,
								 const QString &description = "");

	/*!
	  * Virtual destructor for an AM3DPseudoMotorControl.
	  */
	virtual ~AM3DCoordinatedSystemControl() {}

	/*!
	  * Whether this pseudo motor is expected to be able to return feedback.
	  */
	bool shouldMeasure() const;

	/*!
	  * Whether this pseudo motor is expected to be able to move.
	  */
	bool shouldMove() const;

	/*!
	  * Whether this pseudo motor is expected to be able to stop.
	  */
	bool shouldStop() const;

	/*!
	  * Whether this pseudo motor is able to return feedback at this moment.
	  */
	bool canMeasure() const;

	/*!
	  * Whether this pseudo motor is able to move at this moment.
	  */
	bool canMove() const;

	/*!
	  * Whether this pseudo motor is able to stop at this moment.
	  */
	bool canStop() const;

	/*!
	  * Adds the provided control to the list of child controls.
	  */
    virtual bool addChildControl(AMControl *control);

	/*!
	  * Virtual function which performs the calculation required to transform a
	  * vector movement from the global system to the arbitrary transformed one.
	  * \param globalVector ~ A vector in the global coordinate system which will
	  * be tranformed into the arbitrary transformed system.
	  */
	virtual QVector3D globalAxisToPrime(const QVector3D& globalVector) const { return globalVector; }

	/*!
	  * Virtual function which performs the calculation required to transform a
	  * vector movement from the arbitrary tranformed system to the global one.
	  * \param primeVector ~ A vector in the arbitrary coordinate system which
	  * will be transformed into the global system.
	  */
	virtual QVector3D primeAxisToGlobal(const QVector3D& primeVector) const { return primeVector; }
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

	/*!
	  * Updates the minimum position value possible for this pseudo motor by
	  * transforming the range of possible motions within the global axis to
	  * the arbitrary coordinate system.
	  */
	void updateMinimumValue();

	/*!
	  * Updates the maximum position value possible for this pseudo motor by
	  * transforming the range of possible motions within the global axis to
	  * the arbitrary coordinate system.
	  */
	void updateMaximumValue();

	/*!
	  * Updates the maximum and minimum position values possible for this pseudo
	  * motor by transforming the range of possible values for each within the
	  * global axes to the arbitrary coordinate system, then setting the minimum
	  * to the smaller of the two, and the maximum to the larger of the two.
	  */
	virtual void updateMaximumAndMinimumValues();

	/*!
	  * Updates the setpoint value of this pseudo motor by transforming the global
	  * set of setpoints into the arbitrary coordinate system.
	  */
	virtual void updateSetpoint();
protected:

	/*!
	  * Whether this pseudo motor, in its current transformed state, is affected
	  * my motions made in the global X Axis.
	  */
	bool affectedByMotionsInX() const;

	/*!
	  * Whether this pseudo motor, in its current transformed state, is affected
	  * by motions made in the global Y Axis.
	  */
	bool affectedByMotionsInY() const;

	/*!
	  * Whether this pseudo motor, in its current transformed state, is affected
	  * by motions made in the global Z Axis.
	  */
	bool affectedByMotionsInZ() const;

	/*!
	  * Creates a move action which will perform a parrallel move across the required
	  * global axis controls in order to move across the vector which will take
	  * the position from the current value to the new setpoint.
	  */
	AMAction3* createMoveAction(double setpoint);

	/*!
	  * Helper function which extracts the value from a vector which relates to
	  * the current axis designation of this pseudo motor.
	  * \param vector ~ The vector which a value is to be extracted from.
	  */
	double designatedAxisValue(const QVector3D& vector) const;

	AMControl* globalXAxis_;
	AMControl* globalYAxis_;
	AMControl* globalZAxis_;

	AxisDesignation axis_;
};

#endif // AM3DCOORDINATEDSYSTEMCONTROL_H
