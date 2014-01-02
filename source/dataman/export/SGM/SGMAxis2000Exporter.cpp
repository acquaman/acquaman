#include "SGMAxis2000Exporter.h"

#include <QTextStream>
#include <QFile>

#include "dataman/AMScan.h"
#include "dataman/export/AMExporterOption.h"
#include "acquaman/AMXASScanConfiguration.h"

 SGMAxis2000Exporter::~SGMAxis2000Exporter(){}
SGMAxis2000Exporter::SGMAxis2000Exporter(QObject *parent) :
	AMExporterGeneralAscii(parent)
{
}

QString SGMAxis2000Exporter::exportScan(const AMScan *scan, const QString &destinationFolderPath, const AMExporterOption *option, int autoIndex){

	setCurrentAutoIndex(autoIndex);
	setCurrentFilename(option->fileName());
	setDestinationFolderPath(destinationFolderPath);

	setCurrentScan(scan);

	mainFileName_ = parseKeywordString(QString("%1/%2").arg(destinationFolderPath).arg(option->fileName()));

	qDebug() << option->fileName() << destinationFolderPath;

	if(!openFile(mainFileName_))
		return QString();

	QTextStream ts(file_);
	int eRange = currentScan_->scanSize(0);

	ts << "ScanDefinition = { ";
	ts << QString("Label = \"%1\"; ").arg(mainFileName_);
	ts << "\"NEXAFS Point Scan\"; ";
	ts << "Flags = \"Spectra\"; ";
	ts << "ScanType = \"Spectra\"; ";
	const AMXASScanConfiguration *xasScanConfiguration = qobject_cast<const AMXASScanConfiguration*>(currentScan_->scanConfiguration());
	ts << QString("Dwell = %1;").arg(xasScanConfiguration->regionTime(0));
	ts << "\n\tRegions = (1,";
	ts << "\n{";
	ts << "\n\t\t\tPAxis = { ";
	ts << "Name = \"Energy\"; ";
	ts << "Unit = \"eV\"; ";
	ts << QString("Min = %1; ").arg((double)currentScan_->rawDataSources()->at(0)->axisValue(0, 0));
	ts << QString("Max = %1; ").arg((double)currentScan_->rawDataSources()->at(0)->axisValue(0, eRange-1));
	ts << "Dir = 1;";
	ts << "\n\t\t\t\tPoints = (";
	ts << QString("%1, ").arg(eRange);
	for(int x = 0; x < eRange-1; x++)
		ts << QString("%1, ").arg((double)currentScan_->rawDataSources()->at(0)->axisValue(0, x));
	ts << QString("%1").arg((double)currentScan_->rawDataSources()->at(0)->axisValue(0, eRange-1));
	ts << ");";
	ts << "\n};";
	ts << "\n});";

	file_->close();

	return mainFileName_;
}
