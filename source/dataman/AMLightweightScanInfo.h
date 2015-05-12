#ifndef AMLIGHTWEIGHTSCANINFO_H
#define AMLIGHTWEIGHTSCANINFO_H

#include <QString>
#include <QDateTime>
#include <QHash>

#define AMLIGHTWEIGHTSCANINFO_SQL_ERROR 36441
class AMDbThumbnail;
class AMDatabase;
/**
 * A class representing the information stored against a scan in the user database. The class is designed
 * to be a more lightweight, quick to load way of displaying bulk information about scans. The class
 * AMLightweightScanInfoCollection is the only class that should ever be constructing or writing to this class,
 * as such its constructor and setter are marked as protected, and AMLightweightScanInfoCollection a friend of
 * this class.
 */
class AMLightweightScanInfo : public QObject
{
	Q_OBJECT
public:

	/**
	 * Constructor which creates an AMLightweightScanInfo from the provided paramters.
	 */
	explicit AMLightweightScanInfo(int id,
						const QString& name,
						int number,
						const QDateTime& dateTime,
						const QString& scanType,
						int runId,
						const QString& runName,
						const QString& notes,
						const QString& sampleName,
						int thumbnailFirstId,
						int thumbnailCount,
						AMDatabase* database,
						QObject* parent = 0);


	/// The id of the scan in the database. Since these are only ever loaded from db, this will always be
	/// present
	int id() const;
	/// A descriptive name of the scan
	QString name() const;
	/// A user set number associated with the scan
	int number() const;
	/// The Date and Time that the scan was conducted
	QDateTime dateTime() const;
	/// The technique used to perform the scan (XAS, Fast etc.)
	QString scanType() const;
	/// The id of the run this scan is associated with, used when filtering by run
	int runId() const;
	/// A descriptive name of the run
	QString runName() const;
	/// A set of misc. notes which are associated with the scan
	QString notes() const;
	/// A descriptive name of the sample upon which the scan was performed
	QString sampleName() const;
	/// Returns the thumbnail at the given index which is associated with this scan.
	/// Prompts lazy loading if the thumbnail at this index has not yet been loaded
	/// from the database. If there is no thumbnail with the provided index
	/// associated with this scan, 0 is returned.
	AMDbThumbnail* thumbnailAt(int index) const;
	/// The number of thumbnails which are currently stored in the database against this scan
	int thumbnailCount() const;
	/// Refreshes the thumbnails for this Scan. To be called when an update to the
	/// thumbnail table has caused all the thumbnails for this scan to be
	/// relocated in the table.
	void refreshThumbnails(int thumbnailFirstId, int thumbnailCount);
	/// The id of the first thumbnail associated with this Scan
	int thumbnailFirstId() const;
	/// Adds the provided experimentId to the Scan
	void addExperimentId(int experimentId);
	/// The id (if any) of the experiment this scan is associated with
	const QList<int> experimentIds() const;

	/// Sets the descriptive name of the scan
	void setName(const QString& name);
	/// Sets the number associated with the scan
	void setNumber(int number);
	/// Sets the Date and Time that the scan was conducted
	void setDateTime(const QDateTime &dateTime);
	/// Sets the technique sed to perform the scan
	void setScanType(const QString& scanType);
	/// Associates this scan with the run which shares the provided runId
	void setRunId(int runId);
	/// Sets the descriptive name of the run
	void setRunName(const QString& runName);
	/// Sets the misc. notes associated with the scan
	void setNotes(const QString& notes);
	/// Sets the descriptive name of the sample upon which the scan was performed
	void setSampleName(const QString& sampleName);

protected:
	/// Clears all the thumbnails for this scan
	void clearThumbnails();

private:
	AMDatabase* database_;
	int id_;
	QString name_;
	int number_;
	QDateTime dateTime_;
	QString scanType_;
	int runId_;
	QString runName_;
	QList<int> experimentIds_;
	QString notes_;
	QString sampleName_;
	QList<int> thumbnailIds_;
	mutable QHash<int, AMDbThumbnail*> thumbnailsMap_;
};

#endif // AMLIGHTWEIGHTSCANINFO_H
