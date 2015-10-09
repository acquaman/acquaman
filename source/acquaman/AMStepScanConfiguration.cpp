/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
		scanAxes_.at(i)->deleteLater();

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
	for (int i = 0, size = scanAxes_.count(); i < size; i++)
		scanAxes_.at(i)->deleteLater();

	scanAxes_.clear();

	foreach (AMDbObject *object, newScanAxes){

		AMScanAxis *axis = qobject_cast<AMScanAxis *>(object);

		if (axis)
			scanAxes_.append(axis);
	}
}

void AMStepScanConfiguration::merge(AMStepScanConfiguration *configuration)
{
	if (configuration) {

		AMScanConfiguration::merge(configuration);

		foreach (AMScanAxis *axis, configuration->scanAxes())
			scanAxes_.append(axis->createCopy());
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
