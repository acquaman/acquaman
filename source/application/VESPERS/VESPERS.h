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

#include "dataman/info/AMROIInfo.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
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
	enum Beam { NoBeam = 0, Pink, TenPercent, OnePointSixPercent, Si };

	/// Enum for making the decision on what fluorescence detector the user wants to use.
	enum FluorescenceDetector { NoXRF = 0, SingleElement = 1, FourElement = 2 };

	/// Enum for making the decision on what CCD detector the user wants to use.
	enum CCDDetector { NoCCD = 0, Roper = 1, Mar = 2, Pilatus = 4 };

	/// Enum for the ion chambers used in scans.  These are used for the incoming or transmitted total intensity.
	enum IonChamber { Isplit = 0, Iprekb, Imini, Ipost };

	/// Enum for what motor(s) are going to be used.  Currently, there are only the motors and pseudo motors of the primary sample stage.  More will be added as they are added to the beamline.
	enum Motor { NoMotor = 0, H = 1, V = 2, N = 4, X = 8, Z = 16, Y = 32, AttoH = 64, AttoV = 128, AttoN = 256, AttoX = 512, AttoZ = 1024, AttoY = 2048, AttoRz = 4096, AttoRy = 8192, AttoRx = 16384, BigBeamX = 32768, BigBeamZ = 65536, WireH = 131072, WireV = 262144, WireN = 524288 };

	/// Enum for choosing the geometry.
	/*!
		The following are the available choices for the geometry.  Note that the Big beam option is technically available, but most of the
		hardware for that part of the endstation is not available currently.  What will be used there is still the same though.

		- Invalid:	Used as the null or uninitialised value.
		- StraightOn:  This has the sample sitting perpendicular to the beam.  When using the sample stage, it uses the x and z motors and
					only has XAS and XRF mapping available.  The current setup has ion chambers and the single element vortex detector.
		- Single45Vertical:	This has the sample sitting at 45 degrees vertically such that reflection based diffraction patterns can be
					measured as well as any XRF or XAS measurements.  When using the sample stage, it uses the pseudo-motors H and V and
					has all of the techniques available to it (XAS, XRF, XRD).  The current setup has ion chambers, single element vortex
					detector, and the Roper CCD.
		- Single45Horizontal:  This has the sample sitting at 45 degrees horizontally.  When using the sample stage, it uses the x and z
					motors and only has XAS and XRF mapping available.  The current setup has ion chambers and the four element vortex
					detector.
		- Double45:	This has the sample sitting at 45 degrees both vertically and horizontally such that reflection based diffraction
					patterns can be measured as well as XRF or XAS measurements.  When using the sample stage, it uses the pseudo-motors
					H and V and has all of the techniques available to it (XAS, XRF, XRD).  The current setup has ion chambers, four element
					vortex detector, and the Roper CCD.
		- BigBeam:	This has the sample sitting upstream of the Pre-KB ion chamber.  This setup has a completely different sample stage
					and the only techniques available to it are XAS and XRF mapping (macro-probe mapping).  The current setup has ion chambers
					and the four element vortex detector.
	  */
	enum Geometry { Invalid = 0, StraightOn, Single45Vertical, Single45Horizontal, Double45, BigBeam };

	/// Helper method that takes a time in seconds and returns a string of d:h:m:s.
	inline QString convertTimeToString(double time)
	{
		QString timeString;

		int days = int(time/3600.0/24.0);

		if (days > 0){

			time -= days*3600.0*24;
			timeString += QString::number(days) + "d:";
		}

		int hours = int(time/3600.0);

		if (hours > 0){

			time -= hours*3600;
			timeString += QString::number(hours) + "h:";
		}

		int minutes = int(time/60.0);

		if (minutes > 0){

			time -= minutes*60;
			timeString += QString::number(minutes) + "m:";
		}

		int seconds = ((int)time)%60;
		timeString += QString::number(seconds) + "s";

		return timeString;
	}

	/// Helper method that returns a list of QPairs where each pair corresponds to the same ROIs.  Used only when using both vortex detectors together.
	inline QList<QPair<int, int> > findRoiPairs(AMROIInfoList *list1, AMROIInfoList *list2)
	{
		QList<QPair<int, int> > list;

		// Do it the easy way first.  Only possible when the sizes are the same.
		if (list1->count() == list2->count()){

			bool allLinedUp = true;

			for (int i = 0, count = list1->count(); i < count; i++)
				if (list1->at(i).name() != list2->at(i).name())
					allLinedUp = false;

			// If true, this is really straight forward.
			if (allLinedUp){

				for (int i = 0, count = list1->count(); i < count; i++)
					list << qMakePair(i, i);
			}

			// Otherwise, we have to check each individually.  Not all may match and only matches will be added to the list.
			else {

				for (int i = 0, count = list1->count(); i < count; i++)
					for (int j = 0; j < count; j++)
						if (list1->at(i).name() == list2->at(j).name())
							list << qMakePair(i, j);
			}
		}

		// This is the same the above double for-loop but with different boundaries.
		else {

			for (int i = 0, count1 = list1->count(); i < count1; i++)
				for (int j = 0, count4 = list2->count(); j < count4; j++)
					if (list1->at(i).name() == list2->at(j).name())
						list << qMakePair(i, j);
		}

		return list;
	}

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
			vespersDefault->setHeaderText("Scan: $name #$number\nDate: $dateTime\nSample: $sample\nFacility: $facilityDescription\n\n$scanConfiguration[header]\nActual Horizontal Position:\t$controlValue[Horizontal Sample Stage] mm\nActual Vertical Position:\t$controlValue[Vertical Sample Stage] mm\n\n$notes\nNote that I0.X is the energy feedback.\n\n");
		else if (hasGotoPosition)
			vespersDefault->setHeaderText("Scan: $name #$number\nDate: $dateTime\nSample: $sample\nFacility: $facilityDescription\n\n$scanConfiguration[header]\nActual Horizontal Position:\t$controlValue[Horizontal Sample Stage] mm\n\n$notes\n\n");
		else
			vespersDefault->setHeaderText("Scan: $name #$number\nDate: $dateTime\nSample: $sample\nFacility: $facilityDescription\n\n$scanConfiguration[header]\n\n$notes\n\n");
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
	inline QString pvToString(AMProcessVariable *pv)
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

#endif // VESPERS_H
