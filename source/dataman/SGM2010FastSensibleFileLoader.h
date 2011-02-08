#ifndef SGM2010FASTSENSIBLEFILELOADER_H
#define SGM2010FASTSENSIBLEFILELOADER_H

#include "dataman/AMAbstractFileLoader.h"
#include "util/AMBiHash.h"

class AMFastScan;

class SGM2010FastSensibleFileLoader : public AMAbstractFileLoader
{
public:
    SGM2010FastSensibleFileLoader(AMFastScan *scan = 0);

    QString formatTag() const { return "sgm2010FastSensible"; }

    /// load from SGM sensible fast scan file format
    bool loadFromFile(const QString &filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks);

    /// save to SGM sensible fast scan file format
    bool saveToFile(const QString &filepath);


protected:
    /// A list (in order) of the column names that our users might be interested in (ie: those which we should expose the raw data for, by default).  These names should match the column names in columns2pvNames_.
    QStringList defaultUserVisibleColumns_;
};

#endif // SGM2010FASTSENSIBLEFILELOADER_H
