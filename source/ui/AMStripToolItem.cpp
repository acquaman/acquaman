#include "AMStripToolItem.h"
#include "MPlot/MPlotSeries.h"
#include "beamline/AMPVControl.h"
#include "dataman/datasource/AMDataSource.h"
#include "dataman/datasource/AMDataSourceSeriesData.h"
#include "dataman/datasource/AMStripToolControlDataSource.h"

AMStripToolItem::AMStripToolItem(AMControl *control, QObject *parent) :
	QObject(parent)
{
	// Initialize class variables.

	initialize();

	// Current settings.

	setControl(control);
}

AMStripToolItem::AMStripToolItem(const QString &pvName, QObject *parent) :
	QObject(parent)
{
	// Initialize class variables.

	initialize();

	// Current settings.

	setControl(pvName);
}

AMStripToolItem::~AMStripToolItem()
{

}

void AMStripToolItem::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_) {
			delete plotItem_;
			plotItem_ = 0;
		}

		control_ = newControl;

		if (control_) {
			plotItem_ = createPlotItem(control_);
		}

		emit controlChanged(control_);
	}
}

void AMStripToolItem::setControl(const QString &pvName)
{
	setControl( createControl(pvName) );
}

void AMStripToolItem::initialize()
{
	control_ = 0;
	plotItem_ = 0;
}

AMControl* AMStripToolItem::createControl(const QString &pvName)
{
	AMControl *result = 0;

	if (!pvName.isEmpty())
		result = new AMReadOnlyPVControl(pvName, pvName);

	return result;
}

MPlotItem* AMStripToolItem::createPlotItem(AMControl *control)
{
	MPlotSeriesBasic *result = 0;

	if (control) {
		AMDataSource *dataSource = new AMStripToolControlDataSource(control, control->name(), this);
		result = new MPlotSeriesBasic(new AMDataSourceSeriesData(dataSource));
	}

	return result;
}
