#include "AMStepScanConfiguration.h"

AMStepScanConfiguration::AMStepScanConfiguration(QObject *parent)
	: AMScanConfiguration(parent)
{
}

AMStepScanConfiguration::AMStepScanConfiguration(const AMStepScanConfiguration &original)
	: AMScanConfiguration(original)
{
	foreach (AMScanAxis *axis, original.scanAxes())
		scanAxes_.append(axis->createCopy());
}

AMStepScanConfiguration::~AMStepScanConfiguration()
{
	for (int i = 0, size = scanAxes_.count(); i < size; i++)
		delete scanAxes_.at(i);

	scanAxes_.clear();
}

AMDbObjectList AMStepScanConfiguration::dbReadScanAxes()
{
	AMDbObjectList list;

	foreach (AMScanAxis *axis, scanAxes_.toList())
		list << axis;

	return list;
}

void AMStepScanConfiguration::dbLoadScanAxes(const AMDbObjectList &newScanAxes)
{
	scanAxes_.clear();

	foreach (AMDbObject *object, newScanAxes){

		AMScanAxis *axis = qobject_cast<AMScanAxis *>(object);

		if (axis)
			scanAxes_.append(axis);
	}
}

void AMStepScanConfiguration::insertScanAxis(int index, AMScanAxis *newAxis)
{
	scanAxes_.insert(index, newAxis);
	emit scanAxisAdded(newAxis);
}

void AMStepScanConfiguration::appendScanAxis(AMScanAxis *newAxis)
{
	scanAxes_.append(newAxis);
	emit scanAxisAdded(newAxis);
}

bool AMStepScanConfiguration::removeScanAxis(AMScanAxis *axis)
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

AMScanAxis *AMStepScanConfiguration::removeScanAxis(int index)
{
	AMScanAxis *scanAxis = scanAxes_.takeAt(index);
	emit scanAxisRemoved(scanAxis);
	return scanAxis;
}
