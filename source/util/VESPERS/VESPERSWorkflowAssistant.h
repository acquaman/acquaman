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


#ifndef VESPERSWORKFLOWASSISTANT_H
#define VESPERSWORKFLOWASSISTANT_H

#include <QObject>

#include "ui/AMWorkflowManagerView.h"
#include "acquaman/VESPERS/VESPERSEXAFSScanConfiguration.h"

/*!
  This class acts as an assistant to the current version of the workflow to allow for multiple copies of a scan to be repeated.  The idea
  is that the number of scans can be quickly and easily changed, even if the scan is being run.  It listens to the workflow
  and adds scan actions based on the current scan and the total number defined in the scan.  The primary goal of this class is to
  provide the same seamless control of the workflow that is currently available while adding the ability for repeatable scans.  It should not
  hinder any of the current useability of the workflow, even if currently the workflow is hardly utilized with VESPERS at present.
  */
class VESPERSWorkflowAssistant : public QObject
{
	Q_OBJECT
public:
	/// Constructor.  Takes a pointer to the workflow.
	explicit VESPERSWorkflowAssistant(AMWorkflowManagerView *workflow, QObject *parent = 0);

	/// Returns the name of the current scan configuration.  Returns 0 if there is no scan to be viewed.
	/*! \note Currently returns only VESPERSEXAFSScanConfiguration because it is the only type of scan currently supported.
		If more scans become supported before the workfow is rewritten then this class will be extended.
		*/
	VESPERSEXAFSScanConfiguration *currentConfiguration() const { return config_; }

	/// Returns the name of the current scan configuration.
	QString configName() const { return config_ != 0 ? config_->name() : ""; }
	/// Returns the current total number of scans that should be executed.  Returns -1 if the the configuration is invalid.
	int totalScans() const { return config_ != 0 ? config_->numberOfScans() : -1; }
	/// Returns the current scan running.  Returns -1 if no scans are running.
	int currentScan() const { return currentScan_; }

signals:
	/// Notifier that the workflow is running or not.
	void workflowRunningChanged(bool);
	/// Notifier that the current number of scans changed.
	void currentScanChanged(int);
	/// Notifier that the configuration has changed.
	void configurationChanged();
	/// Notifier that the progress of the current scan has changed as a decimal 0 < % < 1.
	void progressChanged(double);

public slots:
	/// Sets the total number of scans for this configuration.  This also adds or removes scans from the workflow if required.
	void setTotalScans(int num);
	/// Sets the current progress.
	void onCurrentProgressChanged(double elapsed, double total) { emit progressChanged(elapsed/total); }
	/// Handles when scans are cancelled.  Need to delete all of the extra scans that have been setup.
	void onScanCancelled();

protected slots:
	/// Slot that listens to the workflow and updates internal pieces of this class.
	void listen();
	/// Helper slot that handles what needs to happen when the workflow changes running status.
	void onWorkflowRunningChanged(bool running);

protected:
	/// Helper function.  Adds extra scan actions to the workflow.  Adds \param num extra scans.
	void addExtraScanActions(int num);
	/// Helper function.  Removes extra scan actions from the workflow.  Removes \param num extra scans.
	void removeExtraScanActions(int num);

	/// Pointer to the workflow.
	AMWorkflowManagerView *workflow_;

	/// Current number of scans that have been scanned in the current set.
	int currentScan_;

	/// The current scan configuration.
	VESPERSEXAFSScanConfiguration *config_;

};

#endif // VESPERSWORKFLOWASSISTANT_H
