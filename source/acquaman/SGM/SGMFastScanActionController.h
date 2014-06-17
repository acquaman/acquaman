#ifndef SGMFASTSCANACTIONCONTROLLER_H
#define SGMFASTSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "acquaman/SGM/SGMFastScanConfiguration2013.h"
#include "dataman/AMUser.h"
#include "actions3/AMAction3.h"

#include <QThread>

#include "acquaman/AMScanActionControllerBasicFileWriter.h"
Q_DECLARE_METATYPE(AMScanActionControllerBasicFileWriter::FileWriterError)

class AMScanActionControllerScanAssembler;
class AMListAction3;

#define SGMFASTSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS 288003
#define SGMFASTSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE 288004
#define SGMFASTSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR 288005

/// Specific implementation for the SGM XAS fast scan.
class SGMFastScanActionController : public AMScanActionController
{
	Q_OBJECT

public:
	/// Constructor.  Takes in a SGMFastScanConfiguration2013.
	SGMFastScanActionController(SGMFastScanConfiguration2013 *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMFastScanActionController();

	/// Builds all the raw data sources and configures and starts up the file writer thread.
	virtual void buildScanController();

signals:
	/// Notifier that new information/data should be written to file.
	void requestWriteToFile(int fileRank, const QString &textToWrite);
	/// Notifier that tells the file writer that all file writing activities are done after a scan has finished and to close all file access.
	void finishWritingToFile();

protected slots:
	/// Handles dealing with file writer errors.
	void onFileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError error);
	/// Handles dealing with the file writer when it changes busy state.
	void onFileWriterIsBusy(bool isBusy);

	void onEverythingFinished();

protected:
	/// Re-implemented due to the extra complexity of setting up a fast scan.
	virtual bool startImplementation();
	/// Re-implemented due to the extra complexity of setting up a fast scan.
	virtual void cancelImplementation();
	/// Empty method since there are no extra steps in the build implementation.
	virtual void buildScanControllerImplementation() {}

	/// Handles the specific data output of the fast scan, which differs greatly from a step scan.
	bool event(QEvent *e);

	/// Handles writing out the header data for the data file.
	void writeHeaderToFile();
	/// Handles writing out the data for the data file.
	void writeDataToFiles();

	/// Creates the extensive list of initialization actions required to setup a fast scan.
	AMAction3* createInitializationActions();
	/// Creates the list of actions required to put the beamline back in a normal state.
	AMAction3* createCleanupActions();

	/// Helper function to generate the notes for this Scan
	QString buildNotes();

protected:
	SGMFastScanConfiguration2013 *configuration_;

	int encoderStartValue_;
	double spacingParam_;
	double c1Param_;
	double c2Param_;
	double sParam_;
	double thetaParam_;

	QMap< QString, QVector<double> > allDataMap_;
	AMnDIndex insertionIndex_;

	QThread *fileWriterThread_;
	bool fileWriterIsBusy_;

	bool goodInitialState_;
};

#endif // SGMFASTSCANACTIONCONTROLLER_H
