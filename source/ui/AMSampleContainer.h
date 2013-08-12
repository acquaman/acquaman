#ifndef AMSAMPLECONTAINER_H
#define AMSAMPLECONTAINER_H

#include <QList>
#include <QAbstractListModel>
#include <QDateTime>
#include <QImage>

class AMSampleEthan;
class AMShapeData;

class AMSampleContainer : public QAbstractListModel
{
    Q_OBJECT
public:
    AMSampleContainer(QObject* parent = 0);

    const QList<AMSampleEthan*> sampleList() const;

    AMSampleEthan* sample(int index) const;

    QString sampleName() const;
    QString sampleName(int index) const;
    QDateTime dateTime() const;
    QDateTime dateTime(int index) const;
    QString dateTimeString() const;
    QString dateTimeString(int index) const;
    QString sampleNotes() const;
    QString sampleNotes(int index) const;
    QByteArray sampleImage() const;
    QByteArray sampleImage(int index) const;

    int index() const;
    int count() const;

    int rowCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;


public slots:
    void setSampleList(QList<AMSampleEthan*> sampleList);
    void addSample(AMSampleEthan* sample);
    void removeSample(AMSampleEthan* sample);

    void setSampleName(QString name, int index);
    void setSampleDateTime(QDateTime dateTime, int index);
    void setSampleNotes(QString notes, int index);
    void setSampleImage(QByteArray image, int index);
    void setSampleImage(QImage image, int index);
    void setIndex(int index);

    void updateSamples();


protected:
    bool isValid(int index) const;
    bool hasShape(AMShapeData* shapeData) const;

protected:
    QList<AMSampleEthan*> sampleList_;
    int index_;
    AMSampleEthan* nullSample_;
};

#endif // AMSAMPLECONTAINER_H
