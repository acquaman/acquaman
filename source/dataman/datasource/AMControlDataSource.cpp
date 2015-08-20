#include "AMControlDataSource.h"

AMControlDataSource::AMControlDataSource(AMControl *control, const QString &name, QObject *parent) :
	QObject(parent), AMDataSource(name)
{
	control_ = 0;
	axes_ = QList<AMAxisInfo>();

	setControl(control);
}

AMControlDataSource::~AMControlDataSource()
{

}

QString AMControlDataSource::typeDescription() const
{
	return "Scalar Process Variable Data Source";
}

int AMControlDataSource::state() const
{
	return control_->isConnected() ? ProcessingFlag : InvalidFlag;
}

QList<AMAxisInfo> AMControlDataSource::axes() const
{
	return axes_;
}

int AMControlDataSource::rank() const
{
	return 0;
}

AMnDIndex AMControlDataSource::size() const
{
	return AMnDIndex();
}

int AMControlDataSource::size(int axisNumber) const
{
	Q_UNUSED(axisNumber)
	return 1;
}

AMAxisInfo AMControlDataSource::axisInfoAt(int axisNumber) const
{
	Q_UNUSED(axisNumber)
	return AMAxisInfo("scalar", 1);
}

int AMControlDataSource::idOfAxis(const QString& axisName) const
{
	Q_UNUSED(axisName)
	return -1;
}

AMNumber AMControlDataSource::value(const AMnDIndex &indexes) const
{
	if(!control_ || !control_->isConnected())
		return AMNumber();

	if (indexes.rank() != 0)
		return AMNumber(AMNumber::DimensionError);

	return control_->value();
}

AMNumber AMControlDataSource::axisValue(int axisNumber, int index) const
{
	Q_UNUSED(axisNumber)
	Q_UNUSED(index)

	return 0;
}


void AMControlDataSource::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_) {
			disconnect( control_, 0, this, 0 );
		}

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(onDataChanged()) );
			connect( control_, SIGNAL(connected(bool)), this, SLOT(onStateChanged()) );
		}

		onDataChanged();
		onStateChanged();

		emit controlChanged(control_);
	}
}

void AMControlDataSource::onDataChanged()
{
	emitValuesChanged();
}

void AMControlDataSource::onStateChanged()
{
	emitStateChanged(state());
}
