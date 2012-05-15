/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef VESPERSWORKFLOWASSISTANTVIEW_H
#define VESPERSWORKFLOWASSISTANTVIEW_H

#include <QWidget>
#include <QProgressBar>
#include <QSpinBox>
#include <QLabel>

#include "util/VESPERS/VESPERSWorkflowAssistant.h"

/*! This class builds a view from a VESPERSWorkflowAssistant.  It behaves similarly to the bottom bar in that it offers some
  scan controlling aspects.  It will contain the number of scans, a progress bar for the overall number of scans progress,
  and also provide some other rudimentary information about the scan currently scanning, such as the name.  If more information
  is required then it will also be added.

  Unless it is incredibly tacky, this view will only be visible when XANES/EXAFS scans are running.
  */
class VESPERSWorkflowAssistantView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds the view around the VESPERSWorkflowAssistant object passed into it.
	explicit VESPERSWorkflowAssistantView(VESPERSWorkflowAssistant *assistant, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Handles changes to the current scan index.
	void onCurrentScanChanged(int index);
	/// Handles changes to the scan configuration (ie: the scan set has changed).
	void onScanConfigurationChanged();
	/// Helper slot that handles telling the workflow assistant that the total number of scans has changed.
	void onTotalScansChanged(int num);
	/// Helper slot that sets the progress bar.
	void onProgressChanged(double progress);

protected:
	/// Pointer to the workflow assistant.
	VESPERSWorkflowAssistant *assistant_;
	/// The progress bar of all of the set of scans.
	QProgressBar *progressBar_;
	/// The total number of scans.
	QSpinBox *totalScans_;
	/// The label holding the name of the current scan.
	QLabel *name_;
	/// The label holding the current scan.
	QLabel *currentScan_;
};

#endif // VESPERSWORKFLOWASSISTANTVIEW_H
