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


#ifndef IDEASXRFDETAILEDDETECTORVIEW_H
#define IDEASXRFDETAILEDDETECTORVIEW_H

#include "ui/beamline/AMXRFDetailedDetectorView.h"

<<<<<<< HEAD:source/ui/IDEAS/IDEASXRFDetailedDetectorViewWithSave.h
#include "acquaman/IDEAS/IDEASXRFScanConfiguration.h"
#include "actions3/actions/AMScanAction.h"


=======
>>>>>>> master:source/ui/IDEAS/IDEASXRFDetailedDetectorView.h
#include <QPushButton>
#include <QSignalMapper>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QtGui/QComboBox>

#include "acquaman/IDEAS/IDEASXRFScanConfiguration.h"
#include "actions3/actions/AMScanAction.h"
#include "ui/dataman/AMChooseScanDialog.h"

class AMExportController;

class IDEASXRFDetailedDetectorView : public AMXRFDetailedDetectorView
{
	Q_OBJECT

public:
	/// Constructor.
<<<<<<< HEAD:source/ui/IDEAS/IDEASXRFDetailedDetectorViewWithSave.h
	IDEASXRFDetailedDetectorViewWithSave(AMXRFDetector *detector, QWidget *parent = 0);

=======
	IDEASXRFDetailedDetectorView(AMXRFDetector *detector, QWidget *parent = 0);
>>>>>>> master:source/ui/IDEAS/IDEASXRFDetailedDetectorView.h
	/// Destructor.
	virtual ~IDEASXRFDetailedDetectorView();

	/// Re-implementing but still going to use the base class buildDetectorView since this view is merely adding to it.
	virtual void buildDetectorView();

signals:


protected slots:
	/// Starts the acquisition.  Calls acquire() but subclasses can reimplement if there is a more sofisticated start routine.
	virtual void startAcquisition();
	/// Handles bringing up and exporting the given XRF scans.
	void onSaveScanButtonClicked();
<<<<<<< HEAD:source/ui/IDEAS/IDEASXRFDetailedDetectorViewWithSave.h
	void onNotesTextChanged();
	void onScanNameChanged(QString name);
	void onScanNumberChanged(int number);
	void onAcquisitionSucceeded();
=======
	/// Handles grabbing the scan and exporting it.
	void exportScan();
	/// Handles deleting the export controller.
	void onExportControllerStateChanged(int state);

	void onPeakingTimeBoxChanged(const QString &arg1);
	void onKETEKPeakingTimeChanged();
>>>>>>> master:source/ui/IDEAS/IDEASXRFDetailedDetectorView.h
	void onDeadTimeCheckButtonClicked();


protected:
	/// Method that builds the Scan Save Button and associated things.
	void buildScanSaveViews();

	/// The button for saving the scan.
	QPushButton *saveScanButton_;
	/// Choose scan dialog.
	AMChooseScanDialog *chooseScanDialog_;
	/// The export controller.
	AMExportController *exportController_;

	/// button to trigger a 0.1s XRF acquitisition to check to too-high count rates.
	QPushButton *deadTimeCheckButton;

<<<<<<< HEAD:source/ui/IDEAS/IDEASXRFDetailedDetectorViewWithSave.h

	/// A  scan configuration notes editor
	QPlainTextEdit *notesEdit;

	QGridLayout *scanInfoGridLayout;
	QLabel *scanNameLabel;
	QLineEdit *scanName;
	QLabel *scanNumberLabel;
	QSpinBox *scanNumber;


	/// The scan configuration.  Contains some configuration details about the scan such as integration time, minimum energy, maximum energy.
	IDEASXRFScanConfiguration *config_;

        /// The scan action created when a scan is saved
        AMAction3* scanAction_;


=======
	QComboBox *peakingTimeBox;
>>>>>>> master:source/ui/IDEAS/IDEASXRFDetailedDetectorView.h
};

#endif // IDEASXRFDETAILEDDETECTORVIEW_H
