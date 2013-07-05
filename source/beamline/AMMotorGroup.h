#ifndef AMMOTORGROUP_H
#define AMMOTORGROUP_H

#include <QObject>
#include <QMap>

#include "beamline/AMControl.h"

/// The object that contains all the information necessary for viewing.  Contains extra information and a logical grouping for up to three controls.
class AMMotorGroupObject : public QObject
{
	Q_OBJECT

public:
	/// Enum for the orientation of motor.
	enum Orientation { Horizontal = 0, Vertical, Normal };
	/// Enum for the motion type of the motor.
	enum MotionType { Translational = 0, Rotational = 1 };

	/// Constructor.  Builds a new motor group object with a single control.
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
