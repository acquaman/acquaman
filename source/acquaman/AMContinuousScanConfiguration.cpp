#include "AMContinuousScanConfiguration.h"

AMContinuousScanConfiguration::AMContinuousScanConfiguration(QObject *parent)
	: AMScanConfiguration(parent)
{

}

AMContinuousScanConfiguration::AMContinuousScanConfiguration(const AMContinuousScanConfiguration &original)
	: AMScanConfiguration(original)
{
	foreach (AMScanAxis *axis, original.scanAxes())
		scanAxes_.append(axis->createCopy());
}

AMContinuousScanConfiguration::~AMContinuousScanConfiguration()
{
	for (int i = 0, size = scanAxes_.count(); i < size; i++)
		scanAxes_.at(i)->deleteLater();

	scanAxes_.clear();
}

AMDbObjectList AMContinuousScanConfiguration::dbReadScanAxes()
{
	AMDbObjectList list;

	foreach (AMScanAxis *axis, scanAxes_.toList())
		list << axis;

	return list;
}

void AMContinuousScanConfiguration::dbLoadScanAxes(const AMDbObjectList &newScanAxes)
{
	for (int i = 0, size = scanAxes_.count(); i < size; i++)
		scanAxes_.at(i)->deleteLater();

	scanAxes_.clear();

	foreach (AMDbObject *object, newScanAxes){

		AMScanAxis *axis = qobject_cast<AMScanAxis *>(object);

		if (axis)
			scanAxes_.append(axis);
	}
}

void AMContinuousScanConfiguration::insertScanAxis(int index, AMScanAxis *newAxis)
{
	scanAxes_.insert(index, newAxis);
	emit scanAxisAdded(newAxis);
}

void AMContinuousScanConfiguration::appendScanAxis(AMScanAxis *newAxis)
{
	scanAxes_.append(newAxis);
	emit scanAxisAdded(newAxis);
}

bool AMContinuousScanConfiguration::removeScanAxis(AMScanAxis *axis)
{
	int index = -1;

	for (int i = 0, size = scanAxes_.count(); i < size; i++)
		if (scanAxes_.at(i) == axis)
			index = i;

	if (index != -1){

		AMScanAxis *axis = scanAxes_.takeAt(index);
		emit scanAxisRemoved(axis);
		return true;
	}

	return false;
}

AMScanAxis *AMContinuousScanConfiguration::removeScanAxis(int index)
{
	AMScanAxis *scanAxis = scanAxes_.takeAt(index);
	emit scanAxisRemoved(scanAxis);
	return scanAxis;
}
