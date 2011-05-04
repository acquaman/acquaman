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

	columnHeader_ =	"$dataSet";	/// \todo Removed $dataSetUnits because we don't have that implemented yet.
	columnHeaderIncluded_ = true;

	columnHeaderDelimiter_ = "==========";

	sectionHeader_ = "\n$dataSetName: $dataSetDescription\nSize: $dataSetSize\n"		/// \todo Removed $dataSetUnits because we don't have that implemented yet.
						"----------";
	sectionHeaderIncluded_ = true;

	separateSectionFileName_ = "$name_$number_$dataSet_$dateTime[yyyyMMdd_hhmmss].dat";
}

