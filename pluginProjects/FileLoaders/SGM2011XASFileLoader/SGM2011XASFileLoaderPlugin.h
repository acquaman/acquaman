#ifndef SGM2011XASFILELOADERPLUGIN_H
#define SGM2011XASFILELOADERPLUGIN_H

#include <QObject>
#include <QStringList>

#define SGM2011XASFILELOADERPLUGIN_CANNOT_OPEN_FILE 627501
#define SGM2011XASFILELOADERPLUGIN_BAD_FORMAT_NO_EVENT_HEADER 627502
#define SGM2011XASFILELOADERPLUGIN_BAD_FORMAT_NO_PV_CONVERSION 627503
#define SGM2011XASFILELOADERPLUGIN_BAD_FORMAT_NO_ENERGY_COLUMN 627504
#define SGM2011XASFILELOADERPLUGIN_MISSING_SPECTRA_FILE 627505
#define SGM2011XASFILELOADERPLUGIN_CANNOT_OPEN_SPECTRA_FILE 627506
#define SGM2011XASFILELOADERPLUGIN_BAD_FORMAT_CORRUPTED_SPECTRA_DATA 627507
#define SGM2011XASFILELOADERPLUGIN_DATA_COLUMN_MISMATCH 627508

#include "dataman/AMFileLoaderInterface.h"
#include "util/AMBiHash.h"

class SGM2011XASFileLoaderPlugin : public AMFileLoaderInterface
{

public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept sgm2011XAS.
	virtual QStringList acceptedFileFormats() { return QStringList() << "sgm2011XAS"; }

	virtual bool accepts(AMScan *scan);

	virtual bool load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor);

	/// translate a PV string from the file header to a meaningful name, if we recognize it.  Unrecognized PV strings are untouched and false is returned.
	bool pv2columnName(QString& pv) {
		QString newName = columns2pvNames_.valueR(pv);
		if(newName.isEmpty())
			return false;
		pv = newName.section('_', 0, 0);//Want to remove anything after the underscore
		return true;
	}

protected:
	/// A forward- and reverse-mapping from meaningful data column names to process variable strings
	AMBiHash<QString, QString> columns2pvNames_;

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

class SGM2011XASFileLoaderFactory : public QObject, public AMFileLoaderFactory
{
	Q_OBJECT
	Q_INTERFACES(AMFileLoaderFactory)

public:
	/// A list of strings matching the "fileFormat()" string in AMScan, which this file loader can handle. We accept sgm2011XAS.
	virtual QStringList acceptedFileFormats() { return QStringList() << "sgm2011XAS"; }
	/// This function allows you to do a more detailed check of an AMScan to see whether you can load data for it.
	virtual bool accepts(AMScan *scan);

	/// This should create an instance of an AMFileLoaderInterface object to do the actual loading.
	virtual AMFileLoaderInterface* createFileLoader() { return new SGM2011XASFileLoaderPlugin(); }
};


#endif
