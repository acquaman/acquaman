#ifndef AMLIGHTWEIGHTSCANINFO_H
#define AMLIGHTWEIGHTSCANINFO_H

#include <QString>
#include <QDateTime>
#include <QHash>
class AMDbThumbnail;
/**
 * A class representing the information stored against a scan in the user database. The class is designed
 * to be a more lightweight, quick to load way of displaying bulk information about scans. See class
 * AMLightweightScanInfoCollection for class which loads this information from the database in bulk.
 */
class AMLightweightScanInfo
{
private:
	int id_;
	QString name_;
	int number_;
	QDateTime dateTime_;
	QString scanType_;
	QString runName_;
	QString notes_;
	QString sampleName_;
	QList<int> thumbnailIds_;
	QHash<int, AMDbThumbnail*> thumbnailsMap_;

public:
	AMLightweightScanInfo(int id,
						const QString& name,
						int number,
						const QDateTime& dateTime,
						const QString& scanType,
						const QString& runName,
						const QString& notes,
						const QString& sampleName,
						int thumbnailFirstId,
						int thumbnailCount);

	int id() const;
	QString name() const;
	int number() const;
	QDateTime dateTime() const;
	QString scanType() const;
	QString runName() const;
	QString notes() const;
	QString sampleName() const;
	AMDbThumbnail* thumbnailAt(int index);
	int thumbnailCount();
};

#endif // AMLIGHTWEIGHTSCANINFO_H
