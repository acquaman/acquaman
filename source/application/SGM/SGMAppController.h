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


#ifndef SGMAPPCONTROLLER_H
#define SGMAPPCONTROLLER_H

#include "application/AMAppController.h"

class SGMSampleTransferView;
class AMSampleManagementWidget;
class CLSSIS3820ScalerView;
class AMDetectorView;
class AMScanConfigurationViewHolder;
class AMScanConfigurationViewHolder3;
class SGMXASScanConfigurationView;
class SGMFastScanConfigurationView;
class SGMSidebar;
class SGMSettingsMasterView;
class AMGithubManager;
class AMDetector;

class SGMAppController : public AMAppController {
	Q_OBJECT

public:
	/// This constructor should be empty. Call SGMAppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit SGMAppController(QObject* parent = 0);

	/// Destructor
	virtual ~SGMAppController() {}

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

public slots:
	virtual bool startupCreateDatabases();

	/// Re-implemented from AMAppController to create and register the SGM database with the database system.
	virtual bool startupRegisterDatabases();
	virtual bool startupPopulateNewDatabase();
	virtual bool startupLoadFromExistingDatabase();

protected slots:
	/// This slot catches changes in the current widget of the AMMainWindow. \c pane is the new current widget.  Re-implement to catch any widget-specific responses that you need here.
	/*! \note This only applies to panes that are currently docked within the main window.  If a pane has been undocked, no notification will be received when it becomes raised or activated by the user.
	  */
	virtual void onCurrentPaneChanged(QWidget* pane);

	void onSGMBeamlineConnected();
	void onSGMScalerConnected(bool connected);
	void onSGMAmptekSDD1Connected(bool connected);
	void onSGMAmptekSDD2Connected(bool connected);

	/// \todo comment this, David...
	void onCurrentScanControllerCreated();
	void onCurrentScanControllerDestroyed();
	void onCurrentScanControllerStarted();
	void onCurrentScanControllerFinished();

	void onActionSGMSettings();

	void onSGMBeamlineDetectorAvailabilityChanged(AMDetector *detector, bool isAvailable);

protected:
	bool startupSGMInstallActions();
	bool setupSGMConfigurationFiles();
	bool setupSGMPlugins();
	bool setupSGMExporterOptions();
	bool setupSGMPeriodicTable();

	bool setupSGMViews();

protected:

	SGMSampleTransferView *sampleTransferView_;
	AMSampleManagementWidget *samplePositionView_;
	CLSSIS3820ScalerView *sgmScalerView_;
	AMDetectorView *amptekSDD1View_;
	AMDetectorView *amptekSDD2View_;
	SGMXASScanConfigurationView *xasScanConfigurationView_;
	SGMFastScanConfigurationView *fastScanConfigurationView_;
	AMScanConfigurationViewHolder *xasScanConfigurationHolder_;
	AMScanConfigurationViewHolder *fastScanConfigurationHolder_;
	AMScanConfigurationViewHolder3 *xasScanConfigurationHolder3_;
	SGMSidebar *sgmSidebar_;

	/// Persistent view for SGMSettings
	SGMSettingsMasterView *sgmSettingsMasterView_;

	QString lastWaitingDetectors_;
};

#endif // SGMAPPCONTROLLER_H
