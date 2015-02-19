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

#include "dataman/export/AMSMAKExporter.h"

/*!
	This class extends the generic SMAK exporter because VESPERS has special area detector columns
	that need extra logic when writing out the exported files.

	It is important to note that this is used ONLY with VESPERS 2D maps because it needs some VESPERS specific knowledge.
	Not for general use.
  */
class VESPERSExporterSMAK : public AMSMAKExporter
{
	Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE explicit VESPERSExporterSMAK(QObject *parent = 0);
	/// Constructor.
	virtual ~VESPERSExporterSMAK();

	/// Description of the exporter.
	virtual QString description() const { return "VESPERS SMAK 2D Map (plain text file)"; }

protected:
	/// Method that writes the data in the main table, including the function names.
	virtual void writeMainTable();
};

#endif // VESPERSEXPORTERSMAK_H
