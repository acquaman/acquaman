#ifndef AM2DSCANACTIONCONTROLLER_H
#define AM2DSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "dataman/AMUser.h"
#include "acquaman/AM2DScanConfiguration.h"

#include <QThread>
#include <QTimer>

class AMScanActionControllerScanAssembler;
class AMListAction3;

#include "acquaman/AMScanActionControllerBasicFileWriter.h"
Q_DECLARE_METATYPE(AMScanActionControllerBasicFileWriter::FileWriterError)

#define AM2DSCANACTIONCONTROLLER_CANNOT_CONVERT_CONFIGURATION 264002
#define AM2DSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS 264003
#define AM2DSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE 264004
#define AM2DSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR 264005

/// This class does most of the work of setting up a simple scan controller using the 2D scan configuration.
class AM2DScanActionController : public AMScanActionController
{
	Q_OBJECT

public:
	/// Constructor. Takes in an AM2DScanConfiguration.
	AM2DScanActionController(AM2DScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~AM2DScanActionController();

	/// Returns whether it is safe to be deleted.  This is important due to the multithreaded nature of the file writer.
	virtual bool isReadyForDeletion() const;
	/// Method that builds all the genearl aspects, sucah as measurements and raw data sources, and the file writer capabilities of the controller.
	virtual void buildScanController();

signals:
	/// Notifier that new infomration/data should be written to file.
	void requestWriteToFile(int fileRank, const QString &textToWrite);
	/// Notifier that tells the file writer that all file writing activities are done after a scan has finished.
	void finishWritingToFile();

protected slots:
	/// Handles dealing with file writer errors.
	void onFileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError error);
	/// Handles dealing with the file writer when it changes busy state.
	void onFileWriterBusy(bool isBusy);
	/// Helper slot that tells AMCDFDataStore to flush it's contents to disk.  This prevents it from corrupting the data files.
	void flushCDFDataStoreToDisk();

protected:
	/// Implementation to ensure that the data acquisition event is caught and handled.
	virtual bool event(QEvent *e);

	/// Method that writes out the header information into the scanning file.
	void writeHeaderToFile();
	/// Method that writes out the current data to the scanning file.
	void writeDataToFiles();

	/// The assembler that takes the scan configuration and turns it into a tree of scanning actions.
	AMScanActionControllerScanAssembler *newScanAssembler_;
	/// The pointer to the 2D scan configuration to provide the subclass information.
	AM2DScanConfiguration *configuration2D_;

	/// The current index new data will be inserted into.
	AMnDIndex currentAxisValueIndex_;
	/// The name of the x-axis control.
	QString xAxisName_;
	/// The name of the y-axis control.
	QString yAxisName_;

	/// Pointer to the thread that handles all the file writing.
	QThread *fileWriterThread_;
	/// Flag for keeping track of whether the file writer thread is busy or not.
	bool fileWriterIsBusy_;
	/// A timer used when using AMCDFDataStore.  After a timeout it flushes the contents to disk.
	QTimer flushToDiskTimer_;
};

#endif // AM2DSCANACTIONCONTROLLER_H
