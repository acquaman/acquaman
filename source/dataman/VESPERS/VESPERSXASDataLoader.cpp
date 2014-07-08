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

#include "VESPERSXASDataLoader.h"

#include "util/AMErrorMonitor.h"
#include "analysis/AM1DExpressionAB.h"

#include <QFile>
#include <QTextStream>

 VESPERSXASDataLoader::~VESPERSXASDataLoader(){}
VESPERSXASDataLoader::VESPERSXASDataLoader(AMXASScan *scan)
	: AMAbstractFileLoader(scan)
{
}

bool VESPERSXASDataLoader::loadFromFile(const QString &filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks)
{
	// Currently don't have meta data.
	Q_UNUSED(setMetaData)
	Q_UNUSED(setRawDataSources)
	Q_UNUSED(createDefaultAnalysisBlocks)

	AMXASScan *scan = qobject_cast<AMXASScan *>(scan_);

	if (!scan){

		AMErrorMon::alert(0, 0, "VESPERS XAS File Loader: Could not load XAS data into a non-XAS scan.");
		return false;
	}

	// Clear the old scan axes to ensure we don't have any extras.
	scan->clearRawDataCompletely();
	scan->rawData()->addScanAxis( AMAxisInfo("eV", 0, "Incident Energy", "eV") );

	QFile file(filepath);
	if(!file.open(QIODevice::ReadOnly)) {
		AMErrorMon::error(0, -1, "XASFileLoader parse error while loading scan data from file.");
		return false;
	}

	QFile spectra;
	QVector<int> data;

	QTextStream in(&file);
	QString line;
	QStringList lineTokenized;

	// Need to determine if the single element or four element vortex detector was used.  Also need to determine which ion chambers were used for I0 and It.
	// First two lines are useless.
	line = in.readLine();
	line = in.readLine();

	line = in.readLine();

	if (line.contains("#(2)"))
		line = in.readLine();

	bool usingVortex = line.contains("IOC1607-004") || line.contains("dxp1607-B21-04");

	if (usingVortex){

		data.resize(2048);
		QString temp(filepath);
		temp.chop(4);
		spectra.setFileName(temp+"_spectra.dat");

		if(!spectra.open(QIODevice::ReadOnly)) {
			AMErrorMon::error(0, -1, QString("XASFileLoader parse error while loading scan spectra data from %1.").arg(spectra.fileName()));
			return false;
		}
	}
	else
		spectra.setFileName("");

	QTextStream spectraStream(&spectra);
	QString spectraLine;
	QStringList spectraTokenized;

	while ((line = in.readLine()).contains("#")){
		//Do nothing
	}

	// Clear any old data so we can start fresh.
	scan->clearRawDataPointsAndMeasurements();

	// Some setup variables.
	int axisValueIndex = 0;

	if (usingVortex){

		for (int i = 0; i < scan_->rawDataSourceCount()-1; i++)
			scan_->rawData()->addMeasurement(AMMeasurementInfo(scan_->rawDataSources()->at(i)->name(), scan_->rawDataSources()->at(i)->description()));

		QList<AMAxisInfo> axisInfo;
		AMAxisInfo ai("Energy", 2048, "Energy", "eV");
		ai.increment = 10;
		ai.start = AMNumber(0);
		ai.isUniform = true;
		axisInfo << ai;

		scan_->rawData()->addMeasurement(AMMeasurementInfo(scan_->rawDataSources()->at(scan_->rawDataSourceCount()-1)->name(), scan_->rawDataSources()->at(scan_->rawDataSourceCount()-1)->description(), "eV", axisInfo));
	}

	else{

		for (int i = 0; i < scan_->rawDataSourceCount(); i++)
			scan_->rawData()->addMeasurement(AMMeasurementInfo(scan_->rawDataSources()->at(i)->name(), scan_->rawDataSources()->at(i)->description()));

	}
	while (!in.atEnd()){

		lineTokenized << line.split(", ");

		scan->rawData()->beginInsertRows(1, -1);

		scan_->rawData()->setAxisValue(0, axisValueIndex, lineTokenized.at(1).toDouble());

		// This isn't the most efficient way of putting the spectra data in, but it will do for the time being.
		if (usingVortex){

			// Only going to rawDataSourceCount-1 because the last raw data source is the 2D spectra scan and requires its own method of entering the data.
			for (int i = 0; i < scan_->rawDataSourceCount()-1; i++)
				scan_->rawData()->setValue(axisValueIndex, i, AMnDIndex(), lineTokenized.at(i+2).toDouble());

			spectraTokenized.clear();
			spectraLine = spectraStream.readLine();
			spectraTokenized << spectraLine.split(",");

			for (int j = 0; j < 2048; j++)
				data[j] = spectraTokenized.at(j).toInt();

			scan_->rawData()->setValue(axisValueIndex, scan_->rawDataSourceCount()-1, data.constData());
		}

		else{

			// In transmission, there is no 2D spectra.  Go through all the data sources.
			for (int i = 0; i < scan_->rawDataSourceCount(); i++)
				scan_->rawData()->setValue(axisValueIndex, i, AMnDIndex(), lineTokenized.at(i+2).toDouble());
		}

		scan->rawData()->endInsertRows();

		axisValueIndex++;
		line = in.readLine();
		lineTokenized.clear();
	}

	file.close();

	if (usingVortex)
		spectra.close();

	return true;
}
