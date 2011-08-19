/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "SGM2010FastSensibleFileLoader.h"
#include <QDir>

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "dataman/AMFastScan.h"
#include "util/AMErrorMonitor.h"
#include "analysis/AM1DExpressionAB.h"

SGM2010FastSensibleFileLoader::SGM2010FastSensibleFileLoader(AMFastScan *scan) : AMAbstractFileLoader(scan)
{
	defaultUserVisibleColumns_ << "tey";
	defaultUserVisibleColumns_ << "tfy";
	defaultUserVisibleColumns_ << "i0";
	defaultUserVisibleColumns_ << "photodiode";
}

bool SGM2010FastSensibleFileLoader::loadFromFile(const QString &filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks){
	Q_UNUSED(filepath)
	Q_UNUSED(setMetaData)
	Q_UNUSED(setRawDataSources)
	Q_UNUSED(createDefaultAnalysisBlocks)
	return false;
}

bool SGM2010FastSensibleFileLoader::saveToFile(const QString &filepath){
	AMFastScan* scan = qobject_cast<AMFastScan*>(scan_);
	if(!scan)
		return false;

	QFile f(filepath);
	if(!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -1, "SGM2010FastSensibleFileLoader write error while creating file. Could not create file."));
		return false;
	}
	QTextStream fs(&f);

	fs << "Energy TEY TFY I0 Photodiode\n";
	for(int x = 0; x < scan->rawData()->scanSize(0); x++){
		fs << (double)(scan->rawData()->axisValue(0, x)) << " "
				<< (int)(scan->rawData()->value(AMnDIndex(x), 0, AMnDIndex())) << " "
				<< (int)(scan->rawData()->value(AMnDIndex(x), 1, AMnDIndex())) << " "
				<< (int)(scan->rawData()->value(AMnDIndex(x), 2, AMnDIndex())) << " "
				<< (int)(scan->rawData()->value(AMnDIndex(x), 3, AMnDIndex())) << "\n";
	}

	return true;
}
