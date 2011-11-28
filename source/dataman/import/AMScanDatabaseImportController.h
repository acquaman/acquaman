#ifndef AMSCANDATABASEIMPORTCONTROLLER_H
#define AMSCANDATABASEIMPORTCONTROLLER_H

#include <QObject>
#include <QMap>

#include "dataman/database/AMDatabase.h"
#include "util/AMSettings.h"

class AMScan;

class AMScanDatabaseImportController : public QObject
{
    Q_OBJECT
public:
	enum AMScanDatabaseImportControllerState {
		Preparing,
		Analyzing,
		ReadyToImport,
		Importing,
		Cancelled,
		Finished
	};

	explicit AMScanDatabaseImportController(AMDatabase* destinationDb = AMDatabase::database("user"),
											const QString& destinationPath = AMUserSettings::userDataFolder,
											QObject *parent = 0);

	// linear sequence of states
	AMScanDatabaseImportControllerState state() { return state_; }


	// must call first to go from preparing to analyzing.
	bool setSourceFolderAndLoadDatabase(const QString& absolutePathToSourceFolder);

	// only works if state is analyzing.
	void analyzeFacilitiesForDuplicates();
	void analyzeRunsForDuplicates();
	void analyzeExperimentsForDuplicates();
	void analyzeSamplesForDuplicates();

	// only works if state is readyToimport. Does nothing otherwise.
	void startDatabaseOperations();

	// only works in Importing state. No db changes before that. Sets state to cancelled afterwards.
	void cancelAndRollBack();

	// see what runs were found, and if there are duplicates. Returns the mapping from source run ids to destination run ids.
	QMap<int, int> runIdMapping() const { return s2dRunIds_; }
	void doNotMergeRun(int sourceRunId) { if(s2dRunIds_.contains(sourceRunId)) s2dRunIds_[sourceRunId] = -1; }
	QString runName(int sourceRunId) const { return sourceRuns_.value(sourceRunId); }

	QMap<int, int> experimentIdMapping() const { return s2dExperimentIds_; }
	void doNotMergeExperiment(int sourceExperimentId) { if(s2dExperimentIds_.contains(sourceExperimentId)) s2dExperimentIds_[sourceExperimentId] = -1; }
	QString experimentName(int sourceExperimentId) const { return sourceExperiments_.value(sourceExperimentId); }

	QMap<int, int> sampleIdMapping() const { return s2dSampleIds_; }
	void doNotMergeSample(int sourceSampleId) { if(s2dSampleIds_.contains(sourceSampleId)) s2dSampleIds_[sourceSampleId] = -1; }
	QString sampleName(int sourceSampleId) const { return sourceSamples_.value(sourceSampleId); }


signals:
	void stepProgress(int percentDone);
	void progressDescription(const QString& description);

public slots:

protected:
	QString sourcePath_, destinationPath_;
	AMDatabase* sourceDb_, *destinationDb_;

	QMap<int, int> s2dFacilityIds_;
	QMap<int, QString> sourceFacilities_;
	QMap<int, int> s2dRunIds_;
	QMap<int, QString> sourceRuns_;
	QMap<int, int> s2dExperimentIds_;
	QMap<int, QString> sourceExperiments_;
	QMap<int, int> s2dSampleIds_;
	QMap<int, QString> sourceSamples_;

	void copyFacilities();
	void copyRuns();
	void copyExperiments();
	void copySamples();
	void copyScans();

	/// Helper function to check if a \c file exists within the \c parentFolder, and returns a unique name that doesn't conflict with what's already on the file system.  Relative paths are OK within \c file.
	QString makeUniqueFileName(const QString& parentFolder, const QString& file);
	/// Helper function to check if a user wants to skip or duplicate a scan where the raw data seems to already exist in the destination. Returns true if we should skip the new scan, false if the user wants to keep both.
	bool userSaysShouldSkipDuplicateScan(AMScan* scan);
	/// Helper function to copy a source file to a destination file.  The paths should be absolute. If a complete set of folders doesn't exist yet to the destination, they will be created. Returns true on success.
	bool copyFileAndMakeFoldersAsRequired(const QString& sourcePath, const QString& destinationPath);


	AMScanDatabaseImportControllerState state_;
};

#endif // AMSCANDATABASEIMPORTCONTROLLER_H
