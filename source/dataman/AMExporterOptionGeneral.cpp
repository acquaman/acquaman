#include "AMExporterOptionGeneral.h"

AMExporterOptionGeneral::AMExporterOptionGeneral(QObject *parent) :
	AMExporterOption(parent)
{

	headerText_ =	"Scan:\t$name #$number\n"
					"Date:\t$dateTime\n"
					"Sample:\t$sample\n"
					"Facility:\t$facilityDescription\n\n";

	columnHeader_ =	"$dataSourceName ($dataSourceUnits)";

	isColumnHeaderIncluded_ = true;

	columnHeaderDelimiter_ = "==========";

	sectionDelimiter_ = "\n$dataSourceName: $dataSourceDescription ($dataSourceUnits)\nSize: $dataSourceSize\n"
						"----------";
}

