#ifndef AMSTEPSCANACTIONCONTROLLER_H
#define AMSTEPSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "acquaman/AMStepScanConfiguration.h"
#include "dataman/AMUser.h"

#include <QThread>
#include <QTimer>

class AMScanActionControllerScanAssembler;
class AMListAction3;

#include "acquaman/AMScanActionControllerBasicFileWriter.h"
Q_DECLARE_METATYPE(AMScanActionControllerBasicFileWriter::FileWriterError)

#define AMSTEPSCANACTIONCONTROLLER_COULD_NOT_ADD_DETECTOR 265000
#define AMSTEPSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS 265003
#define AMSTEPSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE 265004
#define AMSTEPSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR 265005

/// This class is the base class for all step based scan controllers.  It can handle any number of dimensions as long as they are all step axes.
class AMStepScanActionController : public AMScanActionController
{
	Q_OBJECT

public:
	/// Constructor.  Takes in an AMStepScanConfiguration.
	AMStepScanActionController(AMStepScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~AMStepScanActionController();

	/// Returns whether it is safe to be deleted.  This is importanat due to the multi-threaded nature of the file writing.
	virtual bool isReadyForDeletion() const;

	/// Method that builds all the general aspects, such as measurements and raw data sources, and the file writer capabilities for the scan controller.
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
	/// Helper slot that tells AMCDFDataStore to flush it's contents to disk.  This prevents it from corrupting itself.
	void flushCDFDataStoreToDisk();

protected:
	/// Implementation to ensure that the data acquisition event is caught and handled.
	virtual bool event(QEvent *e);

	/// Creates the scan assembler that builds all the actions used to run the scan.
	virtual void createScanAssembler();
	/// Creates the axis order for higher dimensional scans.  The default order is the axis order, but if a different order is desired, then you should reimplement this in subclasses.
	/*!
		Be sure to use scan_->rawData() for access to the scan axes and not the configuration, as they haven't been properly configured by the time this has been called.
	  */
	virtual void createAxisOrderMap();

	/// Method that writes out the header information into the scanning file.
	void writeHeaderToFile();
	/// Method that writes out the current data to the scanning file.
	void writeDataToFiles();

	/// Method that prefills the scan's raw data store for 2D and higher dimensionality scans.
	void prefillScanPoints();

	/// The assembler that takes in the region scan configuration and turns it into a tree of scanning actions.
	AMScanActionControllerScanAssembler *scanAssembler_;
	/// The pointer to the region scan configuration to provide the subclass information.
	AMStepScanConfiguration *stepConfiguration_;

	/// Flag for determining whether axis value comes from setpoint or feedback.  Should be set by subclasses.  Only valid for 1D scans.
	bool useFeedback_;

	/// The current index new data will be inserted into.
	AMnDIndex currentAxisValueIndex_;
	/// The current value for all axes.
	QVector<double> currentAxisValues_;
	/// The names of the axis controls.
	QStringList axisNames_;
	/// The axis order mapping.  The mapping is <scan axis name, desired index>.  As with any list, the index should be between 0 and scanRank()-1.
	QMap<QString, int> axisOrderMap_;

	/// Pointer to the thread that handles all the file writing.
	QThread *fileWriterThread_;
	/// Flag for keeping track of whether the file writer thread is busy or not.
	bool fileWriterIsBusy_;
	/// A timer used when using AMCDFDataStore.  After a timeout it flushes the contents to disk.
	QTimer flushToDiskTimer_;
};

#endif // AMSTEPSCANACTIONCONTROLLER_H
