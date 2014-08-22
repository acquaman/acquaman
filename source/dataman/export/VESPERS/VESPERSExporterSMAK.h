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


#ifndef VESPERSEXPORTERSMAK_H
#define VESPERSEXPORTERSMAK_H

#include "dataman/export/AMExporterGeneralAscii.h"

/*!
	This class extends the generic exporter to write the files in a way that can be
	read by SMAK, but also have all the information of the scan.  Because SMAK does
	not require much of the data to visualize there are two data files.  The first
	will be SMAK specific where all the necessary data is in place to be easily visualized.
	The second will be a data file identical to VESPESExporter2DAscii because it has
	all of the data in a nice format.  Finally, the spectra files will be in SMAK's
	format.  This differs from the standard dacq format in two ways:

		- tab delimeted instead of comma
		- every row is prefaced with an index.

	It is important to note that this is used ONLY with VESPERS 2D maps because it needs some VESPERS specific knowledge.
	Not for general use.
  */
class VESPERSExporterSMAK : public AMExporterGeneralAscii
{
	Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE explicit VESPERSExporterSMAK(QObject *parent = 0);
	/// Constructor.
	virtual ~VESPERSExporterSMAK();

	/// Description of the exporter.
	virtual QString description() const { return "SMAK 2D Map (plain text file)"; }
	/// More verbose description of the exporter.
	virtual QString longDescription() const {
		return "The SMAK 2D Map file format creates two data files.  One is in SMAK's format and one contains one table of data in a plain text file where each data source is in a single column.  You can choose which data sources to include.";
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
	/// Method that writes the SMAK data file.
	void writeSMAKFile();
};

#endif // VESPERSEXPORTERSMAK_H
