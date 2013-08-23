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

	const AMSample* sampleAt(int index) const;
	AMSample* sampleAt(int index);

	/// Adds a sample and automatically calls append with the name as the key
	bool addSample(AMSample *sample);

	int indexOfSample(AMSample *sample);

	bool operator==(const AMSamplePlate &other);

public slots:
	void onSampleCameraShapesChanged();
	void onShapeDataPropertyUpdated(AMShapeData *shapeData);

signals:
	// The following signals are forwarded from our signalSource().
	void sampleChanged(int index);
	void sampleAdded(int index);
	void sampleRemoved(int index);
	void sampleAboutToBeAdded(int index);
	void sampleAboutToBeRemoved(int index);

	void sampleAddedThroughCamera(AMSample *sample);

protected slots:
	void onSampleDetailsChanged();

protected:
	AMSample* sampleFromShape(AMShapeData *shapeData);

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

class AMSamplePlateBrowser : public QAbstractListModel
{
Q_OBJECT
public:
	AMSamplePlateBrowser(AMDatabase *database, QObject *parent = 0);

	int rowCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	bool hasSamplePlate(AMSamplePlate *samplePlate);

public slots:
	void reloadFromDatabase();

	void addSamplePlate(AMSamplePlate *samplePlate);

	void setCurrentSamplePlate(AMSamplePlate *samplePlate);

protected:
	AMDatabase *database_;
	QList<AMSamplePlate*> allSamplePlates_;
	int currentSamplePlateIndex_;
};

#endif // AMSAMPLEPLATE_H
