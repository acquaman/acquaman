#ifndef AMSAMPLEPLATEMOVEACTIONINFO_H
#define AMSAMPLEPLATEMOVEACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMControlInfoList.h"

#define AMSAMPLEPLATEMOVEACTIONINFO_INVALID_SAMPLEPLATE_ID 327001
#define AMSAMPLEPLATEMOVEACTIONINFO_INVALID_SAMPLEPOSITION_INDEX 327002
#define AMSAMPLEPLATEMOVEACTIONINFO_INVALID_SAMPLEPOSITION_ID 327003

class AMSamplePlateMoveActionInfo : public AMActionInfo3
{
Q_OBJECT
Q_PROPERTY(int samplePlateId READ samplePlateId WRITE setSamplePlateId)
Q_PROPERTY(int samplePositionIndex READ samplePositionIndex WRITE setSamplePositionIndex)

public:
	/// Constructor. You should always specify the paramaters, but we provide the default for database reload
	Q_INVOKABLE AMSamplePlateMoveActionInfo(int samplePlateId = -1, int samplePositionIndex = -1, QObject *parent = 0);

	/// Copy constructor
	AMSamplePlateMoveActionInfo(const AMSamplePlateMoveActionInfo &other);

	/// This function is used as a virtual copy constructor
	virtual AMActionInfo3* createCopy() const;

	/// This describes the type of the action
	virtual QString typeDescription() const;

	/// Returns the database id of the samplePlate we're working with
	int samplePlateId() const;

	/// Returns the database id of the samplePosition on the plate we're interested in
	int samplePositionIndex() const;

	/// Returns the actual control info list (position we want to move to) ... loads this from the database if necessary
	const AMControlInfoList* samplePosition();

	/// Returns the name of the sample plate
	QString samplePlateName() const;

	/// Returns the name of the sample position
	QString samplePositionName() const;

public slots:
	/// Changes the plate we're intested in by choosing a different database id
	void setSamplePlateId(int samplePlateId);

	/// Changes the positino we're interested in by choosing a different database id
	void setSamplePositionIndex(int samplePositionIndex);

protected:
	/// A short helper function to update the action's description
	void updateDescriptionText();

protected:
	/// Holds the samplePlate database id
	int samplePlateId_;
	/// Holds the samplePosition index in the samplePlate
	int samplePositionIndex_;

	/// Holds a copy of the control info list (position we want to move to)
	AMControlInfoList *samplePosition_;
};

#endif // AMSAMPLEPLATEMOVEACTIONINFO_H
