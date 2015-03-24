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

class AMMotorGroupObjectView;

/// The object that contains all the information necessary for viewing.  Contains extra information and a logical grouping for up to three controls.
class AMMotorGroupObject : public QObject
{
	Q_OBJECT

public:
	/// Enum for the orientation of motor.
	enum Orientation { Horizontal = 0, Vertical, Normal, Other };
	/// Enum for the motion type of the motor.
	enum MotionType { None = -1, Translational = 0, Rotational = 1 };

	/// Constructor.  Builds a new motor group object with a single control.
 	virtual ~AMMotorGroupObject();
	AMMotorGroupObject(const QString &name, const QString &prefix, const QString &units, AMControl *control, Orientation orientation, MotionType motionType, QObject *parent = 0);
	/// Constructor.  Builds a new motor group object with up to three controls.
	AMMotorGroupObject(const QString &name, const QStringList &prefixes, const QStringList &units, const QList<AMControl *> controls, QList<Orientation> orientations, QList<MotionType> motionTypes, QObject *parent = 0);

	/// Returns the size of the object. This returns the number of controls this object looks after.
	int size() const { return controls_.size(); }
	/// Returns the name of the info.
	QString name() const { return name_; }
	/// Returns the list of prefixes in the order of the controls.
	QStringList prefixes() const { return prefixes_; }
	/// Returns the prefix at element \param index.
	QString prefixAt(int index) const { return prefixes_.at(index); }
	/// Returns the list of units in the order of the controls.
	QStringList units() const { return units_; }
	/// Returns the unit at element \param index.
	QString unitAt(int index) const { return units_.at(index); }
	/// Returns the list of controls.
	QList<AMControl *> controls() const { return controls_; }
	/// Returns the control at element \param index.
	AMControl *controlAt(int index) const { return controls_.at(index); }
	/// Returns the orientations in the order of the controls.
	QList<Orientation> orientations() const { return orientations_; }
	/// Returns the orientation at element \param index.
	Orientation orientationAt(int index) const { return orientations_.at(index); }
	/// Returns the motion types in the order of the controls.
	QList<MotionType> motionTypes() const { return motionTypes_; }
	/// Returns the motion type at element \param index.
	MotionType motionTypeAt(int index) const { return motionTypes_.at(index); }

	// Convenience getters.
	/// Returns the index of the horizontal control (or -1 if no horizontal orientation control exists).
	int horizontalIndex() const;
	/// Returns the index of the vertical control (or -1 if no vertical orientation control exists).
	int verticalIndex() const;
	/// Returns the index of the normal control (or -1 if no normal orientation control exists).
	int normalIndex() const;

	/// Returns the prefix of the horizontal control, if it exists.  Returns an empty string otherwise.
	QString horizontalPrefix() const;
	/// Returns the prefix of the vertical control, if it exists.  Returns an empty string otherwise.
	QString verticalPrefix() const;
	/// Returns the prefix of the normal control, if it exists.  Returns an empty string otherwise.
	QString normalPrefix() const;

	/// Returns the units of the horizontal control, if it exists.  Returns an empty string otherwise.
	QString horizontalUnits() const;
	/// Returns the units of the vertical control, if it exists.  Returns an empty string otherwise.
	QString verticalUnits() const;
	/// Returns the units of the normal control, if it exists.  Returns an empty string otherwise.
	QString normalUnits() const;

	/// Returns the horizontal control, if it exists.  Returns 0 otherwise.
	AMControl *horizontalControl() const;
	/// Returns the vertical control, if it exists.  Returns 0 otherwise.
	AMControl *verticalControl() const;
	/// Returns the normal control, if it exists.  Returns 0 otherwise.
	AMControl *normalControl() const;

	/// Returns the horizontal motion type, if it exists.  Returns None otherwise.
	AMMotorGroupObject::MotionType horizontalMotionType() const;
	/// Returns the vertical motion type, if it exists.  Returns None otherwise.
	AMMotorGroupObject::MotionType verticalMotionType() const;
	/// Returns the normal motion type, if it exists.  Returns None otherwise.
	AMMotorGroupObject::MotionType normalMotionType() const;

	/// check whether there is motor in this motor group moving
	bool isMotorMoving() const;

	// Old actions.  Will update with new ones as soon as I can.
	/// Returns a newly created move action for the horizontal postion.  Returns 0 if not connected.
	AMAction3 *createHorizontalMoveAction(double position);
	/// Returns a newly created stop action for the horizontal position.  Returns 0 if not connected.
	AMAction3 *createHorizontalStopAction();
	/// Returns a newly created move action for the vertical postion.  Returns 0 if not connected.
	AMAction3 *createVerticalMoveAction(double position);
	/// Returns a newly created stop action for the vertical position.  Returns 0 if not connected.
	AMAction3 *createVerticalStopAction();
	/// Returns a newly created move action for the normal postion.  Returns 0 if not connected.
	AMAction3 *createNormalMoveAction(double position);
	/// Returns a newly created stop action for the normal position.  Returns 0 if not connected.
	AMAction3 *createNormalStopAction();
	/// Returns a newly created action that stops ALL the motors.  Returns 0 if not all motors are connected.
	AMAction3 *createStopAllAction();

	virtual AMMotorGroupObjectView *createMotorGroupObjectView();

protected:
	/// Holds the name of the info.
	QString name_;
	/// List that holds the prefixes.
	QStringList prefixes_;
	/// List that holds the units of the object.
	QStringList units_;
	/// List that holds the controls of the object.
	QList<AMControl *> controls_;
	/// List that holds the orientations of the object.
	QList<Orientation> orientations_;
	/// List that holds the motion types of the object.
	QList<MotionType> motionTypes_;
};

/// Container for all the motor group objects.  This acts like a pseudo-sample stage by grouping necessary controls.
class AMMotorGroup : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMMotorGroup(QObject *parent = 0);

	/// Returns all of the names in the group.
	QList<QString> names() const { return infoMap_.keys(); }
	/// Returns all of the motor group info's.
	QList<AMMotorGroupObject *> motorGroupObjects() const { return infoMap_.values(); }
	/// Returns the number of motor group objects.
	int size() const { return infoMap_.size(); }

	/// Returns the info for a given name.  Returns 0 if not found.
	AMMotorGroupObject *motorGroupObject(const QString &name) const;
	/// Returns the name for a given info.  Returns a null string if not found.
	QString name(AMMotorGroupObject *motorObject) const ;

public slots:
	/// Adds a motor group info with the given reference name.
	void addMotorGroupObject(const QString &name, AMMotorGroupObject *object);

protected:
	/// Holds the map of the infos and the reference names.
	QMap<QString, AMMotorGroupObject *> infoMap_;
};

#endif // AMMOTORGROUP_H
