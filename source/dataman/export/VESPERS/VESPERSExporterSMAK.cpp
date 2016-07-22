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


#include "VESPERSExporterSMAK.h"

#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/AMScan.h"
#include "util/AMErrorMonitor.h"
#include "acquaman/VESPERS/VESPERS2DScanConfiguration.h"

#include <QStringBuilder>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

VESPERSExporterSMAK::~VESPERSExporterSMAK(){}

VESPERSExporterSMAK::VESPERSExporterSMAK(QObject *parent)
	: AMSMAKExporter(parent)
{
}

void VESPERSExporterSMAK::writeMainTable()
{
	QTextStream ts(file_);

	// 1. Column header.
	if(option_->columnHeaderIncluded()) {

		ts << "# ";

		for(int c=0; c<mainTableDataSources_.count(); c++) {

			setCurrentDataSource(mainTableDataSources_.at(c));

			if(mainTableIncludeX_.at(c))
				ts << "H" << option_->columnDelimiter() << "V" << option_->columnDelimiter();

			ts << parseKeywordString(option_->columnHeader()) << option_->columnDelimiter();
		}
	}

	ts << option_->newlineDelimiter() << option_->columnHeaderDelimiter() << option_->newlineDelimiter();

	// 2. rows
	VESPERS2DScanConfiguration *config = qobject_cast<VESPERS2DScanConfiguration *>(const_cast<AMScanConfiguration *>(currentScan_->scanConfiguration()));
	if (!config)
		return;

	QString suffix;

	if (config->ccdDetector().testFlag(VESPERS::Roper))
		suffix = "spe";

	else if (config->ccdDetector().testFlag(VESPERS::Mar))
		suffix = "tif";

	else if (config->ccdDetector().testFlag(VESPERS::Pilatus))
		suffix = "tif";

	QString ccdFileName = config->ccdFileName();
	int yRange = yRange_ == -1 ? currentScan_->scanSize(1) : (yRange_-1);
	int xRange = currentScan_->scanSize(0);

	for(int y = 0; y < yRange; y++) {

		for (int x = 0; x < xRange; x++){

			// over rows within columns
			for(int c=0; c<mainTableDataSources_.count(); c++) {

				setCurrentDataSource(mainTableDataSources_.at(c));
				AMDataSource* ds = currentScan_->dataSourceAt(currentDataSourceIndex_);
				int precision = option_->exportPrecision(ds->name());

				// print x and y column?
				if(mainTableIncludeX_.at(c)) {

					ts << ds->axisValue(0,x).toString(precision);
					ts << option_->columnDelimiter();
					ts << ds->axisValue(1, y).toString(precision);
					ts << option_->columnDelimiter();
				}

				if(ds->name().contains("FileNumber"))
					ts << QString("%1_%2.%3").arg(ccdFileName).arg(int(ds->value(AMnDIndex(x, y)))-1).arg(suffix);	// The -1 is because the value stored here is the NEXT number in the scan.  Purely a nomenclature setup from the EPICS interface.

				else
					ts << ds->value(AMnDIndex(x, y)).toString(precision);

				ts << option_->columnDelimiter();
			}

			ts << option_->newlineDelimiter();
		}
	}

	if (yRange_ != -1 && xIndex_ != -1){

		for (int x = 0; x < xIndex_; x++){

			// over rows within columns
			for(int c=0; c<mainTableDataSources_.count(); c++) {

				setCurrentDataSource(mainTableDataSources_.at(c));
				AMDataSource* ds = currentScan_->dataSourceAt(currentDataSourceIndex_);
				int precision = option_->exportPrecision(ds->name());

				// print x and y column?
				if(mainTableIncludeX_.at(c)) {

					ts << ds->axisValue(0,x).toString(precision);
					ts << option_->columnDelimiter();
					ts << ds->axisValue(1, yRange_-1).toString(precision);
					ts << option_->columnDelimiter();
				}

				if(ds->name().contains("FileNumber"))
					ts << QString("%1_%2.%3").arg(ccdFileName).arg(int(ds->value(AMnDIndex(x, yRange_-1)))-1).arg(suffix);	// The -1 is because the value stored here is the NEXT number in the scan.  Purely a nomenclature setup from the EPICS interface.

				else
					ts << ds->value(AMnDIndex(x, yRange_-1)).toString(precision);

				ts << option_->columnDelimiter();
			}


			ts << option_->newlineDelimiter();
		}
	}


	ts << option_->newlineDelimiter();
}
