/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMEXPORTERATHENA_H
#define AMEXPORTERATHENA_H

#include "dataman/export/AMExporterGeneralAscii.h"

/*!
	This class extends the generic exporter to write the files in a way that is readily available for Athena to import.
	Essentially there is a small format change where every line of the header is predicated with a # symbol and it also
	has a special delimiter for separating the header from the data and finally the column names can't have spaces.

	What this class will do is make sure that the given header contains all of the necessary # symbols and makes sure
	all of the column names are readable inside of Athena.

	Due to the simple extension to AMExporterGeneralAscii, AMExporterAthena uses AMExporterOptionGeneralAscii for its
	exporter option.  It does not require anything in addition to that.
  */

class AMExporterAthena : public AMExporterGeneralAscii
{
	Q_OBJECT

public:
	/// Constructor.
 	virtual ~AMExporterAthena();
	Q_INVOKABLE explicit AMExporterAthena(QObject *parent = 0);

	/// Description of the exporter.
	virtual QString description() const { return "Athena Style (plain text file)"; }
	/// More verbose description of the exporter.
	virtual QString longDescription() const {
		return "The Athena Style file format creates one table of data in a plain text file in the recommended file format for Athena.  You can choose which data sources to include.  You can also specify what to include in an optional header, and where to put higher-dimensional (2D, 3D, etc.) data.";
	}

	/// Exports the scan.
	virtual QString exportScan(const AMScan *scan, const QString &destinationFolderPath, const AMExporterOption *option, int autoIndex = 0);

protected:
	/// Method that writes the header piece of the exported file.
	virtual void writeHeader();
	/// Method that writes the data in the main table, including the function names.
	virtual void writeMainTable();
	/// Method that writes the separate sections of the data file.  Athena doesn't like separate sections, so this function is castrated.
	virtual void writeSeparateSections();
};

#endif // AMEXPORTERATHENA_H
