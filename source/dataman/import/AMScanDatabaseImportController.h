#ifndef AMSCANDATABASEIMPORTCONTROLLER_H
#define AMSCANDATABASEIMPORTCONTROLLER_H

#include <QObject>
#include <QMap>

class AMDatabase;

class AMScanDatabaseImportController : public QObject
{
    Q_OBJECT
public:
	explicit AMScanDatabaseImportController(AMDatabase* destinationDb, QObject *parent = 0);

	bool setSourceFolderAndLoadDatabase(const QString& absolutePathToSourceFolder);

	void analyzeFacilitiesForDuplicates();
	void analyzeRunsForDuplicates();
	void analyzeExperimentsForDuplicates();
	void analyzeSamplesForDuplicates();

	void startDatabaseOperations();

signals:
	void stepProgress(double percent);
	void progressDescription(const QString& description);

public slots:

protected:
	QString sourcePath_;
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



};

#endif // AMSCANDATABASEIMPORTCONTROLLER_H
