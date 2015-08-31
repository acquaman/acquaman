#ifndef REIXS_H
#define REIXS_H

#include <QString>
#include <QList>

#include "dataman/export/AMExporterOptionSMAK.h"
#include "dataman/database/AMDbObjectSupport.h"

namespace REIXS {

	/// Builds the standard exporter option used for all exported scans.
	inline AMExporterOptionGeneralAscii *buildStandardExporterOption(const QString &name, bool includeHigherOrderSources)
	{
		QList<int> matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionGeneralAscii>(), "name", name);

		AMExporterOptionGeneralAscii *exporterOption = new AMExporterOptionGeneralAscii();

		if (matchIDs.count() != 0)
			exporterOption->loadFromDb(AMDatabase::database("user"), matchIDs.at(0));

		exporterOption->setName(name);
		exporterOption->setFileName("$name_$number.dat");
		exporterOption->setHeaderText("#Scan: $name #$number\n#DB Serial: $serial\n#Date: $dateTime\n#Sample: $sample\n#\n#User Energy Offest: $control[userEnergyOffset]\n#sampleChamber x,y,z,r: $control[sampleX], $control[sampleY], $control[sampleZ], $control[sampleTheta]\n#Grating Mask Position: $control[gratingMask]\n#Spectrometer Grating: $control[spectrometerGrating]\n#Spectrometer Energy: $control[spectrometer]\n#Hexapod x,y,z: $control[hexapodX],$control[hexapodZ],$control[hexapodZ]\n#Hexapod u,v,w: $control[hexapodU],$control[hexapodV],$control[hexapodW]\n#Hexapod r,s,t: $control[hexapodR],$control[hexapodS],$control[hexapodT]\n#Spectrometer Rotation Drive: $control[spectrometerRotationDrive]\n#Detector Translation: $control[detectorTranslation]\n#Detector Tilt Drive: $control[detectorTiltDrive]\n#Endstation Position: $control[endstationTranslation]\n#Mono Mirror Selection, Translation: $control[monoMirrorSelector], $control[monoMirrorTranslation]\n#Mono Grating Selection, Translation: $control[monoGratingSelector], $control[monoGratingTranslation]\n#M5 Pitch: $control[M5Pitch]\n#M5 Yaw: $control[M5Yaw]\n#EPU polarization mode,angle: $control[epuPolarization], $control[epuPolarizationAngle]\n#Room Temp: $control[SOETemp]\n#Preamp Temp: $control[MCPPreampTemp]\n#Bealine Energy: $control[beamlineEV]\n#Ring Current: $control[ringCurrent]\n#Mesh Current: $control[I0Current]\n#Sample Current:  $control[TEYCurrent]\n#\n#\n#$notes\n#Note that I0.X is the energy feedback.\n\n");
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
		exporterOption->setSeparateSectionFileName("$name_$dataSetName_$number.dat");
		exporterOption->setHigherDimensionsInRows(true);
		exporterOption->storeToDb(AMDatabase::database("user"));

		return exporterOption;
	}

}

#endif // REIXS_H
