#ifndef PGM
#define PGM

#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExporterOptionXDIFormat.h"

namespace PGM
{
	/// Enumeration of the available XRF detectors.
	enum FluorescenceDetector {
		NoneXRFDetector = 0,
		OceanOpticsDetector = 1
        };

        inline AMExporterOptionXDIFormat *buildXDIFormatExporterOption(const QString &name, bool includeHigherOrderSources)
        {
            QList<int> matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionXDIFormat>(), "name", name);

            AMExporterOptionXDIFormat *option = new AMExporterOptionXDIFormat();

            if(matchIDs.count() != 0)
                option->loadFromDb(AMDatabase::database("user"), matchIDs.at(0));

            option->setName(name);
            option->setFileName("$name_$number.dat");
            option->setHeaderText("Scan: $name #$number\nDate: $dateTime\nFacility: $facilityDescription\n\n$scanConfiguration[header]\n\n$notes\n\n");
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

#endif // PGM_H

