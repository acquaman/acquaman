#ifndef SGM
#define SGM

#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExporterOptionXDIFormat.h"

namespace SGM
{
	/// Builds the standard exporter option used for all exported scans.
	inline AMExporterOptionXDIFormat *buildXDIFormatExporterOption(const QString &name, bool includeHigherOrderSources)
	{
		QList<int> matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionXDIFormat>(), "name", name);

		AMExporterOptionXDIFormat *option = new AMExporterOptionXDIFormat();

		if (matchIDs.count() != 0)
			option->loadFromDb(AMDatabase::database("user"), matchIDs.at(0));

		option->setName(name);
		option->setFileName("$name_$number.dat");
		option->setHeaderText("Scan: $name #$number\nDate: $dateTime\nSample: $sample\nFacility: $facilityDescription\n\n$scanConfiguration[header]\n\n$notes\n\n");
		option->setHeaderIncluded(true);
		option->setColumnHeader("$dataSetName $dataSetInfoDescription");
		option->setColumnHeaderIncluded(true);
		option->setColumnHeaderDelimiter("");
		option->setSectionHeader("");
		option->setSectionHeaderIncluded(true);
		option->setIncludeAllDataSources(true);
		option->setFirstColumnOnly(true);
		option->setIncludeHigherDimensionSources(includeHigherOrderSources);
		option->setSeparateHigherDimensionalSources(true);
		option->setSeparateSectionFileName("$name_$dataSetName_$number.dat");
		option->setHigherDimensionsInRows(true);
		option->storeToDb(AMDatabase::database("user"));

		return option;
	}

	/// Builds the standard exporter option used for all exported scans.
	inline AMExporterOptionGeneralAscii *buildAsciiFormatExporterOption(const QString &name, bool includeHigherOrderSources)
	{
		QList<int> matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionGeneralAscii>(), "name", name);

		AMExporterOptionGeneralAscii *option = new AMExporterOptionGeneralAscii();

		if (matchIDs.count() != 0)
			option->loadFromDb(AMDatabase::database("user"), matchIDs.at(0));

		option->setName(name);
		option->setFileName("$name_$number.dat");
		option->setHeaderText("Scan: $name #$number\nDate: $dateTime\nSample: $sample\nFacility: $facilityDescription\n\n$scanConfiguration[header]\n\n$notes\n\n");
		option->setHeaderIncluded(true);
		option->setColumnHeader("$dataSetName $dataSetInfoDescription");
		option->setColumnHeaderIncluded(true);
		option->setColumnHeaderDelimiter("");
		option->setSectionHeader("");
		option->setSectionHeaderIncluded(true);
		option->setIncludeAllDataSources(true);
		option->setFirstColumnOnly(true);
		option->setIncludeHigherDimensionSources(includeHigherOrderSources);
		option->setSeparateHigherDimensionalSources(true);
		option->setSeparateSectionFileName("$name_$dataSetName_$number.dat");
		option->setHigherDimensionsInRows(true);
		option->storeToDb(AMDatabase::database("user"));

		return option;
	}
}

#endif // SGM

