/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef VESPERS_H
#define VESPERS_H

/*!
	This namespace contains enums that are used throughout the VESPERS application.  It is meant to consolidate them all
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
#include "beamline/AMProcessVariable.h"

namespace VESPERS {

	/// Enum for the different beams.
	/*!
	 - Pink is not monochromatized and contains all the energies from the bending magnet.
	 - TenPercent (10%) is a broad band pass filter.
	 - OnePointSixPercent (1.6%) is a narrow band pass filter.
	 - Si is the monochromator with 0.01% band pass.
	  */
	enum Beam
	{
		NoBeam = 0,
		Pink,
		TenPercent,
		OnePointSixPercent,
		Si
	};

	/// Enum for making the decision on what fluorescence detector the user wants to use.
	enum FluorescenceDetector
	{
		NoXRF = 0,
		SingleElement = 1,
		FourElement = 2,
		Ge13Element = 4
	};
	Q_DECLARE_FLAGS(FluorescenceDetectors, FluorescenceDetector)

	/// Enum for making the decision on what CCD detector the user wants to use.
	enum CCDDetector
	{
		NoCCD = 0,
		Roper = 1,
		Mar = 2,
		Pilatus = 4
	};
	Q_DECLARE_FLAGS(CCDDetectors, CCDDetector)

	/// Enum for the ion chambers used in scans.  These are used for the incoming or transmitted total intensity.
	enum IonChamber
	{
		Isplit = 0,
		Iprekb,
		Imini,
		Ipost
	};

	/// Enum for what motor(s) are going to be used.  Currently, there are only the motors and pseudo motors of the primary sample stage.  More will be added as they are added to the beamline.
	enum Motor
	{
		NoMotor = 0,
		H = 1,
		V = 2,
		N = 4,
		X = 8,
		Z = 16,
		Y = 32,
		AttoH = 64,
		AttoV = 128,
		AttoN = 256,
		AttoX = 512,
		AttoZ = 1024,
		AttoY = 2048,
		AttoRz = 4096,
		AttoRy = 8192,
		AttoRx = 16384,
		BigBeamX = 32768,
		BigBeamZ = 65536,
		WireH = 131072,
		WireV = 262144,
		WireN = 524288
	};
	Q_DECLARE_FLAGS(Motors, Motor)

	/// Returns the home directory for Acquaman.
	inline QString getHomeDirectory()
	{
		// Find out which path we are using for acquaman (depends on whether you are on Mac or Linux or beamline OPI).
		QString homeDir = QDir::homePath();
		if(QDir(homeDir+"/dev").exists())
			homeDir.append("/dev");
		else if(QDir(homeDir+"/beamline/programming").exists())
			homeDir.append("/beamline/programming");

		return homeDir;
	}

	/// Builds the standard exporter option used for all exported scans.
	inline AMExporterOptionGeneralAscii *buildStandardExporterOption(const QString &name, bool includeHigherOrderSources, bool hasGotoPosition, bool addeVFeedbackMessage, bool exportSpectraInRows)
	{
		QList<int> matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionGeneralAscii>(), "name", name);

		AMExporterOptionGeneralAscii *vespersDefault = new AMExporterOptionGeneralAscii();

		if (matchIDs.count() != 0)
			vespersDefault->loadFromDb(AMDatabase::database("user"), matchIDs.at(0));

		vespersDefault->setName(name);
		vespersDefault->setFileName("$name_$fsIndex.dat");
		if (hasGotoPosition && addeVFeedbackMessage)
			vespersDefault->setHeaderText("Scan: $name #$number\nDate: $dateTime\nFacility: $facilityDescription\n\n$scanConfiguration[header]\nActual Horizontal Position:\t$controlValue[Horizontal Sample Stage] mm\nActual Vertical Position:\t$controlValue[Vertical Sample Stage] mm\n\n$notes\nNote that I0.X is the energy feedback.\n\n");
		else if (hasGotoPosition)
			vespersDefault->setHeaderText("Scan: $name #$number\nDate: $dateTime\nFacility: $facilityDescription\n\n$scanConfiguration[header]\nActual Horizontal Position:\t$controlValue[Horizontal Sample Stage] mm\n\n$notes\n\n");
		else
			vespersDefault->setHeaderText("Scan: $name #$number\nDate: $dateTime\nFacility: $facilityDescription\n\n$scanConfiguration[header]\n\n$notes\n\n");
		vespersDefault->setHeaderIncluded(true);
		vespersDefault->setColumnHeader("$dataSetName $dataSetInfoDescription");
		vespersDefault->setColumnHeaderIncluded(true);
		vespersDefault->setColumnHeaderDelimiter("");
		vespersDefault->setSectionHeader("");
		vespersDefault->setSectionHeaderIncluded(true);
		vespersDefault->setIncludeAllDataSources(true);
		vespersDefault->setFirstColumnOnly(true);
		vespersDefault->setIncludeHigherDimensionSources(includeHigherOrderSources);
		vespersDefault->setSeparateHigherDimensionalSources(true);
		vespersDefault->setSeparateSectionFileName("$name_$dataSetName_$fsIndex.dat");
		vespersDefault->setHigherDimensionsInRows(exportSpectraInRows);
		vespersDefault->storeToDb(AMDatabase::database("user"));

		return vespersDefault;
	}

	/// Builds the SMAK exporter option used for all exported scans.
	inline AMExporterOptionSMAK *buildSMAKExporterOption(const QString &name, bool includeHigherOrderSources, bool hasGotoPosition, bool addeVFeedbackMessage, bool exportSpectraInRows)
	{
		QList<int> matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionSMAK>(), "name", name);

		AMExporterOptionSMAK *vespersDefault = new AMExporterOptionSMAK();

		if (matchIDs.count() != 0)
			vespersDefault->loadFromDb(AMDatabase::database("user"), matchIDs.at(0));

		vespersDefault->setName(name);
		vespersDefault->setFileName("$name_$fsIndex.dat");
		if (hasGotoPosition && addeVFeedbackMessage)
			vespersDefault->setHeaderText("Scan: $name #$number\nDate: $dateTime\nFacility: $facilityDescription\n\n$scanConfiguration[header]\nActual Horizontal Position:\t$controlValue[Horizontal Sample Stage] mm\nActual Vertical Position:\t$controlValue[Vertical Sample Stage] mm\n\n$notes\nNote that I0.X is the energy feedback.\n\n");
		else if (hasGotoPosition)
			vespersDefault->setHeaderText("Scan: $name #$number\nDate: $dateTime\nFacility: $facilityDescription\n\n$scanConfiguration[header]\nActual Horizontal Position:\t$controlValue[Horizontal Sample Stage] mm\n\n$notes\n\n");
		else
			vespersDefault->setHeaderText("Scan: $name #$number\nDate: $dateTime\nFacility: $facilityDescription\n\n$scanConfiguration[header]\n\n$notes\n\n");
		vespersDefault->setHeaderIncluded(true);
		vespersDefault->setColumnHeader("$dataSetName $dataSetInfoDescription");
		vespersDefault->setColumnHeaderIncluded(true);
		vespersDefault->setColumnHeaderDelimiter("");
		vespersDefault->setSectionHeader("");
		vespersDefault->setSectionHeaderIncluded(true);
		vespersDefault->setIncludeAllDataSources(true);
		vespersDefault->setFirstColumnOnly(true);
		vespersDefault->setIncludeHigherDimensionSources(includeHigherOrderSources);
		vespersDefault->setSeparateHigherDimensionalSources(true);
		vespersDefault->setSeparateSectionFileName("$name_$dataSetName_$fsIndex.dat");
		vespersDefault->setHigherDimensionsInRows(exportSpectraInRows);
		vespersDefault->setRegExpString("^\\w{1,2}Ka1|^\\w{1,2}Kb1|^\\w{1,2}La1|^\\w{1,2}Lb1|^\\w{1,2}Lg1|SplitIonChamber|PreKBIonChamber|MiniIonChamber");
		vespersDefault->storeToDb(AMDatabase::database("user"));

		return vespersDefault;
	}

	/// Returns whether any files with \param name exist in \param path.  It removes endings like '_1.dat' if they exist.
	inline bool fileNameExists(const QString &path, const QString &name)
	{
		QDir dir(path);

		if (dir.exists()){

			QStringList files = dir.entryList();

			foreach (QString file, files)
				if (file.startsWith(name))
					return true;
		}

		return false;
	}

	/// Returns \param name with three random numbers appended to the end.  Output will be "string-xyz".  Output is not guarenteed to be unique.
	inline QString appendUniqueIdentifier(const QString &name)
	{
		int value = qrand() % 999;
		QString suffix;

		if (value < 10)
			suffix = QString("00%1").arg(value);

		else if (value < 100)
			suffix = QString("0%1").arg(value);

		else if (value < 1000) // always
			suffix = QString("%1").arg(value);

		return name % "-" % suffix;
	}

	/// Converts the bizarre string output of the pv to a real QString.
	inline QString pvToString(const AMProcessVariable *pv)
	{
		int current;
		QString name;

		for (unsigned i = 0; i < pv->count(); i++){

			current = pv->getInt(i);

			if (current == 0)
				break;

			name += QString::fromAscii((const char *) &current);
		}

		return name;
	}

	/// Converts the string to the array of integers it needs to be.
	inline void stringToPV(AMProcessVariable *pv, QString toConvert)
	{
		int converted[256];

		QByteArray toConvertBA = toConvert.toAscii();

		for (int i = 0; i < 256; i++){

			if (i < toConvertBA.size())
				converted[i] = toConvertBA.at(i);
			else
				converted[i] = 0;
		}

		pv->setValues(converted, 256);
	}

	/// Converts the string into an appropriate Motor enum represented as an int.
	inline int sampleStageNameToEnum(const QString &name)
	{
		if (name == "Sample Stage - H, V, N")
			return H | V;

		else if (name == "Sample Stage - X, Z, Y")
			return X | Z;

		else if (name == "Attocube Stage - H, V, N")
			return AttoH | AttoV;

		else if (name == "Attocube Stage - X, Z, Y")
			return AttoX | AttoZ;

		else if (name == "Attocube Stage - Rx")
			return AttoRx;

		else if (name == "Attocube Stage - Ry")
			return AttoRy;

		else if (name == "Attocube Stage - Rz")
			return AttoRz;

		else if (name == "Big Beam - X, Z")
			return BigBeamX | BigBeamZ;

		else if (name == "Wire Stage - H, V, N")
			return WireH | WireV;

		return NoMotor;
	}

	/// Takes a user data folder and returns the proposal number.  Returns an empty string if no proposal number is in the folder.
	inline QString getProposalNumber(const QString &path)
	{
		QStringList pathParts = path.split("/");
		int index = pathParts.indexOf(QRegExp("^\\d{2,2}-\\d{4,4}$"));

		if (index == -1)
			return QString("");

		return pathParts.at(index);
	}
}

Q_DECLARE_OPERATORS_FOR_FLAGS(VESPERS::FluorescenceDetectors)
Q_DECLARE_OPERATORS_FOR_FLAGS(VESPERS::CCDDetectors)
Q_DECLARE_OPERATORS_FOR_FLAGS(VESPERS::Motors)

#endif // VESPERS_H
