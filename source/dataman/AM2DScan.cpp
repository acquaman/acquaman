#include "AM2DScan.h"

AM2DScan::AM2DScan(QObject *parent)
	: AMScan(parent)
{
	data_->addScanAxis(AMAxisInfo("x", 0, "x-axis"));
	data_->addScanAxis(AMAxisInfo("y", 0, "y-axis"));
}
