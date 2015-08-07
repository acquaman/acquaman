#ifndef IDEAS_H
#define IDEAS_H

#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"

namespace IDEAS
{
	#define ERR_IDEAS_XRF_DETECTOR_NOT_CONNECTED 590201 //XRF detector not initialized
	#define ERR_IDEAS_XRF_DETECTOR_SCAN_NOT_EXIST 590202 //XRF scan does not exist

	/// Handles the XRF detector choice.  Available choices are None, KETEK, and 13-element Ge.
	enum FluorescenceDetector {

		NoXRF = 0,
		Ketek = 1,
		Ge13Element = 2
	};
	Q_DECLARE_FLAGS(FluorescenceDetectors, FluorescenceDetector)

	/// Builds the standard exporter option used for all exported scans.
	inline AMExporterOptionGeneralAscii *buildStandardExporterOption(const QString &name, bool includeHigherOrderSources, bool hasGotoPosition, bool addeVFeedbackMessage, bool exportSpectraInRows)
	{
		QList<int> matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionGeneralAscii>(), "name", name);

		AMExporterOptionGeneralAscii *exporterOption = new AMExporterOptionGeneralAscii();

		if (matchIDs.count() != 0)
			exporterOption->loadFromDb(AMDatabase::database("user"), matchIDs.at(0));

		exporterOption->setName(name);
		exporterOption->setFileName("$name_$fsIndex.dat");
		if (hasGotoPosition && addeVFeedbackMessage)
			exporterOption->setHeaderText("Scan: $name #$number\nDate: $dateTime\nSample: $sample\nFacility: $facilityDescription\n\n$scanConfiguration[header]\nActual Horizontal Position:\t$controlValue[Horizontal Sample Stage] mm\nActual Vertical Position:\t$controlValue[Vertical Sample Stage] mm\n\n$notes\nNote that I0.X is the energy feedback.\n\n");
		else if (hasGotoPosition)
			exporterOption->setHeaderText("Scan: $name #$number\nDate: $dateTime\nSample: $sample\nFacility: $facilityDescription\n\n$scanConfiguration[header]\nActual Horizontal Position:\t$controlValue[Horizontal Sample Stage] mm\n\n$notes\n\n");
		else
			exporterOption->setHeaderText("Scan: $name #$number\nDate: $dateTime\nSample: $sample\nFacility: $facilityDescription\n\n$scanConfiguration[header]\n\n$notes\n\n");
		exporterOption->setHeaderIncluded(true);
		exporterOption->setColumnHeader("$dataSetName $dataSetInfoDescription");
		exporterOption->setColumnHeaderIncluded(true);
		exporterOption->setColumnHeaderDelimiter("");
		exporterOption->setSectionHeader("");
		exporterOption->setSectionHeaderIncluded(true);
		exporterOption->setIncludeAllDataSources(true);
		exporterOption->setFirstColumnOnly(true);
		exporterOption->setIncludeHigherDimensionSources(includeHigherOrderSources);
		exporterOption->setSeparateHigherDimensionalSources(true);
		exporterOption->setSeparateSectionFileName("$name_$dataSetName_$fsIndex.dat");
		exporterOption->setHigherDimensionsInRows(exportSpectraInRows);
		exporterOption->storeToDb(AMDatabase::database("user"));

		return exporterOption;
	}
}

Q_DECLARE_OPERATORS_FOR_FLAGS(IDEAS::FluorescenceDetectors)

#endif // IDEAS_H
