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


#include "AMExporterOptionGeneral.h"

AMExporterOptionGeneral::~AMExporterOptionGeneral(){}

AMExporterOptionGeneral::AMExporterOptionGeneral(QObject *parent) :
	AMExporterOption(parent)
{

	includeAllDataSources_ = true;
	includeHigherDimensionSources_ = true;
	firstColumnOnly_ = false;
	separateHigherDimensionalSources_ = false;
	higherDimensionsInRows_ = true;
	headerText_ =	"Scan: $name #$number\n"
					"Date: $dateTime\n"
					"Sample: $sample\n"
					"Facility: $facilityDescription\n\n";
	headerIncluded_ = true;

	columnHeader_ =	"$dataSetName";	/// \todo Removed $dataSetUnits because we don't have that implemented yet.
	columnHeaderIncluded_ = true;

	columnHeaderDelimiter_ = "==========";

	sectionHeader_ = "\n$dataSetName: $dataSetDescription\nSize: $dataSetSize\n"		/// \todo Removed $dataSetUnits because we don't have that implemented yet.
						"----------";
	sectionHeaderIncluded_ = true;

	separateSectionFileName_ = "$name_$number_$dataSet_$dateTime[yyyyMMdd_hhmmss].dat";
}

AMExporterOptionGeneral::AMExporterOptionGeneral(const AMExporterOptionGeneral &original)
	: AMExporterOption(original)
{
	includeAllDataSources_ = original.includeAllDataSources();
	includeHigherDimensionSources_ = original.includeHigherDimensionSources();
	firstColumnOnly_ = original.firstColumnOnly();
	separateHigherDimensionalSources_ = original.separateHigherDimensionalSources();
	higherDimensionsInRows_ = original.higherDimensionsInRows();
	headerText_ = original.headerText();

	headerIncluded_ = original.headerIncluded();

	columnHeader_ =	original.columnHeader();
	columnHeaderIncluded_ = original.columnHeaderIncluded();

	columnHeaderDelimiter_ = original.columnHeaderDelimiter();

	sectionHeader_ = original.sectionHeader();
	sectionHeaderIncluded_ = original.sectionHeaderIncluded();

	separateSectionFileName_ = original.separateSectionFileName();
}

const QMetaObject* AMExporterOptionGeneral::getMetaObject(){
	return metaObject();
}
