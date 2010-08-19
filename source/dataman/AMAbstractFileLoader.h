#ifndef AMABSTRACTFILELOADER_H
#define AMABSTRACTFILELOADER_H

#include <QString>
#include "dataman/AMScan.h"

/// This class defines the interface for a "file loader" object, responsible for loading raw data from a file into a scan object.
class AMAbstractFileLoader
{
public:
	// Construct a loader object pointing to a particular scan
	AMAbstractFileLoader(AMScan* target) {
		scan_ = target;
	}

	/// Format tag: a unique string identifying this format (ex: "sgm2004")
	virtual QString formatTag() const = 0;

	/// Load raw data from \c filepath into the target scan
	virtual bool loadFromFile(const QString& filepath, bool extractMetaData = true, bool createChannels = true) = 0;

	/// Save the target scan to the file at \c filepath
	virtual bool saveToFile(const QString& filepath) = 0;

	/// Set the target scan
	void operator=(AMScan& scan) {
		scan_ = &scan;
	}

protected:
	AMScan* scan_;
};

#endif // AMABSTRACTFILELOADER_H
