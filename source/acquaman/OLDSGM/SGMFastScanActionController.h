/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
#define SGMFASTSCANACTIONCONTROLLER_FAILED_TO_WRITE_FILE 288006

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

protected slots:
	/// Handles dealing with file writer errors.
	void onFileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError error);

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

	/// Helper function to figure out which should be start and end values if up/down scanning is enabled
	void configureStartEndValues();

protected:
	SGMFastScanConfiguration2013 *configuration_;

	int encoderStartValue_;
	double spacingParam_;
	double c1Param_;
	double c2Param_;
	double sParam_;
	double thetaParam_;

	bool enableUpDownScanning_;
	double startEnergy_;
	double endEnergy_;
	int startUndulatorStep_;
	int undulatorRelativeStep_;

	QMap< QString, QVector<double> > allDataMap_;
	AMnDIndex insertionIndex_;

	bool goodInitialState_;
};

#endif // SGMFASTSCANACTIONCONTROLLER_H
