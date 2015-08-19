#ifndef IDEAS_H
#define IDEAS_H

#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterOptionSMAK.h"

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
	inline AMExporterOptionGeneralAscii *buildStandardExporterOption(const QString &name, bool includeHigherOrderSources, bool exportSpectraInRows)
	{
		QList<int> matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionGeneralAscii>(), "name", name);

		AMExporterOptionGeneralAscii *exporterOption = new AMExporterOptionGeneralAscii();

		if (matchIDs.count() != 0)
			exporterOption->loadFromDb(AMDatabase::database("user"), matchIDs.at(0));

		exporterOption->setName(name);
		exporterOption->setFileName("$name_$fsIndex.dat");
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

	/// Builds the SMAK exporter option used for all exported scans.
	inline AMExporterOptionSMAK *buildSMAKExporterOption(const QString &name, bool includeHigherOrderSources, bool exportSpectraInRows)
	{
		QList<int> matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionSMAK>(), "name", name);

		AMExporterOptionSMAK *exporterOption = new AMExporterOptionSMAK();

		if (matchIDs.count() != 0)
			exporterOption->loadFromDb(AMDatabase::database("user"), matchIDs.at(0));

		exporterOption->setName(name);
		exporterOption->setFileName("$name_$fsIndex.dat");
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
		exporterOption->setRegExpString("^\\w{1,2}Ka1|^\\w{1,2}Kb1|^\\w{1,2}La1|^\\w{1,2}Lb1|^\\w{1,2}Lg1|I_0");
		exporterOption->storeToDb(AMDatabase::database("user"));

		return exporterOption;
	}
}

Q_DECLARE_OPERATORS_FOR_FLAGS(IDEAS::FluorescenceDetectors)

#endif // IDEAS_H
