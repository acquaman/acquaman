#ifndef AMSAMPLECONTAINER_H
#define AMSAMPLECONTAINER_H

#include <QList>
#include <QAbstractListModel>
#include <QDateTime>
#include <QImage>

class AMSampleEthan;
class AMShapeData;

/// This class is a container of zero or more AMSampleEthan
/// It inherits from the QAbstractListModel so that it can be
/// easily visualized using QListView.
class AMSampleContainer : public QAbstractListModel
{
    Q_OBJECT
public:
    /// Constructor
    AMSampleContainer(QObject* parent = 0);

    /// returns a const reference to the list of samples
    const QList<AMSampleEthan*> sampleList() const;

    /// returns the sampel at the given index
    AMSampleEthan* sample(int index) const;

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
    void setSampleList(QList<AMSampleEthan*> sampleList);
    /// adds the given sample to the list
    void addSample(AMSampleEthan* sample);
    /// removes the specified sample from the list
    void removeSample(AMSampleEthan* sample);
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
    /// the shapeList in AMShapeDataSet
    void updateSamples();
    /// updates the names of the samples
    void updateNames();


protected:
    /// checks to see if the given index is valid
    bool isValid(int index) const;
    /// checks to see if the list has a sample with the given shape
    bool hasShape(AMShapeData* shapeData) const;

protected:
    /// list of samples
    QList<AMSampleEthan*> sampleList_;
    /// current index
    int index_;
    /// null sample, for use in initializing displays
    AMSampleEthan* nullSample_;
};

#endif // AMSAMPLECONTAINER_H
