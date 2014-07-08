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
	int runId_;
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
						int runId,
						const QString& runName,
						const QString& notes,
						const QString& sampleName,
						int thumbnailFirstId,
						int thumbnailCount);

	int id() const;
	QString name() const;
	void setName(const QString& name);
	int number() const;
	void setNumber(int number);
	QDateTime dateTime() const;
	void setDateTime(const QDateTime &dateTime);
	QString scanType() const;
	void setScanType(const QString& scanType);
	int runId() const;
	void setRunId(int runId);
	QString runName() const;
	void setRunName(const QString& runName);
	QString notes() const;
	void setNotes(const QString& notes);
	QString sampleName() const;
	void setSampleName(const QString& sampleName);
	AMDbThumbnail* thumbnailAt(int index);
	int thumbnailCount();
};

#endif // AMLIGHTWEIGHTSCANINFO_H
