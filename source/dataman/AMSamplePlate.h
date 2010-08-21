#ifndef AMSAMPLEPLATE_H
#define AMSAMPLEPLATE_H

#include <QList>
#include <QDateTime>
#include <QDebug>

#include "AMSample.h"
#include "AMControlSetInfo.h"
#include "AMBiHash.h"

class AMSamplePosition;

class AMSamplePlate : public QObject
{
Q_OBJECT
public:
	explicit AMSamplePlate(QObject *parent = 0);

	~AMSamplePlate() {}

	QString plateName() const;
	int count();

	AMSamplePosition* samplePositionAt(int index);
	AMSamplePosition* samplePositionByName(const QString &name);

	AMSample* sampleAt(int index);
	AMSample* sampleByName(const QString &name);

	AMControlSetInfo* positionAt(int index);
	AMControlSetInfo* positionByName(const QString &name);

	int indexOf(const QString &name);

signals:

public slots:
	void setName(const QString &name);

	bool addSamplePosition(int index, AMSamplePosition *sp);
	bool addSamplePosition(int index, AMSample *sample, AMControlSetInfo *position);
	bool appendSamplePosition(AMSamplePosition *sp);
	bool appendSamplePosition(AMSample *sample, AMControlSetInfo *position);

	bool removeSamplePosition(AMSamplePosition *sp);
	bool removeSamplePosition(int index);

protected:
	QString userName_;
	QString timeName_;
	QList<AMSamplePosition*> samples_;
	AMBiHash<QString, AMSamplePosition*> sampleName2samplePosition_;
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

protected:
	AMSample *sample_;
	AMControlSetInfo *position_;
};

#endif // AMSAMPLEPLATE_H
