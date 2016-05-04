#ifndef SXRMB_H
#define SXRMB_H

/*!
	This namespace contains enums that are used throughout the SXRMB application.  It is meant to consolidate them all
	in one location to minimize duplicated code.  Some of these enums are declared many times throughout the program
	and various classes.
  */

#include <QString>
#include <QPair>
#include <QDir>
#include <QList>
#include <QStringBuilder>

#include "dataman/export/AMExporterOptionSMAK.h"
#include "dataman/database/AMDbObjectSupport.h"


namespace SXRMB {

	#define ERR_SXRMB_SHUTTERS_TIMEOUT 290101
	#define ERR_SXRMB_XRF_DETECTOR_NOT_CONNECTED 290201 //XRF detector not initialized
	#define ERR_SXRMB_XRF_DETECTOR_SCAN_NOT_EXIST 290202 //XRF detector failed to create scan for exporting

	/// Enum for the different endstations.
	/*!
	  - Solid State is the in-vacuum XAS endstation.
	  - Ambiant is the ambiant XAS endstation.
	  - Microprobe is for the microprobe mapping and XAS endstation.
	  */
	enum Endstation
	{
		UnkownEndstation = 0,
		SolidState,
		AmbiantWithGasChamber,
		AmbiantWithoutGasChamber,
		Microprobe
	};

	/// Enum for making the decision on what fluorescence detector the user wants to use.
	enum FluorescenceDetector
	{
		NoXRFDetector = 0,
		BrukerDetector = 1,
		FourElementDetector = 2
	};
	Q_DECLARE_FLAGS(FluorescenceDetectors, FluorescenceDetector)

	/// Helper to convert SXRMB Endstation ID to endstation Name
	inline QString sxrmbEndstationName(Endstation id) {
		QString name;

		switch (id)
		{
		case SolidState:
			name = "Solid State";
			break;

		case AmbiantWithGasChamber:
			name = "Ambiant with Gas Chamber";
			break;

		case AmbiantWithoutGasChamber:
			name = "Ambiant without Gas Chamber";
			break;

		case Microprobe:
			name = "Microprobe";
			break;

		default:
			name = "Invalid Endstation ID";
		}

		return name;
	}

	/// Builds the standard exporter option used for all exported scans.
	inline AMExporterOptionGeneralAscii *buildStandardExporterOption(const QString &name, bool includeHigherOrderSources, bool hasGotoPosition, bool addeVFeedbackMessage, bool exportSpectraInRows)
	{
		QList<int> matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionGeneralAscii>(), "name", name);

		AMExporterOptionGeneralAscii *sxrmbExporterOption = new AMExporterOptionGeneralAscii();

		if (matchIDs.count() != 0)
			sxrmbExporterOption->loadFromDb(AMDatabase::database("user"), matchIDs.at(0));

		sxrmbExporterOption->setName(name);
		sxrmbExporterOption->setFileName("$name_$fsIndex.dat");
		if (hasGotoPosition && addeVFeedbackMessage)
			sxrmbExporterOption->setHeaderText("Scan: $name #$number\nDate: $dateTime\nFacility: $facilityDescription\n\n$scanConfiguration[header]\nActual Horizontal Position:\t$controlValue[Horizontal Sample Stage] mm\nActual Vertical Position:\t$controlValue[Vertical Sample Stage] mm\n\n$notes\nNote that I0.X is the energy feedback.\n\n");
		else if (hasGotoPosition)
			sxrmbExporterOption->setHeaderText("Scan: $name #$number\nDate: $dateTime\nFacility: $facilityDescription\n\n$scanConfiguration[header]\nActual Horizontal Position:\t$controlValue[Horizontal Sample Stage] mm\n\n$notes\n\n");
		else
			sxrmbExporterOption->setHeaderText("Scan: $name #$number\nDate: $dateTime\nFacility: $facilityDescription\n\n$scanConfiguration[header]\n\n$notes\n\n");
		sxrmbExporterOption->setHeaderIncluded(true);
		sxrmbExporterOption->setColumnHeader("$dataSetName $dataSetInfoDescription");
		sxrmbExporterOption->setColumnHeaderIncluded(true);
		sxrmbExporterOption->setColumnHeaderDelimiter("");
		sxrmbExporterOption->setSectionHeader("");
		sxrmbExporterOption->setSectionHeaderIncluded(true);
		sxrmbExporterOption->setIncludeAllDataSources(true);
		sxrmbExporterOption->setFirstColumnOnly(true);
		sxrmbExporterOption->setIncludeHigherDimensionSources(includeHigherOrderSources);
		sxrmbExporterOption->setSeparateHigherDimensionalSources(true);
		sxrmbExporterOption->setSeparateSectionFileName("$name_$dataSetName_$fsIndex.dat");
		sxrmbExporterOption->setHigherDimensionsInRows(exportSpectraInRows);
		sxrmbExporterOption->storeToDb(AMDatabase::database("user"));

		return sxrmbExporterOption;
	}

	/// Builds the SMAK exporter option used for all exported scans.
	inline AMExporterOptionSMAK *buildSMAKExporterOption(const QString &name, bool includeHigherOrderSources, bool hasGotoPosition, bool addeVFeedbackMessage, bool exportSpectraInRows)
	{
		QList<int> matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionSMAK>(), "name", name);

		AMExporterOptionSMAK *sxrmbExporterOption = new AMExporterOptionSMAK();

		if (matchIDs.count() != 0)
			sxrmbExporterOption->loadFromDb(AMDatabase::database("user"), matchIDs.at(0));

		sxrmbExporterOption->setName(name);
		sxrmbExporterOption->setFileName("$name_$fsIndex.dat");
		if (hasGotoPosition && addeVFeedbackMessage)
			sxrmbExporterOption->setHeaderText("Scan: $name #$number\nDate: $dateTime\nFacility: $facilityDescription\n\n$scanConfiguration[header]\nActual Horizontal Position:\t$controlValue[Horizontal Sample Stage] mm\nActual Vertical Position:\t$controlValue[Vertical Sample Stage] mm\n\n$notes\nNote that I0.X is the energy feedback.\n\n");
		else if (hasGotoPosition)
			sxrmbExporterOption->setHeaderText("Scan: $name #$number\nDate: $dateTime\nFacility: $facilityDescription\n\n$scanConfiguration[header]\nActual Horizontal Position:\t$controlValue[Horizontal Sample Stage] mm\n\n$notes\n\n");
		else
			sxrmbExporterOption->setHeaderText("Scan: $name #$number\nDate: $dateTime\nFacility: $facilityDescription\n\n$scanConfiguration[header]\n\n$notes\n\n");
		sxrmbExporterOption->setHeaderIncluded(true);
		sxrmbExporterOption->setColumnHeader("$dataSetName $dataSetInfoDescription");
		sxrmbExporterOption->setColumnHeaderIncluded(true);
		sxrmbExporterOption->setColumnHeaderDelimiter("");
		sxrmbExporterOption->setSectionHeader("");
		sxrmbExporterOption->setSectionHeaderIncluded(true);
		sxrmbExporterOption->setIncludeAllDataSources(true);
		sxrmbExporterOption->setFirstColumnOnly(true);
		sxrmbExporterOption->setIncludeHigherDimensionSources(includeHigherOrderSources);
		sxrmbExporterOption->setSeparateHigherDimensionalSources(true);
		sxrmbExporterOption->setSeparateSectionFileName("$name_$dataSetName_$fsIndex.dat");
		sxrmbExporterOption->setHigherDimensionsInRows(exportSpectraInRows);
		sxrmbExporterOption->setRegExpString("^\\w{1,2}Ka1|^\\w{1,2}Kb1|^\\w{1,2}La1|^\\w{1,2}Lb1|^\\w{1,2}Lg1|I0Detector|TEYDetector");
		sxrmbExporterOption->storeToDb(AMDatabase::database("user"));

		return sxrmbExporterOption;
	}
}

Q_DECLARE_OPERATORS_FOR_FLAGS(SXRMB::FluorescenceDetectors)

#endif // SXRMB_H
