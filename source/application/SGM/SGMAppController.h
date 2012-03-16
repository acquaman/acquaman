/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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
class SGMXASScanConfigurationView;
class SGMFastScanConfigurationView;
class SGMSidebar;
class SGMSettingsMasterView;
class AMGithubManager;

class SGMAppController : public AMAppController {
	Q_OBJECT

public:
	/// This constructor should be empty. Call SGMAppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit SGMAppController(QObject* parent = 0);

	/// Destructor
	virtual ~SGMAppController() {}

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// Re-implemented from AMAppController to create and register the SGM database with the database system.
	virtual bool startupRegisterDatabases();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

public slots:
	virtual bool startupDatabaseUpgrades();

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

	void onActionSGMSettings();

protected:
	bool startupSGMInstallActions();
	bool setupSGMDatabase();
	bool setupSGMPeriodicTable();

	/// Upgrades an AMDbObject class originally called \c originalClassName to \c newClassName.
	/*! This takes care of the problem where a class used in the database needs to be renamed. There are several caveats on this one.
	  The function is only for classes of medium complexity. They can been in an index table, but they shouldn't be the indexed table (for example, a class that inherits AMDetectorInfo is okay, but this can't handle something like AMDetectorInfoSet).
	  This function will ensure that the AMDbObjectTypes_table is properply updated, as well as changing the name of the class table and its AMDbObjectType column.
	  The \c indexTableToColumnWhere map allows you to specify a mapping of tables and columns you want to make sure are updated.
	  For example, if you have an index table you can speficy the index table name and the column to check in. This will run through all of the row of that index table and update instances of the old class name to the new class name.
	  */
	bool dbObjectBecomes(const QString &originalClassName, const QString &newClassName, QMap<QString, QString> parentTablesToColumnNames, QMap<QString, int> indexTablesToIndexSide);

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
	SGMSidebar *sgmSidebar_;

	/// Persistent view for SGMSettings
	SGMSettingsMasterView *sgmSettingsMasterView_;
};

#endif // SGMAPPCONTROLLER_H
