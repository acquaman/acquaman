#include "AMExporterGeneralAscii.h"
#include "AMExporterOptionGeneralAscii.h"
#include "dataman/AMScan.h"
#include <QStringBuilder>

#include "util/AMErrorMonitor.h"

AMExporterGeneralAscii::AMExporterGeneralAscii(QObject *parent) :
	AMExporter(parent)
{
}

AMExporterOption * AMExporterGeneralAscii::createDefaultOption() const {
	return new AMExporterOptionGeneralAscii();
}

bool AMExporterGeneralAscii::isValidFor(const AMScan *scan, const AMExporterOption *iOption) const
{
	// wrong
	const AMExporterOptionGeneralAscii* option = qobject_cast<const AMExporterOptionGeneralAscii*>(iOption);
	if(!option) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -2, "Invalid options specified for the General Ascii Exporter. Please report this bug to the Acquaman developers."));
		return false;
	}

	// missing a required data source
	for(int i=option->dataSources().count()-1; i>=0; i--) {
		if(option->isDataSourceRequired(i) &&
				scan->indexOfDataSource(option->dataSources().at(i)) == -1) {

			AMErrorMon::report(AMErrorReport(this,
											 AMErrorReport::Alert,
											 -3,
											 "Export failed: The scan '" % scan->fullName() % "' does not contain the required data set '" % option->dataSources().at(i) % "'."));
			return false;
		}
	}

	return true;
}

QString AMExporterGeneralAscii::exportScan(const AMScan *scan, const QString &destinationFolderPath, const AMExporterOption *option) const
{
	return QString();
}
