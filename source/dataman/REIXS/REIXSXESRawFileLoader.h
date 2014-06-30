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


#ifndef REIXSXESRAWFILELOADER_H
#define REIXSXESRAWFILELOADER_H

#include "dataman/AMAbstractFileLoader.h"
#include "dataman/AMXESScan.h"

/// THIS CLASS IS DEPRECATED: We now use AMCDFv1FileLoader. This class implements loading and saving of raw XES data in the "reixsXESRaw" file format.  The file format is binary, big endian format, and consists of the following data:
/*!
 - Magic string, in ASCII encoding: "REIXSXESRAW2011.1"
 - Number of x pixels (32-bit integer)
 - Number of y pixels (32-bit integer)
 - all the pixels (32-bit integer each) starting from x=0, y=0, with y varying the fastest (ie: outer loop on x, inner loop on y)
 - A closing magic string, in ASCII encoding: "REIXSXESRAW2011.1"
 */
class REIXSXESRawFileLoader : public AMAbstractFileLoader
{
public:
	/// Construct a file loader to data into \c scan.
 	virtual ~REIXSXESRawFileLoader();
	REIXSXESRawFileLoader(AMXESScan* scan);

	/// Format tag: a unique string identifying this format (ex: "reixsXESRaw")
	virtual QString formatTag() const { return QString("reixsXESRaw"); }

	/// Load raw data from \c filepath into the target scan.  Several options are available to control what this function does with additional information it can find in the file.
	/*! \param setMetaData: true if you want to extract metaData from the file and set the scan's metaData information.
		\param setRawDataSources: true if you want to clear the currently-configured AMScan::rawDataSources() and create new ones. If false, will leave the current rawDataSources alone. Warning: do not create a situation where the existing rawDataSources() attempt to access non-existent raw data.
		\param createDefaultAnalysisBlocks: Sometimes it makes sense for a certain scan type to always have some default AMAnalysisBlocks. (For example: normalized fluorescence yield for an XAS scan.) Set to true if it should attempt to create these.
		*/
	virtual bool loadFromFile(const QString& filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks);

	/// Save the target scan to the file at \c filepath
	virtual bool saveToFile(const QString& filepath);
};

#endif // REIXSXESRAWFILELOADER_H
