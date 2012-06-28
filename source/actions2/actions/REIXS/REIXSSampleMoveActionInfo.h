/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef REIXSSAMPLEMOVEACTIONINFO_H
#define REIXSSAMPLEMOVEACTIONINFO_H

#include "actions2/AMActionInfo.h"

#include "dataman/info/AMControlInfoList.h"

/// Specifies the information required to move the REIXS sample manipulator into position (ie: for REIXSSampleMoveAction). This consists of an AMControlInfoList containing either some fixed manipulator positions (x, y, z, theta), OR the id and sampleIndex of a stored AMSamplePlate in the database. [If a valid sample plate id is provided, it takes precendence over the fixed positions. Note that in this mode, if the sample plate is updated before the action runs, it will move to the most-recently-updated position of the sample on the plate.]
class REIXSSampleMoveActionInfo : public AMActionInfo
{
    Q_OBJECT
	Q_PROPERTY(int samplePlateId READ samplePlateId WRITE setSamplePlateId)
	Q_PROPERTY(int sampleIndex READ sampleIndex WRITE setSampleIndex)
	Q_PROPERTY(double x READ x WRITE setX)
	Q_PROPERTY(double y READ y WRITE setY)
	Q_PROPERTY(double z READ z WRITE setZ)
	Q_PROPERTY(double theta READ theta WRITE setTheta)

public:
	// Constructors
	////////////////////

	/// Default constructor.  A default Q_INVOKABLE constructor is required; our default moves to a conservative safe sample measurement position.
	Q_INVOKABLE explicit REIXSSampleMoveActionInfo(QObject *parent = 0);
	/// This constructor version moves to the position specified in \c targetPositions, which should contain controls named sampleX, sampleY, sampleZ, and sampleTheta.  If the \c targetPositions is a safe position, and the manipulator is currently in a safe position (ie: not yet crashed), the move will ensure that there are no crashes in-between.
	REIXSSampleMoveActionInfo(const AMControlInfoList& targetPositions, QObject* parent = 0);
	/// This constructor version moves to the position specified by \c x, \c y, \c z, and \c theta, in mm, mm, mm, deg.
	REIXSSampleMoveActionInfo(double x, double y, double z, double theta, QObject* parent = 0);
	/// This constructor version moves to the position specified by sample \c sampleIndex on sample plate \c samplePlateId. Note that if the sample plate is updated before running, this will go to the most recently updated sample position.
	REIXSSampleMoveActionInfo(int samplePlateId, int sampleIndex, QObject* parent = 0);

	/// Copy constructor. Copies the position values from \c other's \c positions_.
	REIXSSampleMoveActionInfo(const REIXSSampleMoveActionInfo& other);

	/// This provides the necessary virtual copy constructor
	AMActionInfo* createCopy() const { return new REIXSSampleMoveActionInfo(*this); }

	// New public functions:
	/////////////////////////

	/// Returns true if this info contains the positions we need to do the move. (If samplePlateId() < 1, ensures that positions_ contains sampleX, sampleY, sampleZ, and sampleTheta. If samplePlateId is valid, ensures that sampleIndex_ is a valid sample on that plate, and that the positions stored at that index provide sampleX, sampleY, sampleZ, and sampleTheta.
	bool isValid() const;

	/// Returns the fixed position for the move. \note If a valid samplePlateId() is specified, the position at sampleIndex() on that plate is used instead of this position.
	double x() const { return x_; }
	/// Returns the fixed position for the move. \note If a valid samplePlateId() is specified, the position at sampleIndex() on that plate is used instead of this position.
	double y() const { return y_; }
	/// Returns the fixed position for the move. \note If a valid samplePlateId() is specified, the position at sampleIndex() on that plate is used instead of this position.
	double z() const { return z_; }
	/// Returns the fixed position for the move. \note If a valid samplePlateId() is specified, the position at sampleIndex() on that plate is used instead of this position.
	double theta() const { return theta_; }

	/// Returns the id of the sample plate in the database, or 0 if we should use x(), y(), z(), theta() instead.
	int samplePlateId() const { return samplePlateId_; }

	/// Returns the index of the sample on the plate at samplePlateId().
	int sampleIndex() const { return sampleIndex_; }


signals:

public slots:

	/// Sets the index of the sample on the plate at samplePlateId().
	void setSampleIndex(int index) { sampleIndex_ = index; setModified(true); updateDescriptions(); }

	/// Sets the sample plate id to use to determine the position. Set to 0 if we should use x(), y(), z(), theta() instead.
	void setSamplePlateId(int samplePlateId) { samplePlateId_ = samplePlateId; setModified(true); updateDescriptions(); }


	/// Set the manual target position (x), in mm.  The manual target position is only used if samplePlateId() is 0.
	void setX(double x) { x_ = x; setModified(true); updateDescriptions(); }
	void setY(double y) { y_ = y; setModified(true); updateDescriptions(); }
	void setZ(double z) { z_ = z; setModified(true); updateDescriptions(); }
	/// Set the manual target position (theta), in deg.  The manual target position is only used if samplePlateId() is 0.
	void setTheta(double theta) { theta_ = theta; setModified(true); updateDescriptions(); }

protected:
	/// The id of a stored sample plate in the database, that we will use to determine the position to move to.
	int samplePlateId_;
	/// The index of the sample on samplePlateId_, that we will use to determine the position to move to.
	int sampleIndex_;

	/// Manual target positions.
	double x_, y_, z_, theta_;

	/// A helper function to update the shortDescription() and longDescription() based on the samplePlateId and sampleIndex, or (if those aren't set), the positions_ values.
	void updateDescriptions();

};

#endif // REIXSSAMPLEMOVEACTIONINFO_H
