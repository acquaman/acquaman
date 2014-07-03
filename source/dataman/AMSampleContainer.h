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


#ifndef AMSAMPLECONTAINER_H
#define AMSAMPLECONTAINER_H

/*

#include <QList>
#include <QAbstractListModel>
#include <QDateTime>
#include <QImage>

class AMSample;
class AMShapeData;

/// This class is a container of zero or more AMSample
/// It inherits from the QAbstractListModel so that it can be
/// easily visualized using QListView.
class AMSampleContainer : public QAbstractListModel
{
	Q_OBJECT
public:
	/// Constructor
	AMSampleContainer(QObject* parent = 0);

	/// returns a const reference to the list of samples
	const QList<AMSample*> sampleList() const;

	/// returns the sample at the given index
	AMSample* sample(int index) const;

	/// returns the current sampleName
	QString sampleName() const;
	/// returns the sample name at the given index
	QString sampleName(int index) const;
	/// returns the current sample's dateTime
	QDateTime dateTime() const;
	/// returns the dateTime for the specified index
	QDateTime dateTime(int index) const;
	/// returns the dateTime as a string for the current sample
	QString dateTimeString() const;
	/// returns the dateTiem as a string for the given index
	QString dateTimeString(int index) const;
	/// returns the sample notes for the current sample
	QString sampleNotes() const;
	/// returns the sample notes for the given index
	QString sampleNotes(int index) const;
	/// return the sample image for the current sample
	QByteArray sampleImage() const;
	/// returns the sample image for the given index
	QByteArray sampleImage(int index) const;

	/// returns the index of the current sample
	int currentIndex() const;
	/// returns the number of samples
	int count() const;

	/// returns the number of rows
	int rowCount(const QModelIndex &parent) const;

	/// returns the data - reimplemented from QAbstractListModel::data
	QVariant data(const QModelIndex &index, int role) const;

	/// returns the index of the sample with sampleShapePositionData shape
	int indexOfSample(const AMShapeData* shape) const;


public slots:
	/// sets the sample List to the one provided
	void setSampleList(QList<AMSample*> sampleList);
	/// adds the given sample to the list
	void addSample(AMSample* sample);
	/// removes the specified sample from the list
	void removeSample(AMSample* sample);
	/// sets the name of the specified sample
	void setSampleName(QString name, int index);
	/// sets the dateTime of the specified sample
	void setSampleDateTime(QDateTime dateTime, int index);
	/// sets the notes of the specified sample
	void setSampleNotes(QString notes, int index);
	/// sets the image of the specified sample, as a QByteArray
	void setSampleImage(QByteArray image, int index);
	/// sets the image of the specified sample, as a QImage
	void setSampleImage(QImage image, int index);
	/// sets the currentIndex to index
	void setIndex(int index);

	/// updates the sample list to correspond to
    /// the shapeList in AMSampleCamera
	void updateSamples();
	/// updates the names of the samples
	void updateNames();

signals:
	void sampleAdded(AMSample *sample);

protected:
	/// checks to see if the given index is valid
	bool isValid(int index) const;
	/// checks to see if the list has a sample with the given shape
	bool hasShape(AMShapeData* shapeData) const;

protected:
	/// list of samples
	QList<AMSample*> sampleList_;
	/// current index
	int index_;
	/// null sample, for use in initializing displays
	AMSample* nullSample_;
};

*/

#endif // AMSAMPLECONTAINER_H
