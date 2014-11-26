#ifndef REIXSSCANACTIONCONTROLLERMCPFILEWRITER_H
#define REIXSSCANACTIONCONTROLLERMCPFILEWRITER_H

#include "acquaman/AMScanActionControllerBasicFileWriter.h"

/// This class extends the basic file writer to work specifically for the REIXS MCP detector.
class REIXSScanActionControllerMCPFileWriter : public AMScanActionControllerBasicFileWriter
{
	Q_OBJECT

public:
	/// Constructor.  Requires the file path for where the files whould be stored.  \param filePath should  have everything for the data file except the extension.
	REIXSScanActionControllerMCPFileWriter(const QString &filePath, QObject *parent = 0);
	/// Destructor.
	virtual ~REIXSScanActionControllerMCPFileWriter();

public slots:
	/// Writes the given string to the file.  The only rank that should be passed into this function is 2.
	virtual void writeToFile(int fileRank, const QString &textToWrite);

protected:
	/// Write protection flag
	bool isWritingToFile_;
};

#endif // REIXSSCANACTIONCONTROLLERMCPFILEWRITER_H
