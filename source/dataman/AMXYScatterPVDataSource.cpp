#include "AMXYScatterPVDataSource.h"

AMXYScatterPVDataSource::AMXYScatterPVDataSource(const AMProcessVariable *x, const AMProcessVariable *y, const QString &name, QObject *parent)
	: QObject(parent), AMDataSource(name)
{
	x_ = x;
	y_ = y;

	connect(x_, SIGNAL(valueChanged()), this, SLOT(onDataChanged()));
	connect(x_, SIGNAL(hasValuesChanged(bool)), this, SLOT(onStateChanged()));
	connect(y_, SIGNAL(valueChanged()), this, SLOT(onDataChanged()));
	connect(y_, SIGNAL(hasValuesChanged(bool)), this, SLOT(onStateChanged()));

	AMAxisInfo ai("xValue", 0, "X-value");
	ai.isUniform = true;
	ai.size = 1;
	axes_ << ai;
}
