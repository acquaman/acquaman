#ifndef SGM2011XASFILELOADERPLUGIN_H
#define SGM2011XASFILELOADERPLUGIN_H

#include <QObject>
#include <QStringList>
#include "SGM2011XASFileLoaderPlugin.h"
#include "dataman/AMFileLoaderInterface.h"
#include "util/AMBiHash.h"

class SGM2011XASFileLoaderPlugin : public QObject, AMFileLoaderInterface
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
		pv = newName.section('_', 0, 0);//Want to remove anything after the underscore
		return true;
	}

protected:
	/// A forward- and reverse-mapping from meaningful data column names to process variable strings
	static AMBiHash<QString, QString> columns2pvNames_;

	/* Want/need to use this ... get some help
	/// A forward- and reverse-mapping from offset data (for the spectra.dat files) to their measurement infos
	static AMBiHash<QString, AMMeasurementInfo> offsets2MeasurementInfos_;
	*/
	/**/
	QStringList offsets2MeasurementInfos_;
	/**/

	/// A list (in order) of the column names that our users might be interested in (ie: those which we should expose the raw data for, by default).  These names should match the column names in columns2pvNames_.
	QStringList defaultUserVisibleColumns_;
};

#endif
