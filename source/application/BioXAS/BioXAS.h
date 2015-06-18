#ifndef BIOXAS_H
#define BIOXAS_H

#include "dataman/export/AMExporterOptionXDIFormat.h"
#include "dataman/database/AMDbObjectSupport.h"

namespace BioXAS
{
	/// Builds the standard exporter option used for all exported scans.
	inline AMExporterOptionXDIFormat *buildStandardXDIFormatExporterOption(const QString &name, const QString &elementSymbol, const QString &elementEdge, bool includeHigherOrderSources)
	{
		QList<int> matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionXDIFormat>(), "name", name);

		AMExporterOptionXDIFormat *xasDefault = new AMExporterOptionXDIFormat();

		if (matchIDs.count() != 0)
			xasDefault->loadFromDb(AMDatabase::database("user"), matchIDs.at(0));

		xasDefault->setName(name);
		xasDefault->setFileName("$name_$fsIndex.dat");
		xasDefault->setHeaderText("Scan: $name #$number\nDate: $dateTime\nSample: $sample\nFacility: $facilityDescription\n\n$scanConfiguration[header]\n\n$notes\n\n");
		xasDefault->setHeaderIncluded(true);
		xasDefault->setColumnHeader("$dataSetName $dataSetInfoDescription");
		xasDefault->setColumnHeaderIncluded(true);
		xasDefault->setColumnHeaderDelimiter("");
		xasDefault->setSectionHeader("");
		xasDefault->setSectionHeaderIncluded(true);
		xasDefault->setIncludeAllDataSources(true);
		xasDefault->setFirstColumnOnly(true);
		xasDefault->setIncludeHigherDimensionSources(includeHigherOrderSources);
		xasDefault->setSeparateHigherDimensionalSources(true);
		xasDefault->setSeparateSectionFileName("$name_$dataSetName_$fsIndex.dat");
		xasDefault->setElementSymbol(elementSymbol);
		xasDefault->setElementEdge(elementEdge);
		xasDefault->storeToDb(AMDatabase::database("user"));

		return xasDefault;
	}
}

#endif // BIOXAS_H
