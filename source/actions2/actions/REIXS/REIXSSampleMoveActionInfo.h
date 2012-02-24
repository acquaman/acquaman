#ifndef REIXSSAMPLEMOVEACTIONINFO_H
#define REIXSSAMPLEMOVEACTIONINFO_H

#include "actions2/AMActionInfo.h"

#include "dataman/info/AMControlInfoList.h"

/// Specifies the information required to move the REIXS sample manipulator into position (ie: for REIXSSampleMoveAction). This consists of an AMControlInfoList containing the manipulator positions, under the control names: sampleX, sampleY, sampleZ, and sampleTheta.
class REIXSSampleMoveActionInfo : public AMActionInfo
{
    Q_OBJECT
	Q_PROPERTY(AMDbObject* positions READ dbReadPositions WRITE dbLoadPositions)
public:
	// Constructors
	////////////////////

	/// Default constructor.  A default Q_INVOKABLE constructor is required; our default moves to a conservative safe sample measurement position.
	Q_INVOKABLE explicit REIXSSampleMoveActionInfo(QObject *parent = 0);
	/// This constructor version moves to the position specified in \c targetPositions, which should contain controls named sampleX, sampleY, sampleZ, and sampleTheta.  If the \c targetPositions is a safe position, and the manipulator is currently in a safe position (ie: not yet crashed), the move will ensure that there are no crashes in-between.
	REIXSSampleMoveActionInfo(const AMControlInfoList& targetPositions, QObject* parent = 0);

	/// Copy constructor. Copies the position values from \c other's \c positions_.
	REIXSSampleMoveActionInfo(const REIXSSampleMoveActionInfo& other);

	/// This provides the necessary virtual copy constructor
	AMActionInfo* createCopy() const { return new REIXSSampleMoveActionInfo(*this); }

	// New public functions:
	/////////////////////////

	/// Returns true if this info contains the positions we need to do the move. (Ensures that positions_ contains sampleX, sampleY, sampleZ, and sampleTheta.)
	bool isValid() const;

	/// Returns a constant reference to the AMControlInfoList that stores the target postions
	const AMControlInfoList& targetPositions() const { return positions_; }
	/// Set the target positions for the move.  It should contain AMControlInfos named sampleX, sampleY, sampleZ, and sampleTheta.
	void setTargetPositions(const AMControlInfoList& targetPositions);

	// Database Loading and storage
	/////////////////////

	/// Used during database loading and saving to access the positions, stored as an AMControlInfoList.
	AMDbObject* dbReadPositions() { return &positions_; }
	/// Required for database loading and saving, but never used because dbReadPositions always returns a valid pointer.
	void dbLoadPositions(AMDbObject*) {}

signals:

public slots:

protected:
	/// An AMControlInfoList containing the desired positions of the sample manipulator (sampleX, sampleY, sampleZ, and sampleTheta
	AMControlInfoList positions_;

	/// A helper function to update the shortDescription() and longDescription() based on the position values.
	void updateDescriptions();

};

#endif // REIXSSAMPLEMOVEACTIONINFO_H
