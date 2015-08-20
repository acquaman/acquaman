#include "AMStripToolControlDataSource.h"

AMStripToolControlDataSource::AMStripToolControlDataSource(AMControl *control, const QString &name, QObject *parent) :
	AM1DTimedDataAB(name, parent)
{
	control_ = 0;

	controlSource_ = 0;
	valuesSource_ = 0;
	timesSource_ = 0;

	setControl(control);
}

AMStripToolControlDataSource::~AMStripToolControlDataSource()
{

}

void AMStripToolControlDataSource::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_) {
			if (controlSource_) {
				controlSource_->deleteLater();
				controlSource_ = 0;
			}

			if (valuesSource_) {
				valuesSource_->deleteLater();
				valuesSource_ = 0;
			}

			if (timesSource_) {
				timesSource_->deleteLater();
				timesSource_ = 0;
			}
		}

		control_ = newControl;

		if (control_) {
			controlSource_ = new AMControlDataSource(control_, control_->name(), this);
			valuesSource_ = createValuesDataSource(controlSource_);
			timesSource_ = createTimesDataSource(controlSource_);
		}

		setInputDataSources(QList<AMDataSource*>() << valuesSource_ << timesSource_);

		emit controlChanged(control_);
	}
}

AM0DAccumulatorAB* AMStripToolControlDataSource::createValuesDataSource(AMControlDataSource *controlSource)
{
	AM0DAccumulatorAB *data = new AM0DAccumulatorAB("PVData", this);
	data->setDataStoredCountMax(50);
	data->setInputDataSources(QList<AMDataSource*>() << controlSource);

	return data;
}

AM0DTimestampAB* AMStripToolControlDataSource::createTimesDataSource(AMControlDataSource *controlSource)
{
	AM0DTimestampAB *timestamp = new AM0DTimestampAB("PVDataTimestamp", this);
	timestamp->setDataStoredCountMax(50);
	timestamp->setInputDataSources(QList<AMDataSource*>() << controlSource);

	return timestamp;
}
