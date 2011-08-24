#ifndef VESPERSXASDATALOADER_H
#define VESPERSXASDATALOADER_H

#include "dataman/AMAbstractFileLoader.h"
#include "dataman/AMXASScan.h"

/*!
  This class implements loading of the raw XAS data files that are outputted from the Dacq library.
  The file type is the standard CLS output file.
  */
class VESPERSXASDataLoader : public AMAbstractFileLoader
{

public:
	/// Constructor.  Creates a file loader to load the scan from \param scan.
	explicit VESPERSXASDataLoader(AMXASScan *scan);

	/// Format tag. a unique string identifying this format.
	virtual QString formatTag() const { return QString("vespersXAS"); }

	/// Loads data from \param filepath into the target scan.
	virtual bool loadFromFile(const QString &filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks);

	/// This is unimplemented because the Dacq library and corresponding scan controller handles saving the file.
	virtual bool saveToFile(const QString &filepath) { Q_UNUSED(filepath) return false; }
};

#endif // VESPERSXASDATALOADER_H
