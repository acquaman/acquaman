#ifndef SGM2010FASTFILELOADER_H
#define SGM2010FASTFILELOADER_H

#include "dataman/AMAbstractFileLoader.h"
#include "util/AMBiHash.h"

class AMFastScan;

class SGM2010FastFileLoader : public AMAbstractFileLoader
{
public:
    SGM2010FastFileLoader(AMFastScan *scan = 0);

    QString formatTag() const { return "sgm2010Fast"; }

    /// load from SGM _spectra.dat file format
    bool loadFromFile(const QString &filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks);

    /// save to SGM _spectra.dat file format
    bool saveToFile(const QString &filepath);

    /// translate a PV string from the file header to a meaningful name, if we recognize it.  Unrecognized PV strings are untouched and false is returned.
    static bool pv2columnName(QString& pv) {
	    QString newName = columns2pvNames_.valueR(pv);
	    if(newName.isEmpty())
		    return false;
	    pv = newName;
	    return true;
    }


protected:

    /// A forward- and reverse-mapping from meaningful data column names to process variable strings
    static AMBiHash<QString, QString> columns2pvNames_;

    /// A list (in order) of the column names that our users might be interested in (ie: those which we should expose the raw data for, by default).  These names should match the column names in columns2pvNames_.
    QStringList defaultUserVisibleColumns_;
};

#endif // SGM2010FASTFILELOADER_H
