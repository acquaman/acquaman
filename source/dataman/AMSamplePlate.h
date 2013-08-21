#ifndef AMSAMPLEPLATE_H
#define AMSAMPLEPLATE_H

#include "dataman/database/AMDbObject.h"
#include "util/AMOrderedSet.h"
#include "dataman/AMSample.h"

class AMSamplePlate : public AMDbObject
{
Q_OBJECT
Q_PROPERTY(QDateTime dateTime READ dateTime WRITE dbLoadDateTime)
Q_PROPERTY(AMDbObjectList samples READ dbGetSamples WRITE dbLoadSamples)

Q_CLASSINFO("AMDbObject_Attributes", "description=List of Samples on a Sample Plate")
Q_CLASSINFO("dateTime", "createIndex=true")

public:
	/// Invokable constructor for the AMDbObject system
	Q_INVOKABLE AMSamplePlate(QObject *parent = 0);
	/// Copy constructor
	AMSamplePlate(const AMSamplePlate &other);

	/// Returns the QDateTime when this sample plate was created (for ordering)
	QDateTime dateTime() const;

	int sampleCount() const;

	AMSample* sampleAt(int index);

	/// Adds a sample and automatically calls append with the name as the key
	bool addSample(AMSample *sample);

	int indexOfSample(AMSample *sample);

signals:
	// The following signals are forwarded from our signalSource().
	void sampleChanged(int index);
	void sampleAdded(int index);
	void sampleRemoved(int index);
	void sampleAboutToBeAdded(int index);
	void sampleAboutToBeRemoved(int index);

protected slots:
	//void onSampleNameChanged(const QString &name);
	void onSampleDetailsChanged();

protected:
	/// Set the dateTime for the AMDbObject system
	void dbLoadDateTime(const QDateTime &newDateTime);

	/// Used to store the sample list to the database
	AMDbObjectList dbGetSamples();
	/// Used to retrieve the sample list from the database
	void dbLoadSamples(const AMDbObjectList &newSamples);

protected:
	/// QDateTime for when this sample plate was created
	QDateTime dateTime_;

	AMOrderedList<AMSample*> samples_;

};

#endif // AMSAMPLEPLATE_H
