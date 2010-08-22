#ifndef AMSAMPLEPLATE_H
#define AMSAMPLEPLATE_H

#include <QList>
#include <QDateTime>
#include <QAbstractListModel>
#include <QDebug>

#include "AMSample.h"
#include "AMControlSetInfo.h"
#include "AMBiHash.h"

class AMSamplePosition;
class AMSamplePlateModel;

class AMSamplePlate : public QObject
{
Q_OBJECT
public:
	explicit AMSamplePlate(QObject *parent = 0);

	~AMSamplePlate() {}

	AMSamplePlateModel* model();
	QString plateName() const;
	int count();

	AMSamplePosition* samplePositionAt(size_t index);
	AMSamplePosition* samplePositionByName(const QString &name);

	AMSample* sampleAt(size_t index);
	AMSample* sampleByName(const QString &name);

	AMControlSetInfo* positionAt(size_t index);
	AMControlSetInfo* positionByName(const QString &name);

	int indexOf(const QString &name);

signals:
	void samplePositionChanged(int index);
	void samplePositionAdded(int index);
	void samplePositionRemoved(int index);

public slots:
	void setName(const QString &name);

	bool setSamplePosition(size_t index, AMSamplePosition *sp);

	bool addSamplePosition(size_t index, AMSamplePosition *sp);
	bool addSamplePosition(size_t index, AMSample *sample, AMControlSetInfo *position);
	bool appendSamplePosition(AMSamplePosition *sp);
	bool appendSamplePosition(AMSample *sample, AMControlSetInfo *position);

	bool removeSamplePosition(AMSamplePosition *sp);
	bool removeSamplePosition(size_t index);

protected slots:
	void onDataChanged(QModelIndex a, QModelIndex b);
	void onRowsInserted(QModelIndex parent, int start, int end);
	void onRowsRemoved(QModelIndex parent, int start, int end);

protected:
	bool setupModel();

protected:
	QString userName_;
	QString timeName_;
	AMSamplePlateModel *samples_;
	AMBiHash<QString, AMSamplePosition*> sampleName2samplePosition_;

private:
	int insertRowLatch;
};

class AMSamplePlateModel : public QAbstractListModel
{
Q_OBJECT
public:
	AMSamplePlateModel(QObject *parent = 0);

	int rowCount(const QModelIndex & /*parent*/) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role);
	bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
	bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());
	Qt::ItemFlags flags(const QModelIndex &index) const;

protected:
	QList<AMSamplePosition*> *samples_;
};

class AMSamplePosition : public QObject
{
Q_OBJECT
public:
	AMSamplePosition(AMSample *sample, AMControlSetInfo *position, QObject *parent = 0);

	AMSample* sample();
	AMControlSetInfo* position();

public slots:
	void setSample(AMSample *sample);
	void setPosition(AMControlSetInfo *position);

signals:
	void positionValuesChanged(int index);

protected:
	AMSample *sample_;
	AMControlSetInfo *position_;
};

#endif // AMSAMPLEPLATE_H
