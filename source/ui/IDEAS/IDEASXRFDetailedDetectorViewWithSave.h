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


#ifndef IDEASXRFDETAILEDDETECTORVIEWWITHSAVE_H
#define IDEASXRFDETAILEDDETECTORVIEWWITHSAVE_H

#include "ui/beamline/AMXRFDetailedDetectorView.h"

#include "acquaman/IDEAS/IDEASXRFScanConfiguration.h"
#include "actions3/actions/AMScanAction.h"

#include <QPushButton>
#include <QSignalMapper>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QtGui/QComboBox>


class IDEASXRFDetailedDetectorViewWithSave : public AMXRFDetailedDetectorView
{
	Q_OBJECT

public:
	/// Constructor.
	IDEASXRFDetailedDetectorViewWithSave(AMXRFDetector *detector, QWidget *parent = 0);
	/// Destructor.
	virtual ~IDEASXRFDetailedDetectorViewWithSave();

	/// Re-implementing but still going to use the base class buildDetectorView since this view is merely adding to it.
	virtual void buildDetectorView();

protected slots:
	void onSaveScanButtonClicked();
	void onNotesTextChanged();
	void onScanNameChanged(QString name);
	void onScanNumberChanged(int number);
	void onPeakingTimeBoxChanged(const QString &arg1);
	void onAcquisitionSucceeded();
	void onKETEKPeakingTimeChanged();
	void onDeadTimeCheckButtonClicked();
	/**
	  * Handles the scan action being completed in some fashion (either failed,
	  * succeeded or cancelled). Sets the enabled status of the acquireButton to
	  * true, disconnects signals from the scan action, schedules it for deletion
	  * and then nulls our reference to it.
	  */
	void cleanupScanAction();
protected:
	/// Method that builds the Scan Save Button and associated things.
	void buildScanSaveViews();

	/// The button for saving the scan.
	QPushButton *saveScanButton_;

	/// button to trigger a 0.1s XRF acquitisition to check to too-high count rates.
	QPushButton *deadTimeCheckButton;


	/// A  scan configuration notes editor
	QPlainTextEdit *notesEdit;

	QGridLayout *scanInfoGridLayout;
	QLabel *scanNameLabel;
	QLineEdit *scanName;
	QLabel *scanNumberLabel;
	QSpinBox *scanNumber;
	QComboBox *peakingTimeBox;

	/// The scan configuration.  Contains some configuration details about the scan such as integration time, minimum energy, maximum energy.
	IDEASXRFScanConfiguration *config_;

        /// The scan action created when a scan is saved
        AMAction3* scanAction_;


};

#endif // IDEASXRFDETAILEDDETECTORVIEWWITHSAVE_H
