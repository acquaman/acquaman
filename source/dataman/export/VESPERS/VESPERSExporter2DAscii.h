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


#ifndef VESPERSEXPORTER2DASCII_H
#define VESPERSEXPORTER2DASCII_H

#include "dataman/export/AMExporter2DAscii.h"

/*!
	This class extends the generic exporter to write the files in a way that is similar in style as the current dacq output.
	This is puts all the data for the data sources in single columns, even though all the data sources inside of the maps are
	all 2D data.

	Due to the simple extension to AMExporterGeneralAscii, VESPERSExporter2DAscii uses AMExporterOptionGeneralAscii for its
	exporter option.  It does not require anything in addition to that.

	It is important to note that this is used ONLY with VESPERS 2D maps because it needs some VESPERS specific knowledge.
	Not for general use.
  */
class VESPERSExporter2DAscii : public AMExporter2DAscii
{
	Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE explicit VESPERSExporter2DAscii(QObject *parent = 0);
	/// Destructor.
	virtual ~VESPERSExporter2DAscii();

protected:
	/// Method that writes the data in the main table, including the function names.
	virtual void writeMainTable();
};

#endif // VESPERSEXPORTER2DASCII_H
