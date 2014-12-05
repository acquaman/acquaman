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


#ifndef AMSAMPLEPLATE_H
#define AMSAMPLEPLATE_H

#include "dataman/database/AMDbObject.h"
#include "util/AMOrderedSet.h"
#include "dataman/AMSample.h"

class AMDeferredFunctionCall;

class AMSamplePlate : public AMDbObject
{
Q_OBJECT
Q_PROPERTY(QDateTime dateTime READ dateTime WRITE dbLoadDateTime)
Q_PROPERTY(AMDbObjectList samples READ dbGetSamples WRITE dbLoadSamples)
Q_PROPERTY(QVector3D platePosition READ platePosition WRITE setPlatePosition)
Q_PROPERTY(double plateRotation READ plateRotation WRITE setPlateRotation)

Q_CLASSINFO("AMDbObject_Attributes", "description=List of Samples on a Sample Plate")
Q_CLASSINFO("dateTime", "createIndex=true")

public:
	/// Invokable constructor for the AMDbObject system
	Q_INVOKABLE AMSamplePlate(QObject *parent = 0);
	/// Copy constructor
	AMSamplePlate(const AMSamplePlate &other);
	virtual ~AMSamplePlate();

	/// Returns the QDateTime when this sample plate was created (for ordering)
	QDateTime dateTime() const;

	/// Returns the number of samples on this plate
	int sampleCount() const;

	/// Returns a const pointer to the sample at a given index
	const AMSample* sampleAt(int index) const;
	/// Returns a pointer to the sample at a given index
	AMSample* sampleAt(int index);

	QList<AMSample*> allSamples();
	/// Returns the position of the plate
	QVector3D platePosition();
	/// Returns the rotation of the plate
	double plateRotation();

	/// Adds a sample and automatically calls append with the name as the key
	bool addSample(AMSample *sample);

	bool removeSample(AMSample *sample);

	/// Returns the index of a given sample
	int indexOfSample(const AMSample *sample);

	/// Returns the sample associated with the shapeData. If no sample is associated a null pointer is returned
	AMSample* sampleFromShape(AMShapeData *shapeData);

	/// Comparison operator for sample plates. Sample plates are considered the same if they have the same name, the same number of samples, and all of the samples are named the same and in the same order
	bool operator==(const AMSamplePlate &other);

public slots:
	/// Called when new shapes are available in the sampleCameraView. If there are shapes without samples we make and add the samples.
	void onSampleCameraShapesChanged(const QList<AMShapeData*> shapeList);
	/// Called when a shapeData claims to update properties definitely. We call storeToDb() on the associated sample.
	void onShapeDataPropertyUpdated(AMShapeData *shapeData);

	void requestSampleMoveTo(int index);

	void setPlatePosition(QVector3D position);
	void setPlateRotation(double rotation);

signals:
	// The following signals are forwarded from our signalSource().
	void sampleChanged(int index);
	void sampleAdded(int index);
	void sampleRemoved(int index);
	void sampleAboutToBeAdded(int index);
	void sampleAboutToBeRemoved(int index);

	void sampleMoveToRequested(AMShapeData *shapeData);

	// GET RID OF THIS?
	void sampleAddedThroughCamera(AMSample *sample);

protected slots:
	/// Handles changes to the sample details and causes updates in the related models by emitting sampleChanged
	void onSampleDetailsChanged();

	void onSampleModified(bool isModified);

	/// Handles saving the sample plate to the database if one of the samples was independently saved
	void onSampleRequestStoreToDb();

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

	/// Holds the list of samples
	AMOrderedList<AMSample*> samples_;

	/// Position of the sample plate
	QVector3D platePosition_;

	/// Rotation of sample plate
	double plateRotation_;

	/// Used to schedule a delayed call to refreshFromDb()
	AMDeferredFunctionCall *storeToDbFunctionCall_;
};

class AMSamplePlateBrowser : public QAbstractListModel
{
Q_OBJECT
public:
	/// Qt Model/View class constructor. Takes a database to load the sample plates from
	AMSamplePlateBrowser(AMDatabase *database, QObject *parent = 0);

	/// Returns the total number of sample plates available
	int rowCount(const QModelIndex &parent) const;
	/// Returns data from the sample plates
	QVariant data(const QModelIndex &index, int role) const;
	/// Returns Qt::ItemIsEnabled and Qt::ItemIsSelectable
	Qt::ItemFlags flags(const QModelIndex &index) const;
	/// Returns "Sample Plates"
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	/// Returns whether or not a samplePlate is in the model
	bool hasSamplePlate(AMSamplePlate *samplePlate);

	QList<AMSample*> allSamples();

public slots:
	/// Goes to the database to reload all sample plates
	void reloadFromDatabase();

	/// Adds a sample plate to the end of the list without causing a full model refresh
	void addSamplePlate(AMSamplePlate *samplePlate);

	/// Sets a data field for the sample plate currently loaded in the beamline
	void setCurrentSamplePlate(AMSamplePlate *samplePlate);

protected:
	/// The database we'll load from
	AMDatabase *database_;
	/// The list of all our sample plates
	QList<AMSamplePlate*> allSamplePlates_;
	/// The index of the currently loaded sample plate for the beamline (-1 if there is none)
	int currentSamplePlateIndex_;
};

#endif // AMSAMPLEPLATE_H
