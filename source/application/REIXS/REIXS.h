#ifndef REIXS_H
#define REIXS_H

#include <QString>
#include <QList>

#include "dataman/export/AMExporterOptionSMAK.h"
#include "dataman/database/AMDbObjectSupport.h"

namespace REIXS {

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

#endif // REIXS_H
