#ifndef SGM2010FastFILELOADERPLUGIN_H
#define SGM2010FastFILELOADERPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QDebug>
#include "SGM2010FastFileLoaderPlugin.h"
#include "dataman/AMFileLoaderInterface.h"
#include "util/AMBiHash.h"

class SGM2010FastFileLoaderPlugin : public QObject, AMFileLoaderInterface
{
        Q_OBJECT
	Q_INTERFACES(AMFileLoaderInterface)

public:
	virtual bool accepts(AMScan *scan);

	virtual bool load(AMScan *scan, const QString &userDataFolder);

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

#endif
