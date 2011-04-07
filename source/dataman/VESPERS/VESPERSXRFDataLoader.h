#ifndef VESPERSXRFDATALOADER_H
#define VESPERSXRFDATALOADER_H

#include "dataman/AMAbstractFileLoader.h"
#include "dataman/VESPERS/AMXRFScan.h"

/*! This class implements loading and saving of XRF data in a custom way.  This is because this data is typically not saved currently.
	It consists of columns with the raw data from each element of the detector.
*/
class VESPERSXRFDataLoader : public AMAbstractFileLoader
{
public:
	/// Constructs a file loader to data into \c scan.
	VESPERSXRFDataLoader(AMXRFScan *scan);

	/// Format tag: a unique string identifying this format.
	virtual QString formatTag() const { return QString("vespersXRF"); }

	/// Loads data from \c filepath into the target scan.
	virtual bool loadFromFile(const QString& filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks);

	/// Save the target scan to the file at \c filepath.
	virtual bool saveToFile(const QString& filepath);
};

#endif // VESPERSXRFDATALOADER_H
