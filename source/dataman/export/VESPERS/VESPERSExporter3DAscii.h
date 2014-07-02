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


#ifndef VESPERSEXPORTER3DASCII_H
#define VESPERSEXPORTER3DASCII_H

#include "dataman/export/AMExporterGeneralAscii.h"

/*!
 This class extends the generic exporter to write the files in a way that is similar in style as the current dacq output.
 This is puts all the data for the data sources in single columns, even though all the data sources inside of the maps are
 all 3D data.

 Due to the simple extension to AMExporterGeneralAscii, VESPERSExporter3DAscii uses AMExporterOptionGeneralAscii for its
 exporter option.  It does not require anything in addition to that.

 It is important to note that this is used ONLY with VESPERS 2D maps because it needs some VESPERS specific knowledge.
 Not for general use.
  */
class VESPERSExporter3DAscii : public AMExporterGeneralAscii
{
	Q_OBJECT

public:
	/// Constructor.
 	virtual ~VESPERSExporter3DAscii();
	Q_INVOKABLE explicit VESPERSExporter3DAscii(QObject *parent = 0);

	/// Description of the exporter.
	virtual QString description() const { return "3D Map Style (plain text file)"; }
	/// More verbose description of the exporter.
	virtual QString longDescription() const
	{
		return "The 3D Map Style file format creates one table of data in a plain text file where each data source is in a single column.";
	}

	/// Exports the scan.
	virtual QString exportScan(const AMScan *scan, const QString &destinationFolderPath, const AMExporterOption *option, int autoIndex = 0);

protected:
	/// Parse the data sources in option_, find their index within the scan, and fill mainTableDataSources_, separateSectionDataSources_, and separateFileDataSources_.  Returns true if all required data sources are found.
	virtual bool prepareDataSources();
	/// Method that writes the header piece of the exported file.
	virtual void writeHeader();
	/// Method that writes the data in the main table, including the function names.
	virtual void writeMainTable();
	/// Method that writes the separate sections of the data file.  Athena doesn't like separate sections, so this function is castrated.
	virtual void writeSeparateSections();
	/// Method that writes the separate files for other data sources.
	virtual bool writeSeparateFiles(const QString& destinationFolderPath);
};

#endif // VESPERSEXPORTER3DASCII_H
