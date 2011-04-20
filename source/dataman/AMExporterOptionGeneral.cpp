#include "AMExporterOptionGeneral.h"

AMExporterOptionGeneral::AMExporterOptionGeneral(QObject *parent) :
	AMExporterOption(parent)
{

	includeAllDataSources_ = true;

	headerText_ =	"Scan: $name #$number\n"
					"Date: $dateTime\n"
					"Sample: $sample\n"
					"Facility: $facilityDescription\n\n";
	headerIncluded_ = true;

	columnHeader_ =	"$dataSourceName ($dataSourceUnits)";
	columnHeaderIncluded_ = true;

	columnHeaderDelimiter_ = "==========";

	sectionHeader_ = "\n$dataSourceName: $dataSourceDescription ($dataSourceUnits)\nSize: $dataSourceSize\n"
						"----------";
	sectionHeaderIncluded_ = true;

	separateSectionFileName_ = "$name_$number_$dataSource_$dateTime.dat";
}

