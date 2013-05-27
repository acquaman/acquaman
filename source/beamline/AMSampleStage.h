#ifndef AMSAMPLESTAGE_H
#define AMSAMPLESTAGE_H

#include <QObject>
#include "beamline/AMControl.h"

/// This class controls a single axis that would be used in conjunction with AMSampleStage.
class AMSampleStageAxis : public QObject
{
	Q_OBJECT

public:
	/// Enum that holds whether the axis is translational or rotational.
	enum MotionType { Translational = 1, Rotational = 2 };
	/// Builds a sample stage axis with the given name and type.
	explicit AMSampleStageAxis(const QString &name, MotionType type, AMControl *control, QObject *parent = 0);
};

/// This class encapulates a sample stage using the acquaman framework.
/*!
	This class is intended to be as agnostic as possible about providing an efficient means of
	combining motors together that move a sample in a general 3D space.  Not all sample stages
	will have the capability to move in all directions or even be able to move thoroughout the
	entire movement range (eg: only being able to move 20 degrees of 360 degrees).

	Although not fully implemented, when you add an axis you must bind it to one of the 6 orthogonal
	axes: x, y, z, theta, phi, or gamma.  This allows the possibility to create new ways
	to move the sample stage by using simple transforms.
  */
class AMSampleStage : public QObject
{
	Q_OBJECT

public:
	/// Builds an empty sample stage.
	explicit AMSampleStage(QObject *parent = 0);

signals:

public slots:

};

#endif // AMSAMPLESTAGE_H
