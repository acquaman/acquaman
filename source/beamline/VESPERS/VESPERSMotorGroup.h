#ifndef VESPERSMOTORGROUP_H
#define VESPERSMOTORGROUP_H

#include <QObject>
#include <QMap>

#include "beamline/AMControl.h"

/// The info that contains all the information necessary for viewing.  Contains extra information and a logical grouping for one or two controls.
class VESPERSMotorGroupInfo : public QObject
{
	Q_OBJECT

public:
	/// Enum for the orientation of motor.
	enum Orientation { None = 0, Horizontal, Vertical };
	/// Enum for the motion type of the motor.
	enum MotionType { Translational = 0, Rotational = 1 };

	/// Constructor.  Builds a new motor group info with a single control.
	VESPERSMotorGroupInfo(const QString &name, AMControl *control, Orientation orientation, MotionType motionType, QObject *parent = 0);
	/// Constructor.  Builds a new motor group info using two controls.  Currently assumes both are the same motion type.
	VESPERSMotorGroupInfo(const QString &name, AMControl *firstControl, AMControl *secondControl, MotionType motionType, QObject *parent = 0);

	/// Returns the name of the info.
	QString name() const { return name_; }
	/// Returns the first control of the info.
	AMControl *firstControl() const { return firstControl_; }
	/// Returns the second control of the info.
	AMControl *secondControl() const { return secondControl_; }
	/// Returns the orientation of the first control.  If there are two valid controls in the info then this returns Orientation::None because orientation isn't as meaningful in this case.
	Orientation orientation() const { return orientation_; }
	/// Returns the motion type for this info.
	MotionType motionType() const { return motionType_; }

protected:
	/// Holds the name of the info.
	QString name_;
	/// Holds the pointer to the first control.
	AMControl *firstControl_;
	/// Holds the pointer to the second control (if it exists).
	AMControl *secondControl_;
	/// Holds the orientation of the first control motor.  If there are two, orientation doesn't matter.
	Orientation orientation_;
	/// Holds the motion type for this info.  Currently limited to either translational or rotational.  Not both.
	MotionType motionType_;
};

/// Container for all the motor group infos.
class VESPERSMotorGroup : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	VESPERSMotorGroup(QObject *parent = 0);

	/// Returns all of the names in the group.
	QList<QString> names() const { return infoMap_.keys(); }
	/// Returns all of the motor group info's.
	QList<VESPERSMotorGroupInfo *> motorGroupInfos() const { return infoMap_.values(); }

	/// Returns the info for a given name.  Returns 0 if not found.
	VESPERSMotorGroupInfo *motorGroupInfo(const QString &name) const;
	/// Returns the name for a given info.  Returns a null string if not found.
	QString name(VESPERSMotorGroupInfo *motorInfo) const ;

public slots:
	/// Adds a motor group info with the given reference name.
	void addMotorGroupInfo(const QString &name, VESPERSMotorGroupInfo *info);

protected:
	/// Holds the map of the infos and the reference names.
	QMap<QString, VESPERSMotorGroupInfo *> infoMap_;
};

#endif // VESPERSMOTORGROUP_H
