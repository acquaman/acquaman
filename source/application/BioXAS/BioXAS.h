#ifndef BIOXAS_H
#define BIOXAS_H

#include "dataman/export/AMExporterOptionXDIFormat.h"
#include "dataman/database/AMDbObjectSupport.h"

namespace BioXAS
{
	/// Handles the XRF detector choice.  Available choices are None, KETEK, and 13-element Ge.
	enum FluorescenceDetector {
		NoneXRFDetector = 0,
		FourElementVortexDetector = 1,
		Ge13ElementDetector = 2
	};

	/// Builds the standard exporter option used for all exported scans.
	inline AMExporterOptionXDIFormat *buildStandardXDIFormatExporterOption(const QString &name, const QString &elementSymbol, const QString &elementEdge, bool includeHigherOrderSources)
	{
		QList<int> matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionXDIFormat>(), "name", name);

		AMExporterOptionXDIFormat *xasDefault = new AMExporterOptionXDIFormat();

		if (matchIDs.count() != 0)
			xasDefault->loadFromDb(AMDatabase::database("user"), matchIDs.at(0));

		xasDefault->setName(name);
		xasDefault->setFileName("$name_$number.dat");
		xasDefault->setHeaderText("Scan: $name #$number\nDate: $dateTime\nFacility: $facilityDescription\n\n$scanConfiguration[header]\n\n$notes\n\n");
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
		xasDefault->setSeparateSectionFileName("$name_$dataSetName_$number.dat");
		xasDefault->setElementSymbol(elementSymbol);
		xasDefault->setElementEdge(elementEdge);
		xasDefault->storeToDb(AMDatabase::database("user"));

		return xasDefault;
	}
}

#endif // BIOXAS_H
