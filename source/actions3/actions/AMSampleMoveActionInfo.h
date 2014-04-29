#ifndef AMSAMPLEMOVEACTIONINFO_H
#define AMSAMPLEMOVEACTIONINFO_H

#include "actions3/AMActionInfo3.h"

class AMSample;

class AMSampleMoveActionInfo : public AMActionInfo3
{
Q_OBJECT
Q_PROPERTY(AMConstDbObject* sample READ dbReadSample WRITE dbLoadSample)

public:
	/// Constructor takes a const pointer to an AMSample
	Q_INVOKABLE AMSampleMoveActionInfo(const AMSample *sample = 0, QObject *parent = 0);

	/// Copy constructor
	AMSampleMoveActionInfo(const AMSampleMoveActionInfo &other);

	/// Destructor
	virtual ~AMSampleMoveActionInfo();

	/// This function is used as a virtual copy constructor
	virtual AMActionInfo3* createCopy() const;

	/// This describes the type of the action
	virtual QString typeDescription() const;

	/// Returns a const pointer to the sample
	const AMSample* sample() const;

public slots:
	/// Sets the sample from a const pointer
	void setSample(const AMSample *sample);

protected:
	/// A short helper function to update the action's description
	void updateDescriptionText();

	/// Database reading so we can reference this sample in the database
	AMConstDbObject* dbReadSample() const;
	/// Database loading so we can pull this sample out of the database
	void dbLoadSample(AMConstDbObject *newSample);

protected:
	/// Our sample as an AMConstDbObject pointer
	AMConstDbObject *sample_;
};

#endif // AMSAMPLEMOVEACTIONINFO_H
