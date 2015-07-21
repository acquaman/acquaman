/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMMOTORGROUP_H
#define AMMOTORGROUP_H

#include <QObject>
#include <QMap>

#include "beamline/AMControl.h"
#include "actions3/AMAction3.h"
/*!
  * A class which represents a specific motion within a motor group.
  */
class AMMotorGroupMotion : public QObject
{
	Q_OBJECT
public:
	/*!
	  * Creates an instance of a motor group motion with the provided human readable
	  * name, which is performed by the provided motor.
	  * \param name ~ A human readable name which describes the motion performed.
	  * \param motor ~ The motor which will be performing the motions.
	  * NOTE: The class takes NO ownership of the passed motor control.
	  */
	AMMotorGroupMotion(const QString& name,
					   AMControl* motor);

	/*!
	  * Virtual destructor for a motor group motion.
	  */
	virtual ~AMMotorGroupMotion() {}

	/*!
	  * A human readable name which describes this type of motion ("X", "Pitch" etc.)
	  */
	QString name() const;

	/*!
	  * A helper method for obtaining the units associated with the contained
	  * motor. If no motor is associated with this motion, the empty string will
	  * be returned.
	  */
	QString positionUnits() const;

	/*!
	  * The motor which will perform the motions.
	  */
	AMControl* motor() const;


protected:
	QString name_;
	AMControl* motor_;
};

/*!
  * \brief A class representing a single axis of a motor group. Each axis can have
  * a translational movement and/or a rotational movement.
  * To check if a given axis can perform a type of motion use the canTranslate()
  * and canRotate() functions.
  */
class AMMotorGroupAxis : public QObject
{
	Q_OBJECT
public:
	/*!
	  * Creates an instance of a motor group axis with the provided translational
	  * and or rotational motions.
	  * \param translationMotion ~ The motions for translations along this axis.
	  * If 0 is provided it is assumed that this axis cannot translate.
	  * \param rotationalMotion ~ The motions for rotations along this axis. If
	  * 0 is provided it is assumed that this axis cannot rotate.
	  */
	AMMotorGroupAxis(AMMotorGroupMotion* translationalMotion,
					 AMMotorGroupMotion* rotationalMotion);

	/*!
	  * Virtual destructor for a motor group axis.
	  */
	virtual ~AMMotorGroupAxis() {}

	/*!
	  * The given human readable name for translations of this axis.
	  */
	QString translationName() const;

	/*!
	  * The given human readable name for rotations of this axis.
	  */
	QString rotationName() const;

	/*!
	  * The current translational position of this axis. If the axis cannot
	  * translate, a value of 0 is returned.
	  */
	double currentTranslationPosition() const;

	/*!
	  * The current rotational position of this axis. If the axis cannot rotate,
	  * a value of 0 is returned.
	  */
	double currentRotationPosition() const;

	/*!
	  * The units in which the translations of the axis are measured.
	  */
	QString translationPositionUnits() const;

	/*!
	  * The units in which the rotations of the axis are measured.
	  */
	QString rotationPositionUnits() const;

	/*!
	  * Whether this axis is currently moving (translating or rotating).
	  */
	bool isMoving() const;

	/*!
	  * Whether this axis is currently translating.
	  */
	bool isTranslating() const;

	/*!
	  * Whether this axis is currently rotating.
	  */
	bool isRotating() const;

	/*!
	  * Whether this axis can make a translational movement. Checks to ensure that
	  * a motion is associated with translational movements for this axis, that
	  * a motor is associated with that motion, and that the motor is connected.
	  */
	bool canTranslate() const;

	/*!
	  * Whether this axis can make a rotational movement. Checks to ensure that
	  * a motion is associated with rotational movements for this axis, that
	  * a motor is associated with that motion, and that the motor is connected.
	  */
	bool canRotate() const;

	/*!
	  * \brief Creates an action to permform a translational movement on this axis
	  * to the provided position.
	  * If no translation control exists, 0 will be returned. See canTranslate().
	  * \param position ~ The position to which the axis will translate.
	  */
	AMAction3* createTranslateMoveAction(double position);

	/*!
	  * \brief Creates an action to permform a rotational movement on this axis
	  * to the provided position.
	  * If no rotation control exists, 0 will be returned. See canRotate().
	  * \param position ~ The position to which the axis will rotate.
	  */
	AMAction3* createRotateMoveAction(double position);

	/*!
	  * \brief Creates an action which will stop translational movements on this
	  * axis.
	  * If no translation control exists, 0 will be returned. See canTranslate().
	  */
	AMAction3* createStopTranslateAction();

	/*!
	  * \brief Creates an action which will stop rotational movements on this
	  * axis.
	  * If no rotation control exists, 0 will be returned. See canRotate().
	  */
	AMAction3* createStopRotateAction();

signals:
	/*!
	  * Signal indicating that the connected state of the translational motor
	  * has been altered.
	  * \param isConnected ~ The new connected state of the translational motor.
	  */
	void translationConnectedStateChanged(bool isConnected);

	/*!
	  * Signal indicating that the display precision of the translational motor
	  * has been altered.
	  * \param displayPrevcision ~ The new display precision of the translational
	  * motor.
	  */
	void translationDisplayPrecisionChanged(int);

	/*!
	  * Signal indicating that an error has occured with the translational motor.
	  * \param errorMessage ~ The error message given by the translational motor.
	  */
	void translationError(const QString&);

	/*!
	  * Signal indicating that an attempted translational move has failed.
	  * \param explinationCode ~ A code which gives the type of failure.
	  */
	void translationMoveFailed(int explinationCode);

	/*!
	  * Signal indicating that the moving state of the translational motor has been
	  * altered. I.e. the translational motor has gone from moving to not moving,
	  * or vice versa.
	  * \param isMoving ~ The new moving state of the translational motor.
	  */
	void translationMovingStateChanged(bool isMoving);

	/*!
	  * Signal indicating that the position units for the translational motor
	  * has been altered.
	  * \param units ~ The new position units.
	  */
	void translationUnitsChanged(const QString& units);

	/*!
	  * Signal indicating that the connected state of the rotational motor
	  * has been altered.
	  * \param isConnected ~ The new connected state of the rotational motor.
	  */
	void rotationConnectedStateChanged(bool isConnected);

	/*!
	  * Signal indicating that the display precision of the rotational motor
	  * has been altered.
	  * \param displayPrevcision ~ The new display precision of the rotational
	  * motor.
	  */
	void rotationDisplayPrecisionChanged(int);

	/*!
	  * Signal indicating that an error has occured with the rotational motor.
	  * \param errorMessage ~ The error message given by the rotational motor.
	  */
	void rotationError(const QString&);

	/*!
	  * Signal indicating that an attempted rotational move has failed.
	  * \param explinationCode ~ A code which gives the type of failure.
	  */
	void rotationMoveFailed(int explinationCode);

	/*!
	  * Signal indicating that the moving state of the rotational motor has been
	  * altered. I.e. the rotational motor has gone from movint to not moving,
	  * or vice versa.
	  * \param isMoving ~ The new moving state of the rotational motor.
	  */
	void rotationMovingStateChanged(bool isMoving);

	/*!
	  * Signal indicating that the position units for the rotational motor
	  * has been altered.
	  * \param units ~ The new position units.
	  */
	void rotationUnitsChanged(const QString& units);

protected slots:
protected:

	AMMotorGroupMotion* translationalMotion_;
	AMMotorGroupMotion* rotationalMotion_;
};

/*!
  * \brief A class which represents a set of motor group axes which are logically
  * grouped to move a single piece of equipment (eg. a sample stage).
  *
  * The group object is organized in such a way that it will cope with anything
  * from 1 - 3 axes, each of which can perform a translational and/or rotational
  * movements.
  */
class AMMotorGroupObject : public QObject
{
	Q_OBJECT

public:

	/*!
	  * Enumerates all the possible different directions of motions which a motor
	  * group object can move in.
	  */
	enum MotionDirection {
		HorizontalMotion,
		NormalMotion,
		VerticalMotion
	};

	/*!
	  * Creates an instance of an AMMotorGroupObject with the provided name, and
	  * no associated axes. Axes can be added with the setAxis() function.
	  * \param name ~ A human readable name which can be used to logically identify
	  * the contained axes (eg. sample stage).
	  */
	AMMotorGroupObject(const QString& name);

	/*!
	 * Creates an instance of an AMMotorGroupObject with the provided name, and
	 * associated map of axes. The created group object will take ownership of
	 * the motor group axes provided.
	 * \param name ~ A human readable name which can be used to logically identify
	 * the contained axes (eg. sample stage).
	 * \param axes ~ A collection of axes which this object will logically group.
	 * The group object will take ownership of the provided motor group axes.
	 */
	AMMotorGroupObject(const QString& name, const QMap<MotionDirection, AMMotorGroupAxis*>& axes);

	/*!
	  * Virtual destructor for the AMMotorGroupObject
	  */
	virtual ~AMMotorGroupObject() {}

	/*!
	  * Sets the associated AMMotorGroupAxis with the provided motion direction.
	  * If an association already exists for this direction it will be over
	  * written, and the resources associated with the old group axis freed.
	  * If a null value is provided then the group object will no longer have an
	  * association with the motion direction.
	  * \param motionDirection ~ The direction of motion for the axis.
	  * \param axisDetails ~ The motor group axis object to associate with the
	  * direction of motion. The group object takes ownership of the provided motor
	  * group axis.
	  */
	void setDirectionAxis(MotionDirection motionDirection, AMMotorGroupAxis* axisDetails);

	/*!
	  * Associates an axis contructed from the provided details with the given
	  * motion direction. If an association already exists for the provided
	  * motion direction, it will be over written, and the resouces of the associated
	  * axis will be freed.
	  * \param motionDirection ~ The direction of motion along which the axis moves.
	  * \param translationName ~ A human readable name to associated with translations
	  * of this axis (eg. X, Y, In, Out, Up etc.)
	  * \param translationalMotor ~ The motor which will perform translations on
	  * this axis. If 0 is provided it is assumed that this axis cannot translate.
	  * \param rotationalName ~ A human readable name to associate with rotations
	  * of this axis (eg. pitch, theta, widdershins etc.)
	  * \param rotationalMotor ~ The motor which will perform rotations on this
	  * axis. If 0 is provided it is assumed that this axis cannot rotate.
	  */
	void setDirectionAxis(MotionDirection motionDirection,
				 const QString& translationalName,
				 AMControl* translationalMotor,
				 const QString& rotationalName,
				 AMControl* rotationalMotor);

	/*!
	  * \brief The number of axes contained within this group object.
	  * This does not necessarily represent the dimensionality of the movements
	  * of this axis group (as each axis can have a translational and rotational
	  * movement). To obtain the dimensionality of the group object use dimensionality();
	  */
	int axisCount() const;

	/*!
	  * The freedom of movement available to the group object. Can be any number
	  * from 0 - 6, with 6 representing translational and rotational movement
	  * across all 6 axes.
	  */
	int dimensionality() const;

	/*!
	  * The name of the group object.
	  */
	QString name() const;

	/*!
	  * \brief The motor group axis associated with the provided direction of
	  * motion.
	  * If no axis is associated with the provided direction, 0 will be returned.
	  * See hasAxis().
	  * \param motionDirection ~ The direction of motions whose associated axis
	  * is to be returned.
	  */
	AMMotorGroupAxis* axis(MotionDirection motionDirection) const;

	/*!
	  * The motor group axis associated with horizontal motions. If the group does
	  * not contain a horizontal axis, 0 will be returned.
	  * See hasHorizontalAxis().
	  */
	AMMotorGroupAxis* horizontalAxis() const;

	/*!
	  * The motor group axis associated with normal motions. If the group does
	  * not contain a normal axis, 0 will be returned.
	  * See hasNormalAxis().
	  */
	AMMotorGroupAxis* normalAxis() const;

	/*!
	  * The motor group axis associated with vertical motions. If the group does
	  * not contain a vertical axis, 0 will be returned.
	  * See hasVerticalAxis().
	  */
	AMMotorGroupAxis* verticalAxis() const;

	/*!
	  * Whether the provided direction of motion has an associated axis.
	  * \param motionDirection ~ The direction of motion which will be checked to
	  * see if there is an associated axis.
	  */
	bool hasAxis(MotionDirection motionDirection) const;

	/*!
	  * Whether the group object contains an axis for the horizontal direction.
	  */
	bool hasHorizontalAxis() const;

	/*!
	  * Whether the group object contains an axis for movements along the normal
	  * direction.
	  */
	bool hasNormalAxis() const;

	/*!
	  * Whether the group object contains an axis for movements along the vertical
	  * direction.
	  */
	bool hasVerticalAxis() const;

	/*!
	  * Whether any axis within the group object is performing a translational
	  * or rotational movement.
	  */
	bool isMoving() const;

	/*!
	  * Creates an action which will stop all translational and rotational movement
	  * across all associated axes within the group object.
	  */
	AMAction3* createStopAllAction();

protected:
	QString name_;
	QMap<MotionDirection, AMMotorGroupAxis*> axes_;
};

/*!
  * A class which represents a set of already grouped motor axes.
  */
class AMMotorGroup : public QObject
{
	Q_OBJECT

public:
	/*!
	  * Creates an instance of a motor group with no contained group objects.
	  */
	explicit AMMotorGroup(QObject *parent = 0);

	/*!
	  * Virtual destructor for an AMMotorGroup.
	  */
	virtual ~AMMotorGroup() {}

	/*!
	  * A list of all the group object names contained within the group.
	  */
	QList<QString> names() const;

	/*!
	  * A list of all the group objects contained within the group.
	  */
	QList<AMMotorGroupObject *> motorGroupObjects() const;

	/*!
	  * The number of group objects contained within the group.
	  */
	int size() const;

	/*!
	  * The group object with the provided name, or 0 if no matching group object
	  * could be found.
	  * \param name ~ The name of the motor group to be returned.
	  */
	AMMotorGroupObject *motorGroupObject(const QString &name) const;

	/*!
	  * The name of the provided motor group, or the empty string if no matching
	  * group object could be found.
	  * \param motorObject ~ The group object whose name is to be returned.
	  */
	QString name(AMMotorGroupObject *motorObject) const ;

public slots:

	/*!
	 * \brief Adds a group object to this group. If a motor group object is
	 * already found with this name, its resources will be freed, and it will
	 * be replaced by the new group object. This motor group will take ownership
	 * of the provided group object.
	 * \param object ~ The group object to be added to this group.
	 */
	void addMotorGroupObject(AMMotorGroupObject *object);

protected:
	/// Holds the map of the infos and the reference names.
	QMap<QString, AMMotorGroupObject *> groupObjects_;
};

#endif // AMMOTORGROUP_H
